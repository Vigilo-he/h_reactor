#pragma once

#include "h_reactor/acceptor.h"
#include "h_reactor/buffer.h"
#include "h_reactor/event_loop.h"
#include "h_reactor/event_loop_thread_pool.h"
#include "h_reactor/tcp_connection.h"
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace h_reactor {

class TcpServer {
public:
    using ConnectionCallback = std::function<void(const std::shared_ptr<TcpConnection>&)>;
    using MessageCallback = std::function<void(const std::shared_ptr<TcpConnection>&, Buffer*)>;

    TcpServer(EventLoop*, int, int);
    ~TcpServer() = default;

    void Start();

    void SetConnectionCallback(ConnectionCallback);
    void SetMessageCallback(MessageCallback);

public:
    void NewConnection(int);
    void RemoveConnection(const std::shared_ptr<TcpConnection>&);

    EventLoop* loop_;
    Acceptor* acceptor_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;

    std::unordered_map<int, std::shared_ptr<TcpConnection>> connections_;

    ConnectionCallback connection_callback_;
    MessageCallback message_callback_;

    std::mutex mutex_;
};

}