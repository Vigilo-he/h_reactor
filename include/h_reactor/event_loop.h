#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "h_reactor/poller.h"
#include "h_reactor/channel.h"

namespace h_reactor {

class EventLoop {
public:    
    EventLoop();
    ~EventLoop() = default;
    EventLoop(EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    void Loop();

    void Quit();

    void UpdateChannel(Channel*);
    void RemoveChannel(Channel*);

    bool IsInLoopThread();

    void RunInLoop(std::function<void()>);
    void QueueInLoop(std::function<void()>);

    void WakeUp();

    void DoPendingFunctors();

private:
    bool quit_;
    std::unique_ptr<Poller> poller_;
    std::vector<Channel*> active_channels_; 

    std::thread::id thread_id_;
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    std::vector<std::function<void()>> pending_functors_;

    std::mutex mutex_;

    bool call_pending_functors;
};

}