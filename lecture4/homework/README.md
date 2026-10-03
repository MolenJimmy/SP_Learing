# nav_lecture4小作业：Qos_debugger
这道题的目标就是让你快速上手、理解什么是ros。抛开复杂的概念，ros本质上完成的任务就是便利的进程间通信。比如，我有两个进程，一个进程发布雷达数据，另一个进程接收。使用ros就可以方便的完成通讯。你可以搜索以下，发送信息有哪些类型，分别有何特点。特别注意，不同的信息传输方式有不同的质量要求。你不会允许送的外卖没到你手上，但是一个电话过来，也许漏接了也无所谓，可能只是个诈骗。ros2也是这样。重点关注这一点会对这道题有所帮助

> 环境要求：ROS2 Humble

## 包结构

```
src/
  nav_hw_interfaces/     # 接口包：只放 .msg，无业务代码
    msg/SensorData.msg   # 可以打开.msg文件查看接口详细内容
  qos_debugger/          # 业务节点包
    src/qos_debugger_pub.cpp   # 发布 /SensorData
    src/qos_debugger_sub.cpp   # 订阅 /SensorData
```

## 编译

```bash
cd lecture4/homework
colcon build 
source install/setup.bash
```


## 任务一：实现pub和sub的通信

```bash
ros2 -h     //有忘记的命令就输入-h去查询用法
```

**现象**：启动pub和sub节点后sub节点订阅不到任何消息
提示：如果两个节点不能通过话题通信，我们应该如何区查看话题的详细信息（有没有相关的命令）
任务一仅修复qos_debugger_pub.cpp的一处或几处代码即可完成


---

## 任务二：为什么收到的消息会丢包？/(ㄒoㄒ)/~~

第一问找到问题并修改代码后，记得重新
```colcon build```
```source install/setup.bash```
**现象**：sub会打印黄色的warning输出告诉你丢包的序列，每秒还会打印出丢包率

提示：
有没有什么命令可以查看节点的配置(ros2 param -h)
可以通过修复qos_debugger_sub.cpp中的一处或几处代码解决该问题（可能会有多种解决方法）

## 任务三：把收到的消息的帧率计算并打印出来（放在定时器回调函数中每秒打印一次即可）
补全qos_debugger_sub.cpp即可



在下面按顺序完成三个任务，要求把用到的命令放入代码块中并讲解命令，每一问最好加入自己的理解

1、查看话题的详细信息使用命令
```bash
ros2 topic list                                              # 查看所有话题
ros2 topic echo /sensor_data                              # 打印话题内容
```
修复：订阅这的reliablility参数默认是reliable，而发布者是best_effort，不匹配导致sub节点订阅不到任何消息，只需要把发布者的reliablility默认参数改为reliable即可。

2、查看节点的配置：
```bash
ros2 param list /sensor_subscriber     #查看订阅者的参数
```
可以看到订阅者有一个callback_delay_ms参数设定的值是30ms，阻塞订阅线程。然而发布者rate参数为100即发布间隔为10ms，订阅者接受间隔明显太慢，导致丢包，只需要把callback_delay_ms改成尽可能小（如1ms），订阅阻塞间隔小于发布间隔则能够接收到完整的信息。

3、使用now = std::chrono::steady_clock::now()记录当前时间减去上一步记录的时间last_report_time_，得到时间间隔，received_count_当前接受的信息条数减去上一步的条数last_received_count_得到这段时间内接收的条数，接收条数除以时间间隔得到频率。最后在更新记录上一步时间和条数的私有变量。代码如下：
```bash
auto now = std::chrono::steady_clock::now();
    double time_delta = std::chrono::duration<double>(now - last_report_time_).count();

    uint32_t msg_delta = received_count_ - last_received_count_;
    double recv_fps = msg_delta / time_delta;

    RCLCPP_INFO(
        this->get_logger(),
        "接收帧率: %.2f Hz",
        recv_fps);

    last_received_count_ = received_count_;
    last_report_time_ = now;
```