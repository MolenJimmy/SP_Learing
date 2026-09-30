#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat addNoiseColor(const cv::Mat & src)
{
  CV_Assert(src.type() == CV_8UC3);  // 输入必须是 8-bit unsigned, 3 channels

  // Mat内部只是header（行、列、类型、数据指针）,dst=src不会复制像素，只是两个Mat对象指向同一块像素内存
  cv::Mat dst = src.clone();

  // 创建随机噪声
  cv::Mat noise(src.rows, src.cols, CV_8UC1);
  cv::randu(noise, 0, 50);
  cv::Mat noise3;
  cv::merge(std::vector<cv::Mat>{noise, noise, noise}, noise3);

  // 给图像加入噪声
  cv::add(dst, noise3, dst);

  return dst;
}
int main()
{
  cv::Mat img = cv::imread("ex3/image.jpg", cv::IMREAD_COLOR);
  if (img.empty()) {
    std::cerr << "image.jpg not found!" << std::endl;
    return -1;
  }

  cv::Mat noisy = addNoiseColor(img);

  // 缩放原始图像和噪声图像
  cv::Mat img_resized, noisy_resized;
  cv::resize(img, img_resized, cv::Size(), 0.5, 0.5);
  cv::resize(noisy, noisy_resized, cv::Size(), 0.5, 0.5);

  cv::imshow("original", img_resized);  // 显示 original 图像
  cv::imshow("noisy", noisy_resized);   // 显示 noisy 图像
  cv::waitKey(0);                       // 暂停程序，按任意键退出
  return 0;
}