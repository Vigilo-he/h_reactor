#include "h_reactor/channel.h"
#include "h_reactor/acceptor.h"
#include "h_reactor/epoll_poller.h"
#include "h_reactor/event_loop.h"

#include <fcntl.h>
#include <asm-generic/socket.h>
#include <cstring>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

namespace h_reactor {
    
Acceptor::Acceptor(EventLoop* loop, int port)
    : loop_(loop)
    , fd_(::socket(AF_INET, SOCK_STREAM, 0))
    , channel_(new Channel(loop_, fd_))
{
    int opt = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int flags = ::fcntl(fd_, F_GETFL, 0);
    ::fcntl(fd_, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind error" << std::endl;
        ::close(fd_);
        return;
    }

    if (::listen(fd_, SOMAXCONN) < 0) {
        std::cerr << "listen error" << std::endl;
        ::close(fd_);
        return;
    }

    channel_->SetReadCallback([this]() { this->HandleRead(); });
    channel_->EnableRead();
}

Acceptor::~Acceptor() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

void Acceptor::SetNewConnectionCallback(std::function<void(int)> cb) {
    new_connection_callback_ = std::move(cb);
}

void Acceptor::HandleRead() {
    while (true) {
        struct sockaddr_in peer_addr;
        socklen_t peer_len = sizeof(peer_addr);

        int connfd = ::accept(fd_, (struct sockaddr*)&peer_addr, &peer_len);

        if (connfd >= 0) {
            if (new_connection_callback_) {
                new_connection_callback_(connfd);
            } else {
                ::close(connfd);
            }
        } else {
            break;
        }
    }
}

}