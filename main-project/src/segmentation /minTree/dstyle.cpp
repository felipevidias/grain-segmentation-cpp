/**
 * @file dstyle.cpp
 * @brief Implements the TerminalAnimator class for CLI animations.
 */

 #include "dstyle.h"
 #include <iostream>
 #include <vector>
 #include <chrono>
 
 TerminalAnimator::TerminalAnimator() : is_running(false) {
     // A thread só será iniciada quando 'show' for chamado pela primeira vez
 }
 
 TerminalAnimator::~TerminalAnimator() {
     if (animator_thread.joinable()) {
         {
             std::lock_guard<std::mutex> lock(mtx);
             is_running = false;
         }
         cv.notify_one();
         animator_thread.join();
     }
 }
 
 void TerminalAnimator::show(const std::string& loading_message, const std::string& finish_message, const std::string& failed_message) {
     {
         std::lock_guard<std::mutex> lock(mtx);
         current_message = loading_message;
         success_message = finish_message;
         failure_message = failed_message;
         task_failed = false;
         is_running = true;
 
         if (!animator_thread.joinable()) {
              animator_thread = std::thread(&TerminalAnimator::animation_loop, this);
         }
     }
     cv.notify_one();
 }
 
 void TerminalAnimator::succeed() {
     {
         std::lock_guard<std::mutex> lock(mtx);
         is_running = false;
         task_failed = false;
     }
     cv.notify_one();
     std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Garante que a thread de animação imprima a mensagem final
 }
 
 void TerminalAnimator::fail() {
     {
         std::lock_guard<std::mutex> lock(mtx);
         is_running = false;
         task_failed = true;
     }
     cv.notify_one();
     std::this_thread::sleep_for(std::chrono::milliseconds(150));
 }
 
 void TerminalAnimator::animation_loop() {
     const std::vector<std::string> symbols = {"[   ]", "[.  ]", "[.. ]", "[...]", "[ ..]", "[  .]"};
     int i = 0;
 
     while (true) {
         std::unique_lock<std::mutex> lock(mtx);
         cv.wait(lock, [this]{ return is_running.load(); });
         
         while (is_running) {
             i = (i + 1) % symbols.size();
             std::cout << "\r" << style::BLUE << current_message << " " << symbols[i] << style::NORMAL << std::flush;
             
             // Espera por 100ms ou até ser notificado
             if (cv.wait_for(lock, std::chrono::milliseconds(100), [this]{ return !is_running.load(); })) {
                 break;
             }
         }
 
         // Fim do loop de animação, imprime a mensagem final
         if (!task_failed) {
             std::cout << "\r" << style::BOLD << style::GREEN << success_message << style::NORMAL << std::endl;
         } else {
             std::cout << "\r" << style::BOLD << style::RED << failure_message << style::NORMAL << std::endl;
         }
     }
 }