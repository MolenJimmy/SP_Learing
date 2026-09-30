#include <iostream>

struct Tracker
{
  int count;
};

int main()
{
  int frame_id = 10;
  Tracker tracker{1};

  // Create the lambda without invoking it yet.
  // 捕获列表[frame_id, &tracker]
  // frame_id：值捕获,没有&,创建lambda那一瞬间，直接复制一份frame_id=10保存到lambda内部
  //           后面外面修改frame_id，不会影响lambda里面这份拷贝
  // &tracker：引用捕获,不复制对象，lambda直接持有外面tracker的地址，lambda里面操作的就是外面原来的对象
  //            外面修改tracker，lambda里面看到的就是修改后的最新值
  auto task = [frame_id, &tracker](int delta) {
    std::cout << "frame_id = " << frame_id << '\n';
    std::cout << "delta = " << delta << '\n';

    tracker.count += delta;
    std::cout << "tracker.count = " << tracker.count << '\n';
  };

  frame_id = 99;
  tracker.count = 7;

  task(3);
}
