#include <iostream>
#include <vector>

class Armor
{
public:
  std::vector<int> brightness;

  Armor() : brightness{20, 60, 100, 60, 20}
  {
    std::cout << "Armor constructed\n";
  }

  ~Armor()
  {
    std::cout << "Armor destructed\n";
  }
};

int main()
{
  std::cout << "before {}\n";

  // 开启一个独立局部作用域
  // 在里面定义的栈变量，一旦执行到}，变量就直接销毁
  {
    Armor armor;

    // Help Python Tutor display the heap data managed by the vector.
    // vector.data()返回指向vector底层数组首地址的指针
    int * data = armor.brightness.data();
    std::cout << data[2] << '\n';
  }

  std::cout << "after {}\n";
}
