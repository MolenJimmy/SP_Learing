#include "yolo.hpp"

#include <yaml-cpp/yaml.h>

#include "yolos/yolov5.hpp"

namespace auto_aim
{
YOLO::YOLO(const std::string & config_path, bool debug)
{
  //加载yaml配置文件
  auto yaml = YAML::LoadFile(config_path);
  auto yolo_name = yaml["yolo_name"].as<std::string>();

  // 创建YOLOV5子类对象，交给unique_ptr管理
  if (yolo_name == "yolov5") {
    // 安全创建对象，返回`unique_ptr<T>`
    yolo_ = std::make_unique<YOLOV5>(config_path, debug);
  }

  else {
    throw std::runtime_error("Unknown yolo name: " + yolo_name + "!");
  }
}

std::list<Armor> YOLO::detect(const cv::Mat & img, int frame_count)
{
  return yolo_->detect(img, frame_count);
}

}  // namespace auto_aim