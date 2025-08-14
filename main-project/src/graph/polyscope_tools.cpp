/**
 * @file PolyscopeTools.cpp
 * @brief Implements the utility functions for the Polyscope visualizers.
 */

 #include "polyscope_tools.h"

 #include <filesystem>
 #include <algorithm>
 #include <iostream>
 
 // Use the C++17 filesystem library for directory operations.
 namespace fs = std::filesystem;
 
 std::vector<std::string> get_sorted_files(const std::string& directory_path) {
     std::vector<std::string> files;
     
     // Use a try-catch block to gracefully handle errors like a non-existent directory.
     try {
         for (const auto& entry : fs::directory_iterator(directory_path)) {
             if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                 files.push_back(entry.path().string());
             }
         }
     } catch (const fs::filesystem_error& e) {
         std::cerr << "Error accessing directory " << directory_path << ": " << e.what() << std::endl;
     }
     
     // Sort the collected file paths alphabetically.
     std::sort(files.begin(), files.end());
     return files;
 }
 
 std::pair<std::vector<std::array<size_t, 2>>, std::vector<std::array<size_t, 2>>>
 compare_edges(const std::vector<std::array<size_t, 2>>& current_edges,
               const std::set<std::pair<size_t, size_t>>& prev_edges_set) {
     
     std::vector<std::array<size_t, 2>> new_e, old_e;
 
     // If there is no previous state, all current edges are considered new.
     if (prev_edges_set.empty()) {
         new_e = current_edges;
     } else {
         // Classify each current edge as new or old by checking against the previous set.
         for (const auto& edge : current_edges) {
             // A canonical representation {min, max} ensures that edge (u,v) and (v,u) are treated as the same.
             std::pair<size_t, size_t> canonical_edge = {std::min(edge[0], edge[1]), std::max(edge[0], edge[1])};
             if (prev_edges_set.count(canonical_edge)) {
                 old_e.push_back(edge);
             } else {
                 new_e.push_back(edge);
             }
         }
     }
 
     // If a resulting edge list is empty, add a single dummy edge.
     // This is a robust way to handle cases where Polyscope might not
     // gracefully handle an empty edge container for an existing curve network.
     if (new_e.empty()) {
         new_e.push_back({0, 0});
     }
     if (old_e.empty()) {
         old_e.push_back({0, 0});
     }
 
     return {new_e, old_e};
 }