/**
 * @file polyscope_compare_grains.cpp
 * @brief Main application for visualizing and comparing grain network animations.
 *
 * This program loads two time-series datasets of grain positions, compares them
 * frame by frame, and visualizes the results using Polyscope. It provides an
 * interactive GUI to control the animation playback.
 */

// C++ Standard Library
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <map>
#include <algorithm>
#include <tuple>

// Polyscope
#include "polyscope/polyscope.h"
#include "polyscope/point_cloud.h"
#include "imgui.h"

// Project-specific headers
#include "grain.h"
#include "polyscope_tools.h" 

// --- Data Structures for Visualization ---

/**
 * @struct FrameData
 * @brief Holds all coordinate and label data for a single frame comparison.
 * This struct organizes the point cloud data that will be sent to Polyscope.
 */
struct FrameData {
    std::vector<std::array<double, 3>> points_diff1, points_diff2, points_same;
    std::vector<int> labels_diff1, labels_diff2, labels_same;
};

// --- Global State Variables for the GUI ---

std::vector<FrameData> all_frame_data;
int current_frame = 0;
int nb_frames = 0;
bool is_stopped = true;
float duration = 1.0f;
auto last_update_time = std::chrono::high_resolution_clock::now();


// --- Helper Functions ---

// Custom comparator for std::array to be used as a map key
struct ArrayComparator {
    bool operator()(const std::array<double, 3>& a, const std::array<double, 3>& b) const {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    }
};

/**
 * @brief Compares two grain networks and classifies grains based on their coordinates.
 * @param network1 The first grain network.
 * @param network2 The second grain network.
 * @return A tuple containing three vectors of Grain pointers: unique to network1, unique to network2, and common to both.
 */
std::tuple<std::vector<Grain*>, std::vector<Grain*>, std::vector<Grain*>>
get_points_diff_same(GrainNetwork& network1, GrainNetwork& network2) {
    std::map<std::array<double, 3>, Grain*, ArrayComparator> coords1;
    for (int i = 1; ; ++i) {
        Grain* g = network1.get_grain(i);
        if (!g) break;
        coords1[{g->x, g->y, g->z}] = g;
    }

    std::map<std::array<double, 3>, Grain*, ArrayComparator> coords2;
    for (int i = 1; ; ++i) {
        Grain* g = network2.get_grain(i);
        if (!g) break;
        coords2[{g->x, g->y, g->z}] = g;
    }

    std::vector<Grain*> diff1, diff2, same;

    for (const auto& pair : coords1) {
        if (coords2.find(pair.first) == coords2.end()) {
            diff1.push_back(pair.second);
        } else {
            same.push_back(pair.second);
        }
    }

    for (const auto& pair : coords2) {
        if (coords1.find(pair.first) == coords1.end()) {
            diff2.push_back(pair.second);
        }
    }

    return {diff1, diff2, same};
}

// Helper functions to extract data from Grain vectors
std::vector<std::array<double, 3>> get_coords_points(const std::vector<Grain*>& grains) {
    coords.reserve(grains.size());
    for (const auto& g : grains) {
        coords.push_back({g->x, g->y, g->z});
    }
    return coords;
}

std::vector<int> get_labels_points(const std::vector<Grain*>& grains) {
    std::vector<int> labels;
    labels.reserve(grains.size());
    for (const auto& g : grains) {
        labels.push_back(g->label);
    }
    return labels;
}

/**
 * @brief Processes two grain networks to generate the data for a single frame.
 * @param network1 Grain network for the first dataset at a specific time.
 * @param network2 Grain network for the second dataset at the same time.
 * @return A FrameData struct populated with comparison data.
 */
FrameData generate_points_for_frame(GrainNetwork& network1, GrainNetwork& network2) {
    auto [diff1, diff2, same] = get_points_diff_same(network1, network2);
    
    FrameData data;
    data.points_diff1 = get_coords_points(diff1);
    data.labels_diff1 = get_labels_points(diff1);
    data.points_diff2 = get_coords_points(diff2);
    data.labels_diff2 = get_labels_points(diff2);
    data.points_same = get_coords_points(same);
    data.labels_same = get_labels_points(same);
    
    return data;
}


// --- Polyscope Functions ---

/**
 * @brief Registers or updates a point cloud in Polyscope, handling empty data.
 */
