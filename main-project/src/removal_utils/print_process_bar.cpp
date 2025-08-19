/**
 * @file print_process_bar.cpp
 * @brief Implements the progress bar utility function.
 */

 #include "print_process_bar.h"
 #include <iostream>
 #include <string>
 #include <iomanip> // For std::fixed and std::setprecision
 
 void print_progress_bar(int iteration, int total, const std::string& current_operation, int length) {
     // Calculate percentage and bar length
     float percent = 100.0f * (static_cast<float>(iteration) / total);
     int filled_length = static_cast<int>(length * iteration / total);
 
     // Construct the bar string
     std::string bar_str = "";
     for (int i = 0; i < length; ++i) {
         if (i < filled_length) {
             bar_str += "█";
         } else {
             bar_str += "-";
         }
     }
 
     // Print the progress bar to the same line
     std::cout << "\r|" << bar_str << "| "
               << std::fixed << std::setprecision(1) << percent << "% Complete - ("
               << current_operation << ")" << std::flush;
 
     // Go to the next line if the task is done
     if (iteration == total) {
         std::cout << std::endl;
     }
 }