#include <iostream>
#include <opencv2/opencv.hpp>

// ======================= 作业 =======================
// 1. 读取 ../assets/demo.jpg
// 2. 使用 cvtColor 把图像转为灰度图（颜色空间：BGR2GRAY）
// 3. 使用 imwrite 把灰度图保存为 gray.jpg
// 4. 在灰度图上用 circle 画一个圆，标记你要"瞄准"的位置
// 5. 显示灰度图，按任意键退出
// ====================================================

int main()
{
  // 1. 读取图片，路径相对于"运行目录"（build 文件夹）
  cv::Mat img = cv::imread("assets/demo.jpg");

  // 2. 检查是否读取成功
  if (img.empty()) {
    std::cout << "读取图片失败！请检查路径" << std::endl;
    return -1;
  }

  // 3. 转为灰度图
  cv::Mat grey;
  cv::cvtColor(img, grey, cv::COLOR_BGR2GRAY);

  // 4. 保存灰度图
  cv::imwrite("assets/gray.jpg", grey);

  // 5. 在灰度图上画一个圆
  cv::Point aim_point(grey.cols / 2, grey.rows / 2);
  cv::circle(grey, aim_point, 80, cv::Scalar(255), 3);

  // 6. 显示灰度图
  cv::imshow("Gray Image", grey);
  cv::waitKey(0);
  std::cout << "按任意键退出" << std::endl;

  return 0;
}
