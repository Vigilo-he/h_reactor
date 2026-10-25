#include "h_reactor/epoll_poller.h"
#include "h_reactor/event_loop.h"
#include "h_reactor/channel.h"

#include <sys/epoll.h>
#include <unistd.h>

namespace h_reactor {
    
EpollPoller::EpollPoller()
    : epoll_fd_(epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{}

EpollPoller::~EpollPoller() {
    if (epoll_fd_ >= 0) {
        ::close(epoll_fd_);
    }
}

void EpollPoller::Poll(int timeout_ms, ChannelList* active_channels) {
    int num_events = epoll_wait(epoll_fd_, events_.data(), static_cast<int>(events_.size()), timeout_ms);
    if (num_events > 0) {
        FillActiveChannels(num_events, active_channels);

        if (num_events == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
}

void EpollPoller::FillActiveChannels(int num_events, ChannelList* active_channels) {
    for (int i = 0; i < num_events; i++) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);

        channel->SetRevent(events_[i].events);

        active_channels->push_back(channel);
    }
}

void EpollPoller::UpdateChannel(Channel* channel) {
    int fd = channel->Fd();
    int events = channel->Event();

    epoll_event ev;
    ev.events = events;
    ev.data.ptr = channel;

    if (channels_.find(fd) == channels_.end()) {
        channels_[fd] = channel;
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
    } else {
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
    }
}

void EpollPoller::RemoveChannel(Channel* channel) {
    int fd = channel->Fd();

    channels_.erase(channels_.find(fd));

    ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
}
}