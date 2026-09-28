#include <iostream>
class Car
{
public:
  Car();
  ~Car();

  void run();

private:
  int count_ = 0;
};
Car::Car() { std::cout << "Car 构造完成" << std::endl; }

Car::~Car()
{
  std::cout << "Car has been destroyed. It has been used for " << count_ << " times." << std::endl;
}

int main()
{
  {
    Car car;
    for (int i = 0; i < 5; i++) car.run();
  }

  return 0;
}
void Car::run()
{
  count_++;
  std::cout << "car is used" << std::endl;
}