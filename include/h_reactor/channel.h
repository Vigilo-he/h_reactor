#pragma once

#include <functional>

namespace h_reactor {

class EventLoop;

class Channel {
public:
    Channel(EventLoop*, int);
    ~Channel();
    Channel(Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    int Fd() const;
    int Event() const;
    int Revent() const;

    void SetRevent(int);

    void SetReadCallback(std::function<void()>);
    void SetWriteCallback(std::function<void()>);
    void SetErrorCallback(std::function<void()>);

    void EnableRead();
    void DisableRead();
    void EnableWrite();
    void DisableWrite();

    void HandleEvent();

private:
    void Update();

    EventLoop* loop_;
    const int fd_;
    int event_;
    int revent_;

    std::function<void()> read_callback_;
    std::function<void()> write_callback_;
    std::function<void()> error_callback_;
};

}