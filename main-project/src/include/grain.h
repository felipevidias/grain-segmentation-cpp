/**
 * @file Grain.h
 * @brief Defines the data structures for representing individual grains and their network.
 *
 * This file contains the declaration of the `Grain` struct, which models a single
 * grain particle, and the `GrainNetwork` class, which manages the entire collection
 * of grains and provides an interface for loading and analyzing their properties
 * and contacts from data files.
 */

 #ifndef GRAIN_H
 #define GRAIN_H
 
 #include <vector>
 #include <string>
 #include <unordered_map>
 #include <utility> // For std::pair
 
 /**
  * @struct Grain
  * @brief Represents a single grain particle with its properties and connectivity.
  *
  * Stores the grain's unique label, its 3D coordinates, and a list of
  * pointers to its neighbors, forming a graph structure of grain contacts.
  */
 struct Grain {
     int label;          ///< Unique integer identifier for the grain.
     double x, y, z;     ///< The 3D coordinates of the grain's centroid.
     
     /**
      * @brief Pointers to neighboring grains.
      * Pointers are used to efficiently represent the graph of contacts without
      * duplicating Grain objects in memory.
      */
     std::vector<Grain*> neighbors;
 
     /**
      * @brief Default and parameterized constructor.
      * @param lbl The grain's label.
      * @param x_pos The x-coordinate of the grain.
      * @param y_pos The y-coordinate of the grain.
      * @param z_pos The z-coordinate of the grain.
      */
     Grain(int lbl = 0, double x_pos = 0.0, double y_pos = 0.0, double z_pos = 0.0);
 
     /**
      * @brief Appends a grain to the list of neighbors if it is not already present.
      * @param neighbor A pointer to the Grain object to be added as a neighbor.
      */
     void add_neighbor(Grain* neighbor);
 };
 
 /**
  * @class GrainNetwork
  * @brief Manages a collection of grains and their interconnections.
  *
  * This class serves as a high-level interface to the grain data. It handles
  * parsing from text files, stores the collection of all grains for efficient
  * lookup, and provides methods to analyze the network topology, such as
  * identifying connected and unconnected grains.
  */
 class GrainNetwork {
 public:
     /**
      * @brief Loads grain positions and properties from a tracking file.
      * @param filepath The path to the input text file. The file is expected to
      * have a header line, followed by lines of data with columns for label,
      * z-coordinate, y-coordinate, and x-coordinate.
      */
     void load_from_tracking_file(const std::string& filepath);
 
     /**
      * @brief Loads grain contacts from a file and establishes neighbor relationships.
      * @note This method should be called after `load_from_tracking_file`, as it
      * assumes the grains have already been loaded into the network.
      * @param filepath The path to the contact file. The file is expected to have
      * a header line, followed by lines containing two integer labels per line,
      * representing a contact between two grains.
      */
     void load_contacts(const std::string& filepath);
 
     /**
      * @brief Classifies all grains into connected and unconnected sets.
      * A grain is considered connected if its list of neighbors is not empty.
      * @return A `std::pair` containing two vectors of `Grain*`.
      * The `first` vector contains all connected grains.
      * The `second` vector contains all unconnected grains.
      */
     std::pair<std::vector<Grain*>, std::vector<Grain*>> get_connected_status();
 
     /**
      * @brief Retrieves a pointer to a specific grain by its label.
      * @param label The unique identifier of the grain to find.
      * @return A pointer to the `Grain` object if found, otherwise `nullptr`.
      */
     Grain* get_grain(int label);
 
 private:
     /**
      * @brief The primary data store for all Grain objects.
      * An `unordered_map` is used to provide efficient, average-case O(1)
      * lookup of grains by their integer label.
      */
     std::unordered_map<int, Grain> grains;
 };
 
 #endif // GRAIN_H