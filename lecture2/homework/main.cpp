#include "io/camera.hpp"
#include "opencv2/opencv.hpp"
#include "tasks/armor.hpp"
#include "tasks/yolo.hpp"
#include "tools/img_tools.hpp"

int main()
{
  // 初始化相机、yolo类
  Camera my_camera;
  auto_aim::YOLO my_yolo("configs/yolo.yaml", true);
  int frame_count = 0;
  if(!my_camera.Open()){
    std::cout<<"No Camera"<<std::endl;
    return 0;
  }
  while (1) {
    // 调用相机读取图像
    cv::Mat img = my_camera.Read();
    //判空，丢帧就跳过本次循环
    if(img.empty())
        {
            std::cout << "图像为空\n";
            continue;
        }

    // 调用yolo识别装甲板
    std::list<auto_aim::Armor> armors = my_yolo.detect(img,frame_count);

    // 处理每个识别到的装甲板
    for (auto & armor : armors) {

      // 画闭合四边形
      tools::draw_points(img, armor.points, cv::Scalar(0,0,255), 5);
      std::cout << "识别到装甲板: " << auto_aim::COLORS[armor.color] << " " << auto_aim::ARMOR_NAMES[armor.name] << std::endl;

      //四边形顶部居中写文字
      cv::Point2f topMid;
      topMid.x = (armor.points[0].x + armor.points[1].x) / 2.0f;
      topMid.y = (armor.points[0].y + armor.points[1].y) / 2.0f - 5.0f;
      std::string label = auto_aim::COLORS[armor.color] + " " + auto_aim::ARMOR_NAMES[armor.name];
      cv::putText(img, label, topMid, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,255,0), 5);
    }

    // 显示图像
    cv::resize(img, img , cv::Size(640, 480));
    cv::imshow("img", img);
    int key = cv::waitKey(1) & 0xFF;
    if(key == 'q') break;
    frame_count++;
  }
  my_camera.Close();
  std::cout<<"Camera closed"<<std::endl;
  cv::destroyAllWindows();

  return 0;
}