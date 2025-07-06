#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

/**
 * @brief A struct containing static string constants for ANSI terminal styling.
 */
struct style {
    static const std::string RED;
    static const std::string BLUE;
    static const std::string PURPLE;
    static const std::string GREEN;
    static const std::string NO_COL; // Resets color
    static const std::string BOLD;
};

/**
 * @brief Displays a simple, non-blocking loading animation in the terminal.
 *
 * This class launches a background thread to render a spinner animation, allowing
 * the main thread to perform work without being blocked. The animation's state
 * can be controlled from the main thread.
 */
class TermLoading {
public:
    /**
     * @brief Initializes and launches the background animation thread.
     */
    TermLoading();

    /**
     * @brief Safely signals the background thread to terminate and waits for it to join.
     */
    ~TermLoading();

    /**
     * @brief Starts or restarts the loading animation with a new message.
     * @param loading_message The message to display next to the spinner.
     * @param finish_message The success message to display when set_finished() is called.
     * @param failed_message The failure message to display when set_failed() is called.
     */
    void show(const std::string& loading_message,
              const std::string& finish_message = "Finished",
              const std::string& failed_message = "Failed");

    /**
     * @brief Stops the current animation and displays the success message.
     */
    void set_finished();

    /**
     * @brief Stops the current animation and displays the failure message.
     */
    void set_failed();

private:
    /**
     * @brief The function executed by the background thread. Manages the animation lifecycle.
     */
    void loading_loop();

    std::string message_;
    std::string finish_message_;
    std::string failed_message_;

    std::thread thread_;
    std::mutex mtx_;
    std::condition_variable cv_;
    
    std::atomic<bool> running_{true};      // Controls the overall lifetime of the thread.
    std::atomic<bool> active_{false};      // Controls whether the animation is currently spinning.
    std::atomic<bool> task_failed_{false}; // Stores the final state of the task.
};

/**
 * @brief Prints a bold, red error message to the standard error stream (stderr).
 * @param message The error message content to display.
 */
void print_err(const std::string& message);