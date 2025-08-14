/**
 * @file polyscope_follow_grains_global.cpp
 * @brief Visualizes a single grain network animation over time.
 *
 * This application loads a time-series dataset of grain positions and contacts.
 * It displays connected and unconnected grains and visualizes the contact network
 * as curves. It also highlights new connections that appear at each frame.
 */

// C++ Standard Library
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <utility> // For std::pair

// Polyscope
#include "polyscope/polyscope.h"
#include "polyscope/point_cloud.h"
#include "polyscope/curve_network.h"
#include "imgui.h"

// Project-specific headers
#include "grain.h"
#include "polyscope_tools.h" 

// --- Data Structures for Visualization ---

/**
 * @struct FrameData
 * @brief Holds all processed data for a single frame to be displayed in Polyscope.
 */
struct FrameData {
    // Data for connected grains
    std::vector<std::array<double, 3>> points_connected;
    std::vector<int> labels_connected;
    std::vector<size_t> neighbor_count;

    // Data for unconnected grains
    std::vector<std::array<double, 3>> points_unconnected;
    std::vector<int> labels_unconnected;

    // Edge data for curve networks
    std::vector<std::array<size_t, 2>> new_edges;
    std::vector<std::array<size_t, 2>> old_edges;
};


// --- Global State Variables ---

std::vector<FrameData> all_frame_data;
int current_frame = 0;
int nb_frames = 0;
bool is_stopped = true;
float duration = 1.0f;
auto last_update_time = std::chrono::high_resolution_clock::now();


// --- Helper Functions ---


/**
 * @brief Generates all visualization data for a single frame from a GrainNetwork.
 * @param network The grain network for the current frame.
 * @param prev_edges_set A reference to the set of edges from the previous frame.
 * @return A FrameData struct populated with the visualization data.
 */
FrameData generate_points_for_frame(GrainNetwork& network, std::set<std::pair<size_t, size_t>>& prev_edges_set) {
    FrameData data;
    auto [connected_grains, unconnected_grains] = network.get_connected_status();

    // Process connected grains
    data.points_connected.reserve(connected_grains.size());
    data.labels_connected.reserve(connected_grains.size());
    data.neighbor_count.reserve(connected_grains.size());
    std::map<Grain*, size_t> grain_to_idx;
    size_t idx = 0;
    for (Grain* g : connected_grains) {
        data.points_connected.push_back({g->x, g->y, g->z});
        data.labels_connected.push_back(g->label);
        data.neighbor_count.push_back(g->neighbors.size());
        grain_to_idx[g] = idx++;
    }

    // Process unconnected grains
    data.points_unconnected.reserve(unconnected_grains.size());
    data.labels_unconnected.reserve(unconnected_grains.size());
    for (Grain* g : unconnected_grains) {
        data.points_unconnected.push_back({g->x, g->y, g->z});
        data.labels_unconnected.push_back(g->label);
    }

    // Build current edges and compare with previous frame
    std::vector<std::array<size_t, 2>> current_edges_vec;
    for (Grain* g : connected_grains) {
        for (Grain* neighbor : g->neighbors) {
            if (grain_to_idx.count(neighbor)) {
                current_edges_vec.push_back({grain_to_idx[g], grain_to_idx[neighbor]});
            }
        }
    }

    // Utiliza a função centralizada do PolyscopeTools
    auto [new_edges, old_edges] = compare_edges(current_edges_vec, prev_edges_set);
    data.new_edges = new_edges;
    data.old_edges = old_edges;

    // Update prev_edges_set for the next frame
    prev_edges_set.clear();
    for(const auto& edge : current_edges_vec) {
        prev_edges_set.insert({std::min(edge[0], edge[1]), std::max(edge[0], edge[1])});
    }

    return data;
}


// --- Polyscope Functions ---

void update_display(bool to_next_frame = true) {
    if (to_next_frame) {
        current_frame = (current_frame + 1) % nb_frames;
    }
    const auto& data = all_frame_data[current_frame];

    // Register point clouds
    polyscope::PointCloud* ps_cloud_co;
    if (!data.points_connected.empty()) {
        ps_cloud_co = polyscope::registerPointCloud("Grains connected", data.points_connected);
        ps_cloud_co->addScalarQuantity("Labels", data.labels_connected);
        ps_cloud_co->addScalarQuantity("Neighbor Count", data.neighbor_count);
    } else {
        ps_cloud_co = polyscope::registerPointCloud("Grains connected", std::vector<glm::vec3>{{0,0,0}});
        ps_cloud_co->setEnabled(false);
    }
    
    if (!data.points_unconnected.empty()) {
        polyscope::registerPointCloud("Grains unconnected", data.points_unconnected);
    } else {
        polyscope::registerPointCloud("Grains unconnected", std::vector<glm::vec3>{{0,0,0}})->setEnabled(false);
    }

    // Register curve networks for edges
    // Usa um ponto dummy se a lista de pontos estiver vazia para evitar erro
    auto points_for_curves = data.points_connected.empty() ? std::vector<glm::vec3>{{0,0,0}} : data.points_connected;
    polyscope::registerCurveNetwork("Old Links", points_for_curves, data.old_edges);
    polyscope::registerCurveNetwork("New Links", points_for_curves, data.new_edges);
}


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
        std::cerr << "Usage: " << argv[0] << " <tracking_dir> <contact_dir>" << std::endl;
        return 1;
    }

    std::string tracking_dir = argv[1];
    std::string contact_dir = argv[2];

    // Utiliza a função centralizada do PolyscopeTools
    auto tracking_files = get_sorted_files(tracking_dir);
    auto contact_files = get_sorted_files(contact_dir);

    // ... o resto do main continua igual
    if (tracking_files.size() != contact_files.size()) {
        std::cerr << "Error: Tracking and contact directories must have the same number of files." << std::endl;
        return 1;
    }
    
    nb_frames = tracking_files.size() > 1 ? tracking_files.size() - 1 : 0;

    std::vector<GrainNetwork> networks(nb_frames);
    std::cout << "Loading " << nb_frames << " frames..." << std::endl;
    for (int i = 0; i < nb_frames; ++i) {
        networks[i].load_from_tracking_file(tracking_files[i]);
        networks[i].load_contacts(contact_files[i]);
        if ((i + 1) < contact_files.size()) {
            networks[i].load_contacts(contact_files[i + 1]);
        }
    }
    std::cout << "Files loaded!" << std::endl;

    std::cout << "Generating all frame parameters..." << std::endl;
    all_frame_data.reserve(nb_frames);
    std::set<std::pair<size_t, size_t>> prev_edges_set;
    for (int i = 0; i < nb_frames; ++i) {
        all_frame_data.push_back(generate_points_for_frame(networks[i], prev_edges_set));
    }
    std::cout << "Parameters generated!" << std::endl;

    polyscope::init();
    
    update_display(false);
    
    polyscope::getPointCloud("Grains connected")->setColor({0.2, 0.2, 0.8});
    polyscope::getPointCloud("Grains unconnected")->setColor({0.2, 0.8, 0.2});
    polyscope::getCurveNetwork("Old Links")->setColor({0.8, 0.2, 0.8});
    polyscope::getCurveNetwork("New Links")->setColor({0.9, 0.9, 0.9});
    
    polyscope::getPointCloud("Grains connected")->setRadius(0.0025);
    polyscope::getPointCloud("Grains unconnected")->setRadius(0.0025);
    polyscope::getCurveNetwork("Old Links")->setRadius(0.0003);
    polyscope::getCurveNetwork("New Links")->setRadius(0.0004);

    polyscope::setUserCallback(user_callback);
    polyscope::show();

    return 0;
}