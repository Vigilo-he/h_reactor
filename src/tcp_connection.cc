
#include "h_reactor/tcp_connection.h"
#include "h_reactor/channel.h"
#include "h_reactor/event_loop.h"
#include "h_reactor/buffer.h"

#include <iostream>
#include <cerrno>
#include <cstddef>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace h_reactor {
TcpConnection::TcpConnection(EventLoop* loop, int fd)
    : loop_(loop)
    , fd_(fd)
    , channel_(new Channel(loop, fd))
    , input_buffer_(new Buffer())
    , output_buffer_(new Buffer())
{
    channel_->SetWriteCallback([this]() { HandleWrite(); });
    channel_->SetReadCallback([this]() { HandleRead(); });
    channel_->SetErrorCallback([this]() {HandleClose(); });

    std::cout << "TcpConnection 构造，fd: " << fd_ << std::endl;
    
    channel_->EnableRead();
}

TcpConnection::~TcpConnection() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

void TcpConnection::SetMessageCallback(MessageCallback cb) {
    message_callback_ = std::move(cb);
}

void TcpConnection::SetCloseCallback(CloseCallback cb) {
    close_callback_ = std::move(cb);
}

int TcpConnection::Fd() const {
    return fd_;
}

void TcpConnection::HandleRead() {
    auto n = input_buffer_->ReadFd(fd_);
    std::cout << "HandleRead 触发了，读到字节数: " << n << std::endl;

    if (n > 0) {
        message_callback_(shared_from_this(), input_buffer_);
    } else if (n == 0) {
        HandleClose();
    }
}

void TcpConnection::Send(const std::string& message) {
    if (fd_ < 0) return;

    std::cout << "准备发送数据: " << message << std::endl; // 👈 加这句
    ssize_t n = ::write(fd_, message.data(), message.size());
    std::cout << "发送了字节数: " << n << std::endl; // 👈 加这句
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            output_buffer_->Append(message.data(), message.size());
            channel_->EnableWrite();
        } else {
            HandleClose();
        }
    } else if (static_cast<int>(n) < message.size()) {
        output_buffer_->Append(message.data() + n, message.size() - n);
        channel_->EnableWrite();
    }
}

void TcpConnection::HandleWrite() {
    if (output_buffer_->ReadableBytes()) {
        size_t n = ::write(fd_, output_buffer_->Peek(), output_buffer_->ReadableBytes());
        if (n > 0) {
            output_buffer_->Retrieve(n);
            if (output_buffer_->ReadableBytes() == 0) {
                channel_->DisableWrite();
            }
        }
    }
}

void TcpConnection::Shutdown() {
    channel_->DisableWrite();
    if (::shutdown(fd_, SHUT_WR) < 0) {
        HandleClose();
    }
}

void TcpConnection::HandleClose() {
    if (fd_ < 0) return;

    channel_->DisableRead();
    ::close(fd_);
    fd_ = -1;

    if (close_callback_) {
        close_callback_(shared_from_this());
    }

    std::cout << "HandleClose 被调用了，连接关闭！" << std::endl; // 👈 加这句
}
}