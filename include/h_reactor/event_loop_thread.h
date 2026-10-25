#pragma once

#include "h_reactor/event_loop.h"

#include <condition_variable>
#include <mutex>
#include <thread>

namespace h_reactor {

class EventLoopThread {
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoopThread(const EventLoopThread&) = delete;
    EventLoopThread& operator=(const EventLoopThread&) = delete;

    EventLoop* StartLoop();

private:
    void ThreadFunc();

    EventLoop* loop_;
    bool exiting_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}