cmake_minimum_required(VERSION 3.10)
project(h_reactor)

# 必须改成 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

# 假设你已经把协程头文件放在了 h_reactor 的同一级，或者通过相对路径引入
# 这里为了让编译通过，我们假设 h_coro 和 h_reactor 是同级的兄弟目录
include_directories(${PROJECT_SOURCE_DIR}/../h_coro/include)

set(SRC_FILES
    src/channel.cc
    src/epoll_poller.cc
    src/event_loop.cc
    src/acceptor.cc
    src/buffer.cc
    src/tcp_connection.cc
    src/tcp_server.cc
)

add_library(h_reactor STATIC ${SRC_FILES})
target_link_libraries(h_reactor PRIVATE pthread)

# ==========================================
# 测试程序（把协程和网络库结合起来）
# ==========================================
add_executable(test_coro_net ${PROJECT_SOURCE_DIR}/test/test_coro_net.cpp)

# 链接你的网络库
target_link_libraries(test_coro_net PRIVATE h_reactor pthread)