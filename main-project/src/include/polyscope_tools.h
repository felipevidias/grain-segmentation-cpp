/**
 * @file polyscope_tools.h
 * @brief Declares common utility functions for Polyscope-based visualizers.
 *
 * This file provides the declarations for helper functions used across different
 * visualizer executables, such as file system operations and data comparison logic.
 */

 #ifndef POLYSCOPE_TOOLS_H
 #define POLYSCOPE_TOOLS_H
 
 #include <vector>
 #include <string>
 #include <array>
 #include <set>
 #include <utility> // For std::pair
 
 /**
  * @brief Lists, filters for .txt extension, and sorts file paths in a directory.
  * @param directory_path The string path to the directory to scan.
  * @return A std::vector<std::string> containing the sorted, full paths to the text files.
  */
 std::vector<std::string> get_sorted_files(const std::string& directory_path);
 
 /**
  * @brief Compares a set of current edges against a set of previous edges to classify them as new or old.
  * @param current_edges A vector of edges for the current frame, represented as [index1, index2] pairs.
  * @param prev_edges_set A std::set containing canonical representations of edges from the 
  * previous frame for efficient O(log N) lookup.
  * @return A std::pair of vectors. The `first` element contains new edges, and the `second` contains old (persistent) edges.
  * @note If either the new or old edge list is empty, it will be populated with a single
  * dummy edge `{0, 0}`. This ensures that no completely empty containers are passed
  * to Polyscope, which can prevent potential rendering issues.
  */
 std::pair<std::vector<std::array<size_t, 2>>, std::vector<std::array<size_t, 2>>>
 compare_edges(const std::vector<std::array<size_t, 2>>& current_edges,
               const std::set<std::pair<size_t, size_t>>& prev_edges_set);
 
 #endif // POLYSCOPE_TOOLS_H