void register_or_update_cloud(const std::string& name, const std::vector<std::array<double, 3>>& points, const std::vector<int>& labels, bool initially_enabled) {
    static const std::vector<std::array<double, 3>> dummy_point = {{0,0,0}};
    static const std::vector<int> dummy_label = {-1};

    if (!points.empty()) {
        auto* ps_cloud = polyscope::registerPointCloud(name, points);
        ps_cloud->addScalarQuantity("Labels", labels);
        ps_cloud->setEnabled(initially_enabled);
    } else {
        auto* ps_cloud = polyscope::registerPointCloud(name, dummy_point);
        ps_cloud->addScalarQuantity("Labels", dummy_label);
        ps_cloud->setEnabled(false); // Always disable empty clouds
    }
}

/**
 * @brief Updates the Polyscope display with the data from the current frame.
 * @param to_next_frame If true, increments the frame counter.
 */
void update_display(bool to_next_frame = true) {
    if (to_next_frame) {
        current_frame = (current_frame + 1) % nb_frames;
    }

    const auto& data = all_frame_data[current_frame];
    
    register_or_update_cloud("Grains from dataset 1 (Unique)", data.points_diff1, data.labels_diff1, true);
    register_or_update_cloud("Grains from dataset 2 (Unique)", data.points_diff2, data.labels_diff2, true);
    register_or_update_cloud("Grains (Common)", data.points_same, data.labels_same, true);
}


/**
 * @brief The main callback function for the ImGui user interface.
 */
void user_callback() {
    if (ImGui::Button(is_stopped ? "Start" : "Stop")) {
        is_stopped = !is_stopped;
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
        current_frame = 0;
        update_display(false);
        last_update_time = std::chrono::high_resolution_clock::now();
    }

    ImGui::Separator();
    ImGui::SliderFloat("Display Time (s)", &duration, 0.01f, 5.0f, "%.2f");
    ImGui::Separator();

    if (ImGui::SliderInt("Current Frame", &current_frame, 0, nb_frames - 1)) {
        update_display(false);
        last_update_time = std::chrono::high_resolution_clock::now();
    }

    if (!is_stopped) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - last_update_time);
        if (elapsed.count() >= duration) {
            update_display();
            last_update_time = current_time;
        }
    }
}

// --- Main Application Logic ---

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tracking_dir_1> <tracking_dir_2>" << std::endl;
        return 1;
    }

    std::string tracking_dir1 = argv[1];
    std::string tracking_dir2 = argv[2];

    std::cout << "Loading files..." << std::endl;
    auto files1 = get_sorted_files(tracking_dir1);
    auto files2 = get_sorted_files(tracking_dir2);

    if (files1.size() != files2.size()) {
        std::cerr << "Error: Directories do not contain the same number of files." << std::endl;
        return 1;
    }
    nb_frames = files1.size();

    std::vector<GrainNetwork> networks1(nb_frames), networks2(nb_frames);
    for (int i = 0; i < nb_frames; ++i) {
        networks1[i].load_from_tracking_file(files1[i]);
        networks2[i].load_from_tracking_file(files2[i]);
    }
    std::cout << "Files loaded!" << std::endl;

    std::cout << "Generating all frame parameters..." << std::endl;
    all_frame_data.reserve(nb_frames);
    for (int i = 0; i < nb_frames; ++i) {
        all_frame_data.push_back(generate_points_for_frame(networks1[i], networks2[i]));
    }
    std::cout << "Parameters generated!" << std::endl;

    polyscope::init();
    
    // Set up colors and appearance
    polyscope::getPointCloud("Grains from dataset 1 (Unique)")->setColor({0.0, 0.0, 1.0}); // Blue
    polyscope::getPointCloud("Grains from dataset 2 (Unique)")->setColor({1.0, 0.0, 0.0}); // Red
    polyscope::getPointCloud("Grains (Common)")->setColor({0.0, 1.0, 0.0});                // Green

    polyscope::getPointCloud("Grains from dataset 1 (Unique)")->setRadius(0.0025);
    polyscope::getPointCloud("Grains from dataset 2 (Unique)")->setRadius(0.0025);
    polyscope::getPointCloud("Grains (Common)")->setRadius(0.0025);

    update_display(false); // Initial display
    polyscope::setUserCallback(user_callback);
    polyscope::show();

    return 0;
}