#pragma once

#include <unordered_map>
#include <vector>

namespace h_reactor {
    
class Channel;

class Poller {
public:
    using ChannelList = std::vector<Channel*>;

    Poller() = default;
    virtual ~Poller() = default;
    Poller(Poller&) = delete;
    Poller& operator=(const Poller&) = delete;

    virtual void Poll(int, ChannelList*) = 0;

    virtual void UpdateChannel(Channel*) = 0;

    virtual void RemoveChannel(Channel*) = 0;

protected:
    std::unordered_map<int, Channel*> channels_;  
};

}