#pragma once

#include <functional>

namespace h_reactor {
    
class Channel;
class EventLoop;

class Acceptor {
public:
    Acceptor(EventLoop* loop_, int port);
    ~Acceptor();

    void SetNewConnectionCallback(std::function<void(int)>);

    void HandleRead();
    
private:
    EventLoop* loop_;
    const int fd_;
    Channel* channel_;

    std::function<void(int)> new_connection_callback_;
};

}