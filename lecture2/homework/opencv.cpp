#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "io/camera.hpp"
#include "tasks/apriltag_detector.hpp"
#include "tools/img_tools.hpp"

int main()
{
    // 1.初始化并打开相机（复用你之前的相机类）
    Camera my_camera;

    // 2.构造AprilTag检测器对象，传入yaml配置文件路径
    auto_charge::AprilTagDetector tag_detector("config/apriltag_config.yaml");

    int frame_count = 0;
    while(true)
    {
        cv::Mat img = my_camera.Read();
        if(img.empty())
        {
            std::cout << "丢帧\n";
            continue;
        }

        // =========核心调用detect=========
        // 输入图像，返回所有识别到的标签
        std::vector<auto_charge::TagDetection> tags = tag_detector.detect(img);

        // 遍历每一个检测到的tag
        for(const auto& tag : tags)
        {
            std::cout << "识别到Tag id = " << tag.id << std::endl;

            // 画tag的4个角点
            for(size_t i = 0; i < 4; i++)
            {
                cv::line(img, tag.corners[i], tag.corners[(i+1)%4], cv::Scalar(0,0,255),2);
            }
            // 在tag旁边写id文字
            cv::putText(img, std::to_string(tag.id), tag.corners[0],
                        cv::FONT_HERSHEY_SIMPLEX,0.6,cv::Scalar(0,0,255),2);

            // tag.rvec / tag.tvec：PnP解算出来的旋转、平移向量，可以拿来做距离、姿态计算
        }

        cv::imshow("apriltag", img);
        int key = cv::waitKey(1) & 0xFF;
        if(key == 'q') break;
        frame_count++;
    }
    return 0;
}
