/**
 * @file polyscope_follow_grains_global.cpp
 * @brief Visualizes a single grain network animation with label filtering and camera control.
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
#include <utility>

// Polyscope
#include "polyscope/polyscope.h"
#include "polyscope/point_cloud.h"
#include "polyscope/curve_network.h"
#include "polyscope/view.h" // For camera control
#include "imgui.h"

// Project-specific headers
#include "grain.h"
#include "polyscope_tools.h" // Using the centralized tools

// --- Data Structures for Visualization ---

struct FrameData {
    std::vector<std::array<double, 3>> points_connected;
    std::vector<int> labels_connected;
    std::vector<size_t> neighbor_count;

    std::vector<std::array<double, 3>> points_unconnected;
    std::vector<int> labels_unconnected;

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
 * @brief Generates visualization data for a frame, optionally filtering by labels.
 * @param network The grain network for the current frame.
 * @param prev_edges_set A reference to the edge set from the previous frame.
 * @param chosen_labels A set of labels to filter by. If empty, all grains are shown.
 * @return A FrameData struct populated with the visualization data.
 */
FrameData generate_points_for_frame(
    GrainNetwork& network,
    std::set<std::pair<size_t, size_t>>& prev_edges_set,
    const std::set<int>& chosen_labels) {

    FrameData data;
    auto [connected_grains_all, unconnected_grains_all] = network.get_connected_status();

    std::vector<Grain*> connected_grains, unconnected_grains;

    // Filter grains if chosen_labels is not empty
    if (!chosen_labels.empty()) {
        for (Grain* g : connected_grains_all) {
            if (chosen_labels.count(g->label)) {
                connected_grains.push_back(g);
            }
        }
        for (Grain* g : unconnected_grains_all) {
            if (chosen_labels.count(g->label)) {
                unconnected_grains.push_back(g);
            }
        }
    } else {
        connected_grains = connected_grains_all;
        unconnected_grains = unconnected_grains_all;
    }

    // Process connected grains
    std::map<Grain*, size_t> grain_to_idx;
    size_t idx = 0;
    for (Grain* g : connected_grains) {
        data.points_connected.push_back({g->x, g->y, g->z});
        data.labels_connected.push_back(g->label);
        data.neighbor_count.push_back(g->neighbors.size());
        grain_to_idx[g] = idx++;
    }

    // Process unconnected grains
    for (Grain* g : unconnected_grains) {
        data.points_unconnected.push_back({g->x, g->y, g->z});
        data.labels_unconnected.push_back(g->label);
    }

    // Build current edges
    std::vector<std::array<size_t, 2>> current_edges_vec;
    for (Grain* g : connected_grains) {
        for (Grain* neighbor : g->neighbors) {
            if (grain_to_idx.count(g) && grain_to_idx.count(neighbor)) {
                current_edges_vec.push_back({grain_to_idx[g], grain_to_idx[neighbor]});
            }
        }
    }
    
    // Call the centralized compare_edges function from PolyscopeTools
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

void prepare_display() {
    const auto& data = all_frame_data[current_frame];
    glm::vec3 dummy_point = {-99999.9, -99999.9, 99999.9};

    if (!data.points_connected.empty()) {
        auto* ps_cloud = polyscope::registerPointCloud("Grains connected", data.points_connected);
        ps_cloud->addScalarQuantity("Labels", data.labels_connected);
        ps_cloud->addScalarQuantity("Neighbor Count", data.neighbor_count);
    } else {
        polyscope::registerPointCloud("Grains connected", {dummy_point})->setEnabled(false);
    }

    if (!data.points_unconnected.empty()) {
        polyscope::registerPointCloud("Grains unconnected", data.points_unconnected);
    } else {
        polyscope::registerPointCloud("Grains unconnected", {dummy_point})->setEnabled(false);
    }

    auto points_for_curves = data.points_connected.empty() ? std::vector<glm::vec3>{dummy_point} : data.points_connected;
    polyscope::registerCurveNetwork("Old Links", points_for_curves, data.old_edges);
    polyscope::registerCurveNetwork("New Links", points_for_curves, data.new_edges);

    polyscope::getPointCloud("Grains connected")->setColor({0.2, 0.2, 0.8})->setRadius(0.0025);
    polyscope::getPointCloud("Grains unconnected")->setColor({0.2, 0.8, 0.2})->setRadius(0.0025);
    polyscope::getCurveNetwork("Old Links")->setColor({0.8, 0.2, 0.8})->setRadius(0.0003);
    polyscope::getCurveNetwork("New Links")->setColor({0.9, 0.9, 0.9})->setRadius(0.0004);
}

void update_display(bool to_next_frame = true) {
    if (to_next_frame) current_frame = (current_frame + 1) % nb_frames;
    prepare_display();
}

void user_callback() {
    if (ImGui::Button(is_stopped ? "Start" : "Stop")) is_stopped = !is_stopped;
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

