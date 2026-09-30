#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int counter = 0;

constexpr int N = 50000;  // 常量表达式，强制要求在编译期确定值

// 互斥锁变量
std::mutex mutex;

void work()
{
  for (int i = 0; i < N; ++i) {
    counter++;
  }
}

int main()
{
  std::vector<std::thread> threads;

// 创建8个线程，全部执行work()函数
  for (int i = 0; i < 8; ++i) {
    threads.emplace_back(work); // 创建线程对象，启动线程
  }

  // 主线程阻塞等待这个子线程跑完，所有线程全部结束后，主线程才继续往下执行打印
  for (auto & t : threads) {
    t.join();
  }

  std::cout << "expected: " << 8 * N << '\n';
  std::cout << "actual:   " << counter << '\n';
}