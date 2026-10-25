#include "h_reactor/event_loop_thread_pool.h"
#include "h_reactor/event_loop.h"
#include "h_reactor/event_loop_thread.h"
#include <memory>

namespace h_reactor {
EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop)
    : base_loop_(base_loop)
    , num_threads_(0)
    , next_((0))
    , started_(false)
{}

EventLoopThreadPool::~EventLoopThreadPool() = default;

void EventLoopThreadPool::SetThreadsNum(int num) {
    num_threads_ = num;
}

void EventLoopThreadPool::Start() {
    started_ = true;

    for (int i = 0; i < num_threads_; i++) {
        auto t = std::make_unique<EventLoopThread>();

        loops_.push_back(t->StartLoop());
        threads_.push_back(std::move(t));
    }
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
    if (loops_.empty()) {
        return base_loop_;
    }

    EventLoop* loop = loops_[next_];
    next_ = (next_ + 1) % loops_.size();
    return loop;
}

}