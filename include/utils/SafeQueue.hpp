#ifndef EDGE_SENTINEL_UTILS_SAFE_QUEUE_H_
#define EDGE_SENTINEL_UTILS_SAFE_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

template <typename T>
class SafeQueue {
   public:
    SafeQueue() : running_(true) {
    }

    // Adds an item to the queue and notifies the worker.
    void Push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cond_var_.notify_one();
    }

    // Waits for an item to be available. Returns false if shutting down.
    bool Pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);

        // Wait until queue is not empty or the system is shutting down.
        cond_var_.wait(lock, [this] { return !queue_.empty() || !running_; });

        if (queue_.empty()) {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    // Signals the queue to stop waiting and prevents further pushes.
    void Shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cond_var_.notify_all();
    }

   private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
    bool running_;
};

#endif  // EDGE_SENTINEL_UTILS_SAFE_QUEUE_H_