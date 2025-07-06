#include "include/contact_detection_by_extending_labels.hpp"
#include "include/common.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For system()
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <tuple>

// --- Internal Helper Structures & Functions ---

/**
 * @brief A simple struct to hold 3D integer coordinates.
 * Overloads operator< to allow storage in std::set.
 */
struct Voxel {
    int i, j, k;

    bool operator<(const Voxel& other) const {
        return std::tie(i, j, k) < std::tie(other.i, other.j, other.k);
    }
};

// --- I/O Placeholders (TO BE IMPLEMENTED) ---

// IMPORTANT: This is a placeholder. A real implementation requires a TIFF/RAW library.
Image3D loadTiffImage(const std::string& path) {
    std::cout << "WARNING: Function 'loadTiffImage' is a placeholder. Returning empty image." << std::endl;
    return {};
}
Image3D loadRawImage(const std::string& path, int x, int y, int z) {
    std::cout << "WARNING: Function 'loadRawImage' is a placeholder. Returning empty image." << std::endl;
    return {};
}
std::map<int, Voxel> loadCentroids(const std::string& path) {
    std::map<int, Voxel> centroids;
    std::ifstream file(path);
    std::string line;
    if (!file.is_open()) {
        std::cerr << "Error opening centroids file: " << path << std::endl;
        return centroids;
    }
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        Voxel v;
        int label;
        std::getline(ss, value, ','); v.i = std::stoi(value);
        std::getline(ss, value, ','); v.j = std::stoi(value);
        std::getline(ss, value, ','); v.k = std::stoi(value);
        std::getline(ss, value, ','); label = std::stoi(value);
        centroids[label] = v;
    }
    return centroids;
}

// Helper to get 6-connectivity neighbors for a given voxel.
std::vector<Voxel> get_neighbors(const Voxel& v) {
    return {
        {v.i - 1, v.j, v.k}, {v.i + 1, v.j, v.k},
        {v.i, v.j - 1, v.k}, {v.i, v.j + 1, v.k},
        {v.i, v.j, v.k - 1}, {v.i, v.j, v.k + 1}
    };
}


// --- Main Module Logic ---

void run_contact_detection_by_extending_labels() {
    
    // --- 1. Argument Parsing (Hardcoded Placeholders) ---
    std::string grainsPath = "../data/grains.tif";
    int x = 100, y = 100, z = 100;
    std::string threshold = "27000";
    bool keep_files = false;
    std::string pinkDir = "../Pink/linux/bin/";
    std::string outputPath = "../results/contacts_extending_labels.csv";

    // --- 2. Pre-processing via External Tools ---
    std::cout << "--- Module: Contact Detection by Extending Labels ---" << std::endl;
    std::cout << "Starting pre-processing using external scripts..." << std::endl;
    
    system("mkdir -p tmp");
    system(("python3 ../utils/minTree.py " + grainsPath + " 6 --output=tmp/minTree.tif").c_str());
    system("python3 ../utils/tiff2raw.py tmp/minTree.tif");
    system((pinkDir + "raw2pgm tmp/minTree.raw " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " 0 1 0 tmp/minTree.pgm").c_str());
    system(("python3 ../utils/getCentroid.py " + grainsPath + " tmp/minTree.tif --output=tmp/centroids.csv").c_str());
    system(("python3 ../utils/tiff_binarization.py " + grainsPath + " --threshold=" + threshold + " --output=tmp/grains_binarized.tif").c_str());
    system("python3 ../utils/tiff_binary_sum.py tmp/grains_binarized.tif tmp/minTree.tif --output=tmp/grains_binarized.tif");
    system("python3 ../utils/tiff2raw.py tmp/grains_binarized.tif");
    system((pinkDir + "raw2pgm tmp/grains_binarized.raw " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " 0 1 0 tmp/grains_binarized.pgm").c_str());
    system((pinkDir + "skeleton tmp/grains_binarized.pgm 6 6 tmp/minTree.pgm tmp/skeleton.pgm").c_str());
    system((pinkDir + "pgm2raw tmp/skeleton").c_str());

    std::cout << "Pre-processing complete." << std::endl;

    // --- 3. Data Loading ---
    std::cout << "Loading data..." << std::endl;
    Image3D grains = loadTiffImage("tmp/grains_binarized.tif");
    Image3D minTree = loadTiffImage("tmp/minTree.tif");
    Image3D skeleton = loadRawImage("tmp/skeleton.raw", x, y, z);
    
    if (grains.data.empty() || minTree.data.empty() || skeleton.data.empty()) {
        std::cerr << "Critical Error: Failed to load one or more images. Aborting." << std::endl;
        return;
    }

    // --- 4. Contact Detection: Initial Seeding ---
    std::cout << "Starting contact detection (seeding phase)..." << std::endl;

    std::map<int, std::set<Voxel>> initialVisited;
    std::map<int, std::set<Voxel>> initialToVisit;

    auto centroids = loadCentroids("tmp/centroids.csv");

    // Initialize the core of each grain based on its centroid.
    for(const auto& pair : centroids) {
        int label = pair.first;
        Voxel v = pair.second;
        initialVisited[label].insert(v);
        std::deque<Voxel> kernel_q;
        auto neighbors = get_neighbors(v);
        // ... (rest of the initial propagation logic)
    }
    
    // --- 5. Contact Detection: Main Iterative Loop ---
    std::cout << "Starting contact detection (main loop)..." << std::endl;
    
    std::map<std::pair<int, int>, int> contactsStrength;
    Image3D erodedGrains = grains;
    bool havingContacts = true;
    int contactStrength = 0;

    // This loop erodes the image and re-checks for contacts to determine their strength.
    while(havingContacts) {
        contactStrength++;
        std::cout << "Erosion level (Contact Strength): " << contactStrength << std::endl;
        // ... (rest of the main contact detection loop logic)
    }

    // --- 6. Cleanup & Saving Results ---
    if (!keep_files) {
        system("rm -r tmp");
    }

    std::cout << "Saving results..." << std::endl;
    save_results(contactsStrength, outputPath); // This function is from common.hpp

    std::cout << "--- Module Finished: Contact Detection by Extending Labels ---" << std::endl;
}