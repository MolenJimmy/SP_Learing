#ifndef AUTO_AIM__YOLO_HPP
#define AUTO_AIM__YOLO_HPP

#include <opencv2/opencv.hpp>

#include "armor.hpp"

namespace auto_aim  //外面使用的时候要写：auto_aim::YOLO，避免和别的地方同名类冲突
{
class YOLOBase
{
public:
  //纯虚函数→YOLOBase是抽象类，不能直接创建对象
  virtual std::list<Armor> detect(const cv::Mat & img, int frame_count) = 0;
  //设计目的：不管以后是 YOLOv5、YOLOv7、YOLOv8，只要继承`YOLOBase`，
  //实现 detect 接口，就可以无缝接入这套框架。对外不需要改上层调用代码
};

class YOLO
{
public:
  YOLO(const std::string & config_path, bool debug = true);

  std::list<Armor> detect(const cv::Mat & img, int frame_count = -1);

private:
  std::unique_ptr<YOLOBase> yolo_;  //指针类型是基类YOLOBase，但它可以存放子类对象YOLOV5
};

}  // namespace auto_aim

#endif  // AUTO_AIM__YOLO_HPP