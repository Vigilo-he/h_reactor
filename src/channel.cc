#include "h_reactor/channel.h"
#include "h_reactor/event_loop.h"

#include <sys/epoll.h>

#include <functional>

namespace h_reactor {
    
Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop)
    , fd_(fd)
    , event_(0)
    , revent_(0)
{}

Channel::~Channel() = default;

int Channel::Fd() const {
    return fd_;
}

int Channel::Event() const {
    return event_;
}

int Channel::Revent() const {
    return revent_;
}

void Channel::SetRevent(int revent) {
    revent_ = revent;
}

void Channel::SetReadCallback(std::function<void()> cb) {
    read_callback_ = cb;
}

void Channel::SetWriteCallback(std::function<void()> cb) {
    write_callback_ = cb;
}

void Channel::SetErrorCallback(std::function<void()> cb) {
    error_callback_ = cb;
}

void Channel::EnableRead() {
    event_ |= EPOLLIN;
    Update();
}

void Channel::DisableRead() {
    event_ &= ~EPOLLIN;
    Update();
}

void Channel::EnableWrite() {
    event_ |= EPOLLOUT;
    Update();
}

void Channel::DisableWrite() {
    event_ &= ~EPOLLOUT;
    Update();
}

void Channel::HandleEvent() {
    if ((revent_ & EPOLLIN) && read_callback_) {
        read_callback_();
    }
    if ((revent_ & EPOLLOUT) && write_callback_) {
        write_callback_(); 
    }
    if ((revent_ & EPOLLERR) && error_callback_) {
        error_callback_();
    }
}

void Channel::Update() {
    loop_->UpdateChannel(this);
}
}