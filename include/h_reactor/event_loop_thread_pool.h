#pragma once

#include "h_reactor/event_loop.h"
#include "h_reactor/event_loop_thread.h"
#include <memory>
#include <vector>

namespace h_reactor {

class EventLoopThreadPool {
public:
    EventLoopThreadPool(EventLoop*);
    ~EventLoopThreadPool();

    EventLoopThreadPool(const EventLoopThreadPool&) = delete;
    EventLoopThreadPool& operator=(const EventLoopThreadPool&) = delete;
    
    void SetThreadsNum(int);

    void Start();

    EventLoop* GetNextLoop();

private:
    EventLoop* base_loop_;
    int num_threads_;
    int next_;
    bool started_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

}