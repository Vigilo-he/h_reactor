#include "h_reactor/tcp_server.h"
#include "h_reactor/event_loop.h"
#include "h_reactor/buffer.h"
#include <iostream>
#include <memory>

using namespace h_reactor;

int main() {
    EventLoop loop;
    TcpServer server(&loop, 8000, 5);

    // 收到消息的回调：原样发回
    server.SetMessageCallback([](const std::shared_ptr<TcpConnection>& conn, Buffer* buf) {
        std::string msg = buf->RetrieveAllAsString();
        std::cout << "收到客户端的消息: " << msg << std::endl;
        conn->Send("Server Echo: " + msg);
    });

    // 连接断开的回调
    server.SetConnectionCallback([](const std::shared_ptr<TcpConnection>& conn) {
        std::cout << "新连接建立或断开: fd = " << conn->Fd() << std::endl;
    });

    server.Start();
    loop.Loop(); // 死循环，开始监听

    return 0;
}