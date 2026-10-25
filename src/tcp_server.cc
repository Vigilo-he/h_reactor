#include "h_reactor/tcp_server.h"
#include "h_reactor/acceptor.h"
#include "h_reactor/event_loop.h"
#include "h_reactor/event_loop_thread_pool.h"
#include "h_reactor/tcp_connection.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

namespace h_reactor {

TcpServer::TcpServer(EventLoop* loop, int port, int threads_num)
    : loop_(loop)
    , acceptor_(new Acceptor(loop, port))
    , thread_pool_(std::make_unique<EventLoopThreadPool>(loop))
{
    acceptor_->SetNewConnectionCallback([this](int fd_) { NewConnection(fd_); });
    thread_pool_->SetThreadsNum(threads_num);
}

void TcpServer::SetConnectionCallback(ConnectionCallback cb) {
    connection_callback_ = std::move(cb);
}

void TcpServer::SetMessageCallback(MessageCallback cb) {
    message_callback_ = std::move(cb);
}

void TcpServer::Start() {
    std::cout << "Tcp Server Start" << std::endl;
    thread_pool_->Start();
}

void TcpServer::NewConnection(int fd) {
    EventLoop* io_loop = thread_pool_->GetNextLoop();

    auto conn = std::make_shared<TcpConnection>(io_loop, fd);

    conn->SetMessageCallback(message_callback_);

    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        connections_[fd] = conn;

        conn->SetCloseCallback([this](const std::shared_ptr<TcpConnection>& c) {
            this->RemoveConnection(c);
        });
    }

    if (connection_callback_) {
        connection_callback_(conn);
    }
}

void TcpServer::RemoveConnection(const std::shared_ptr<TcpConnection>& conn) {
    {   
        std::unique_lock<std::mutex> lock(mutex_);
        connections_.erase(conn->Fd());
    }
}

}