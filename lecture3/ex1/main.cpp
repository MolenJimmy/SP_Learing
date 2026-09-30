#include <iostream>
#include <vector>

int main()
{
  int x = 42;
  std::vector<int> v(10);
  v.push_back(1);

  int * p = v.data();  //p：堆地址（存数字的数组）

  std::cout << "&v = " << &v << "\n";
  std::cout << "v.data = " << p << "\n";
  std::cout << "*v.data = " << *p << "\n";   // *p：解引用指针，拿到数组第一个元素的值
  std::cout << "v.size() = " << v.size() << "\n";
  std::cout << "v.capacity() = " << v.capacity() << "\n";  //堆数组最多能存多少元素

  return 0;
}