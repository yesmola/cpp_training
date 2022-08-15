#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace cpputil {

class ThreadPool {
public:
    explicit ThreadPool(const size_t thread_num = 0) : thread_num_(thread_num) {
        is_running_ = true;
        for (int i = 0; i < thread_num_; ++i) {
            threads_.push_back(std::make_unique<std::thread>(std::thread(&ThreadPool::run, this)));
        }
    }

    ~ThreadPool() {
        wait_for_all_tasks();
        destroy_threads();
    }

private:
    void wait_for_all_tasks() {
        is_waiting_ = true;
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        tasks_done_cv_.wait(lock, [this] { return tasks_num_ == 0; });
        is_waiting_ = false;
    }

    void destroy_threads() {
        is_running_ = false;
        tasks_available_cv_.notify_all();
        for (size_t i = 0; i < thread_num_; ++i) {
            threads_[i]->join();
        }
    }

    void run() {
        while (is_running_) {
            std::function<void()> task;
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            tasks_available_cv_.wait(lock, [this] { return !tasks_.empty() || !is_running_; });
            if (is_running_) {
                task = std::move(tasks_.front());
                tasks_.pop();
                lock.unlock();
                task();
                lock.lock();
                --tasks_num_;
                if (is_waiting_) {
                    tasks_done_cv_.notify_one();
                }
            }
        }
    }

private:
    std::atomic<size_t> tasks_num_;
    std::mutex tasks_mutex_;
    std::condition_variable tasks_available_cv_;
    std::condition_variable tasks_done_cv_;
    std::queue<std::function<void()>> tasks_;

    std::atomic_bool is_running_ = false;
    std::atomic_bool is_waiting_ = false;
    size_t thread_num_ = 0;
    std::vector<std::unique_ptr<std::thread>> threads_;
};

} // namespace cpputil
