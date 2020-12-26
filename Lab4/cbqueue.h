#include "mbrot.h"
#include <mutex>
#include <condition_variable>
#include <deque>

template <typename T>
class ConcurrentBoundedQueue {

    std::deque<T> queue_{};
    std::mutex m{};
    std::condition_variable producer_cv{};
    std::condition_variable consumer_cv{};
    int max_items;
public:
    ConcurrentBoundedQueue(ConcurrentBoundedQueue& other) = delete;
    ConcurrentBoundedQueue& operator=(ConcurrentBoundedQueue&) = delete;
    ConcurrentBoundedQueue(int max_items) : max_items(max_items) {}
    void put(T item);
    T get();
};

template <typename T> 
void ConcurrentBoundedQueue<T>::put(T item) {
    std::unique_lock<std::mutex> lock(m);
    while (queue_.size() >= max_items) {
        producer_cv.wait(lock);
    }
    queue_.push_back(item);
    // Notify all waiting consumers that an item was added ? 
    consumer_cv.notify_one();
}

template <typename T> 
T ConcurrentBoundedQueue<T>::get() {
    std::unique_lock<std::mutex> lock(m);
    while (queue_.size() == 0) {
        consumer_cv.wait(lock);
    }
    auto item = queue_.front();
    queue_.pop_front();
    // Notify all producers waiting on a previously full queue
    producer_cv.notify_one();
    return item;
}
