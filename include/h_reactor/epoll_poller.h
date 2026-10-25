#pragma once

#include "h_reactor/poller.h"

#include <map>
#include <sys/epoll.h>

namespace h_reactor {

class Channel;
class EventLoop;

class EpollPoller : public Poller {
public:
    EpollPoller();
    ~EpollPoller() override;

    void Poll(int, ChannelList*) override;

    void UpdateChannel(Channel*) override;

    void RemoveChannel(Channel*) override;

    void FillActiveChannels(int, ChannelList*);

private:
    static const int kInitEventListSize = 16;
    int epoll_fd_;
    std::vector<epoll_event> events_;
    std::map<int, Channel*> channels_;
};

}