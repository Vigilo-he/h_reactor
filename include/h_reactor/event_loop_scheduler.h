#pragma once
#include "h_coro/scheduler.h"
#include "h_reactor/event_loop.h"

namespace h_reactor {

class EventLoopScheduler : public h_coro::Scheduler {
public:
    explicit EventLoopScheduler(EventLoop* loop) : loop_(loop) {}

    void Schedule(std::function<void()> task) override {
        // 核心：把任务丢给 EventLoop（它底层用 eventfd 跨线程唤醒）
        loop_->RunInLoop(std::move(task));
    }

private:
    EventLoop* loop_;
};

} // namespace h_reactor