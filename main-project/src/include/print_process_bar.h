/**
 * @file print_process_bar.h
 * @brief Declares a utility function for displaying a simple text-based progress bar.
 */

 #ifndef PRINT_PROCESS_BAR_H
 #define PRINT_PROCESS_BAR_H
 
 #include <string>
 
 /**
  * @brief Prints or updates a progress bar in the console.
  */
 void print_progress_bar(
     int iteration,
     int total,
     const std::string& current_operation = "",
     int length = 50);
 
 #endif // PRINT_PROCESS_BAR_H