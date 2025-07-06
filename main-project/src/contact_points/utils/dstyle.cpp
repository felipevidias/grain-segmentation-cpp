#include "dstyle.hpp"
#include <iostream>
#include <vector>
#include <chrono>

// --- ANSI Style Constant Definitions ---
const std::string style::RED    = "\033[0;31m";
const std::string style::BLUE   = "\033[0;34m";
const std::string style::PURPLE = "\033[0;35m";
const std::string style::GREEN  = "\033[0;32m";
const std::string style::NO_COL = "\033[0m";
const std::string style::BOLD   = "\033[1m";


// --- TermLoading Class Implementation ---

TermLoading::TermLoading() {
    // The thread is launched upon construction and immediately waits.
    thread_ = std::thread(&TermLoading::loading_loop, this);
}

TermLoading::~TermLoading() {
    // Safely signal the thread to shut down.
    {
        std::lock_guard<std::mutex> lock(mtx_);
        running_ = false;
        active_ = true; // Wake up the thread so it can check the `running_` flag.
    }
    cv_.notify_one();

    // Wait for the thread to finish its execution.
    if (thread_.joinable()) {
        thread_.join();
    }
}

void TermLoading::show(const std::string& loading_message, const std::string& finish_message, const std::string& failed_message) {
    // Set up the messages and wake up the thread to start the animation.
    std::lock_guard<std::mutex> lock(mtx_);
    message_ = loading_message;
    finish_message_ = finish_message;
    failed_message_ = failed_message;
    task_failed_ = false;
    active_ = true;
    cv_.notify_one(); // Wakes up the sleeping thread in loading_loop.
}

void TermLoading::set_finished() {
    task_failed_ = false;
    active_ = false; // Signals the animation loop to stop.
    // A brief pause to ensure the final message prints after the animation stops.
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}

void TermLoading::set_failed() {
    task_failed_ = true;
    active_ = false; // Signals the animation loop to stop.
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}

void TermLoading::loading_loop() {
    const std::vector<std::string> symbols = {"[   ]", "[.  ]", "[.. ]", "[...]"};
    int i = 0;

    // The thread's main lifecycle loop.
    while (running_) {
        // --- Waiting State ---
        {
            // The thread sleeps here until `show()` notifies it.
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this]{ return active_.load(); });
            
            // If woken up to terminate, exit the loop.
            if (!running_) break;
        }

        // --- Animation State ---
        while (active_) {
            i = (i + 1) % symbols.size();
            // `\r` returns the cursor to the start of the line without a newline.
            std::cout << "\r" << style::BLUE << message_ << " " << symbols[i] << style::NO_COL << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // --- Finalization State ---
        // `\r\033[K` returns the cursor and clears the entire line.
        std::cout << "\r\033[K"; 
        if (task_failed_) {
            std::cout << style::BOLD << style::RED << failed_message_ << style::NO_COL << std::endl;
        } else {
            std::cout << style::BOLD << style::GREEN << finish_message_ << style::NO_COL << std::endl;
        }
    }
}


// --- Utility Function Implementation ---

void print_err(const std::string& message) {
    std::cerr << style::RED << style::BOLD << "Error: " << message << style::NO_COL << std::endl;
}