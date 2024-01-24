#pragma once

#include <condition_variable>
#include <mutex>
#include <cassert>
#include <queue>

// A thread safe synchronized queue with blocking pop_front
// and non-blocking push_back
template<typename T>
class Channel {
public:
    void push_back(T item) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(item);
        cv.notify_one();
    }

    T pop_front() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !q.empty(); });
        assert(!q.empty());
        auto item = q.front();
        q.pop();
        return item;
    }

private:
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv;
};