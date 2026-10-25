#include <iostream>
#include "h_reactor/event_loop.h"
#include "h_reactor/event_loop_scheduler.h"
#include "h_coro/task.h"

using namespace h_coro;
using namespace h_reactor;

Task<> MyCoroTask() {
    std::cout << "[Coroutine] 协程开始执行！" << std::endl;
    co_await std::suspend_always{}; // 模拟挂起
    std::cout << "[Coroutine] 协程被唤醒并结束！" << std::endl;
}

int main() {
    EventLoop loop;
    EventLoopScheduler scheduler(&loop);

    std::cout << "--- main 准备启动协程 ---" << std::endl;
    
    // 把协程丢给 scheduler 调度
    MyCoroTask().ScheduleOn(&scheduler);

    std::cout << "--- main 继续往下走 ---" << std::endl;

    // 模拟 loop 跑几次（因为协程里有 suspend_always，它其实会挂起不回来，
    // 这里只为了演示不崩溃）
    for (int i = 0; i < 3; ++i) {
        loop.Loop();
    }

    std::cout << "--- main 结束 ---" << std::endl;
    return 0;
}