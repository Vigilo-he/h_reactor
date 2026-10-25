#pragma once

#include "h_reactor/buffer.h"
#include <functional>
#include <memory>

namespace h_reactor {

class Channel;
class EventLoop;
class EpollPoller;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    using MessageCallback = std::function<void(const std::shared_ptr<TcpConnection>&, Buffer*)>;
    using CloseCallback = std::function<void(const std::shared_ptr<TcpConnection>&)>;

    TcpConnection(EventLoop*, int fd);
    ~TcpConnection();

    void Send(const std::string&);
    void Shutdown();

    void SetMessageCallback(MessageCallback cb);
    void SetCloseCallback(CloseCallback cb);

    int Fd() const;

private:
    void HandleRead();
    void HandleWrite();
    void HandleClose();

    EventLoop* loop_;
    int fd_;
    Channel* channel_;
    Buffer* input_buffer_;
    Buffer* output_buffer_;

    MessageCallback message_callback_;
    CloseCallback close_callback_;
};

}