#include <opencv2/opencv.hpp>

#include "hikrobot/include/MvCameraControl.h"

cv::Mat transfer(MV_FRAME_OUT & raw);

class Camera
{
public:
  Camera();
  ~Camera();
  cv::Mat Read();
  bool Open();
  void Close();

private:
  int ret_{0};                //保存 SDK 函数返回码，初始化等于 0
  void * handle_{nullptr};    //相机句柄。void*通用指针，可以存任意资源；nullptr就是空
  bool m_isGrabbing_{false};  //标记相机是否已经开启取流
};
