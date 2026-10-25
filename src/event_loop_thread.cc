#include "h_reactor/event_loop_thread.h"
#include "h_reactor/event_loop.h"

#include <mutex>

namespace h_reactor {
EventLoopThread::EventLoopThread()
    : exiting_(true)
    , loop_(nullptr)
{}

EventLoopThread::~EventLoopThread() {
    exiting_ = false;
    if (loop_) {
        loop_->Quit();
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

EventLoop* EventLoopThread::StartLoop() {
    thread_ = std::thread(&EventLoopThread::ThreadFunc, this);
    EventLoop* loop;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return this->loop_ != nullptr; });
        loop = loop_;
    }

    return loop;
}

void EventLoopThread::ThreadFunc() {
    EventLoop loop;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cv_.notify_one();
    }

    loop.Loop();

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}

}