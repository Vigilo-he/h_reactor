#include "h_reactor/event_loop.h"
#include "h_reactor/epoll_poller.h"
#include "h_reactor/channel.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/eventfd.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

namespace h_reactor {
    
EventLoop::EventLoop()
    : quit_(false)
    , poller_(std::make_unique<EpollPoller>())
    , thread_id_(std::this_thread::get_id())
    , call_pending_functors(false)
{
    wakeup_fd_ = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    wakeup_channel_.reset(new Channel(this, wakeup_fd_));

    wakeup_channel_->SetReadCallback([this]() {
        uint64_t one;
        ::read(wakeup_fd_, &one, sizeof(one));
    });

    wakeup_channel_->EnableRead();
}

void EventLoop::Loop() {
    quit_ = false;

    while (!quit_) {
        active_channels_.clear();

        poller_->Poll(10000, &active_channels_);

        for (auto channel : active_channels_) {
            channel->HandleEvent();
        }
        
        DoPendingFunctors();
    }
}

void EventLoop::DoPendingFunctors() {
    std::vector<std::function<void()>> functors;
    call_pending_functors = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pending_functors_);
    }

    for (const auto& cb : functors) {
        cb();
    }

    call_pending_functors = false;
}

void EventLoop::Quit() {
    quit_ = true;
}

void EventLoop::UpdateChannel(Channel* channel) {
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
    poller_->RemoveChannel(channel);
}

bool EventLoop::IsInLoopThread() {
    return (std::this_thread::get_id() == thread_id_);
}

void EventLoop::RunInLoop(std::function<void()> cb) {
    if (IsInLoopThread()) {
        cb();
    } else {
        QueueInLoop(std::move(cb));
    }
}

void EventLoop::QueueInLoop(std::function<void()> cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pending_functors_.push_back(std::move(cb));
    }

    if (!IsInLoopThread() || call_pending_functors) {
        WakeUp();
    }
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ::write(wakeup_fd_, &one, sizeof(one));
}

}