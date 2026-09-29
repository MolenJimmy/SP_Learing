#include "camera.hpp"

#include <opencv2/opencv.hpp>

#include "hikrobot/include/MvCameraControl.h"

cv::Mat transfer(MV_FRAME_OUT & raw, void * handle)
{
  cv::Mat img(
    cv::Size(raw.stFrameInfo.nWidth, raw.stFrameInfo.nHeight), CV_8UC3);

  MV_CC_PIXEL_CONVERT_PARAM cvt_param{};
  cvt_param.nWidth = raw.stFrameInfo.nWidth;
  cvt_param.nHeight = raw.stFrameInfo.nHeight;

  cvt_param.pSrcData = raw.pBufAddr;
  cvt_param.nSrcDataLen = raw.stFrameInfo.nFrameLen;
  cvt_param.enSrcPixelType = raw.stFrameInfo.enPixelType;

  cvt_param.pDstBuffer = img.data;
  cvt_param.nDstBufferSize = static_cast<unsigned int>(img.total() * img.elemSize());
  cvt_param.enDstPixelType = PixelType_Gvsp_BGR8_Packed;

  if (MV_CC_ConvertPixelType(handle, &cvt_param) != MV_OK) return cv::Mat();
  return img;
}

Camera::Camera()
{
}

Camera::~Camera()
{
}

cv::Mat Camera::Read()
{
  // 读取一帧图像

  // 先做合法性校验：如果相机没有打开/没有启动取流，返回空 Mat
  if (handle_ == nullptr || !m_isGrabbing_) {
    return cv::Mat();
  }

  MV_FRAME_OUT raw{};  //大括号初始化，结构体全部清零。
  unsigned int nMsec = 100;

  // 阻塞等待一帧，超时 100ms。失败返回空图像
  ret_ = MV_CC_GetImageBuffer(handle_, &raw, nMsec);
  if (ret_ != MV_OK) {
    return cv::Mat();
  }

  cv::Mat img = transfer(raw, handle_);

  // 重中之重！GetImageBuffer拿到帧，必须调用FreeImageBuffer归还缓冲区给SDK
  ret_ = MV_CC_FreeImageBuffer(handle_, &raw);
  return img;
}

bool Camera::Open()
{
  // 如果已经打开相机,handle_不是空指针
  if (handle_ != nullptr) {
    std::cout<<"Have Camera"<<std::endl;
    return true;
  }

  // 调用 SDK 枚举 USB 相机
  MV_CC_DEVICE_INFO_LIST device_list{};
  ret_ = MV_CC_EnumDevices(MV_USB_DEVICE, &device_list);
  if (ret_ != MV_OK || device_list.nDeviceNum == 0) {
    std::cout<<"No Device"<<std::endl;
    return false;
  }

  ret_ = MV_CC_CreateHandle(&handle_, device_list.pDeviceInfo[0]);
  if (ret_ != MV_OK) return false;

  ret_ = MV_CC_OpenDevice(handle_);
  if (ret_ != MV_OK) return false;

  // 设置相机硬件参数，全部是海康 SDKC 接口，第一个参数永远传相机句柄`handle_`
  MV_CC_SetEnumValue(handle_, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_CONTINUOUS);
  MV_CC_SetEnumValue(handle_, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
  MV_CC_SetEnumValue(handle_, "GainAuto", MV_GAIN_MODE_OFF);
  MV_CC_SetFloatValue(handle_, "ExposureTime", 10000);
  MV_CC_SetFloatValue(handle_, "Gain", 20);
  MV_CC_SetFrameRate(handle_, 60);

  //启动取流
  ret_ = MV_CC_StartGrabbing(handle_);  // 只调用一次！开启相机取流，不要每次读帧调用
  if (ret_ == MV_OK) {
    m_isGrabbing_ = true;
  }
  return ret_ == MV_OK;
}

void Camera::Close()
{
  if (handle_ == nullptr) return;

  if (m_isGrabbing_) {
    MV_CC_StopGrabbing(handle_);
    m_isGrabbing_ = false;
  }
  MV_CC_CloseDevice(handle_);
  MV_CC_DestroyHandle(handle_);
  handle_ = nullptr;
}