/**
 * @file dstyle.h
 * @brief Declares utilities for styling terminal output, including colors and animations.
 */

 #ifndef DSTYLE_H
 #define DSTYLE_H
 
 #include <string>
 #include <thread>
 #include <atomic>
 #include <mutex>
 #include <condition_variable>
 
 // Provides ANSI escape codes for terminal colors
 namespace style {
     const std::string RED    = "\033[0;31m";
     const std::string BLUE   = "\033[0;34m";
     const std::string PURPLE = "\033[0;35m";
     const std::string GREEN  = "\033[0;32m";
     const std::string NO_COL = "\033[0m";
     const std::string BOLD   = "\033[1m";
     const std::string NORMAL = "\033[0m";
 }
 
 /**
  * @class TerminalAnimator
  * @brief Manages a loading animation in a separate thread for command-line applications.
  */
 class TerminalAnimator {
 public:
     TerminalAnimator();
     ~TerminalAnimator();
 
     /**
      * @brief Starts displaying an animation with a specific message.
      * @param loading_message The message to display during the animation.
      * @param finish_message The message to display upon successful completion.
      * @param failed_message The message to display upon failure.
      */
     void show(const std::string& loading_message,
               const std::string& finish_message = "✅ Finished",
               const std::string& failed_message = "❌ Failed");
 
     /**
      * @brief Stops the animation and displays the success message.
      */
     void succeed();
 
     /**
      * @brief Stops the animation and displays the failure message.
      */
     void fail();
 
 private:
     void animation_loop();
 
     std::thread animator_thread;
     std::atomic<bool> is_running;
     std::mutex mtx;
     std::condition_variable cv;
 
     std::string current_message;
     std::string success_message;
     std::string failure_message;
     bool task_failed = false;
 };
 
 #endif // DSTYLE_H