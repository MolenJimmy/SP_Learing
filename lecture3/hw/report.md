# 项目理解报告

请尽量使用自己的语言回答以下问题。可以引用少量关键代码或伪代码，但不要只粘贴实现。
完成一节后删除该节末尾的待填写标记；本地检查会拒绝仍有未完成章节的报告。

## 1. 图像生命周期与所有权

解释本项目中图像源为什么会复用缓冲区，以及 `cv::Mat` 的普通复制对底层像素数据
意味着什么。说明你的修改让一个 `Frame` 在进入队列后拥有什么，并解释为何后续读取
不会再改变它。

复用缓冲区减少频繁内存分配、释放，降低内存碎片、减少开销；
cv::Mat的拷贝是浅拷贝，frame.image = buffer_，frame.image获得与buffer_相同的指针，没有额外复制一份数据，之后buffer_的数据改变时，frame.image一起改变;
修改后为深拷贝，在堆开辟一块全新独立的像素内存，frame.image获得新的指针与数据；

## 2. 并发处理与恰好一次

结合 `BlockingQueue` 的 `push`、`pop` 和 `close` 行为，解释多个 worker 如何分工。
为什么你的实现既不会漏掉已经入队的帧，也不会重复处理同一帧？输入耗尽时，正在等待
以及仍在处理数据的 worker 分别会怎样？

多个 worker 如何分工：Pipeline 启动先创建全部 worker 线程，所有worker卡在 while(queue_.pop(frame))；所有worker一起阻塞在pop()，休眠等待任务；Producer线程循环，读到一帧 就queue_.push(frame)；push放入一帧之后，阻塞队列唤醒其中一个正在等待的 worker而不是全部唤醒；被唤醒的worker拿到这一帧，退出 pop，进入workerLoop业务逻辑；处理完成后worker 回到循环顶部，再次调用pop()，继续等待下一帧。
为什么不会漏：workerloop循环的停止逻辑是queue_.pop(frame==false)即队列中没有图像了。
不会重复处理同一帧：元素从队列中被移除，所有权交给worker。
队列内部元素一旦被取出，队列不再持有这份Frame，其他worker不可能再拿到同一个帧。
输入耗尽时：正在等待的 worker阻塞在queue_.pop()，仍在处理数据的worker已经pop拿到 frame，正在执行图像处理。

## 3. 共享统计数据

指出哪些线程会读写 `Statistics`。解释原实现中的竞争为什么可能导致错误结果，并说明
你的同步方案提供了什么保证。还应说明取得快照时为什么是安全的。

哪些线程读写Statistics对象：1 个Producer线程和N个Worker工作线程
原实现的竞争为什么会产生错误结果：多个线程并发读写同一个int 变量，会出现丢失更新，最终统计数字偏小。
同步方案：给Statistics增加mutable std::mutex mtx_，所有修改操作全部用 lock_guard 上锁。
获取快照为什么安全：快照只读取不修改值。

## 4. 线程关闭协议

分别描述以下两条路径中的事件顺序，并解释为什么不会发生 `std::terminate`、悬空访问
或永久等待：

1. 调用者执行 `start()` 后显式调用 `wait()`；
2. 调用者执行 `start()` 后不调用 `wait()`，直接让 `Pipeline` 析构。

如果你的实现允许某个生命周期方法被重复调用，也请说明其行为；如果不允许，请说明前置条件。

1、创建N个worker线程+1个produce线程，workers全部阻塞在queue_.pop()等待帧，producer遍历图片，不断push帧入队列；图片全部读取完毕producer执行queue_.close()。调用pipeline.wait()，阻塞直到 producer 线程退出，之后等待所有 worker完成当前任务并退出，wait()返回，此时所有线程都已经 join完毕，Pipeline后续销毁。
不会std::terminate：wait()内部已经join全部线程，线程对象不再 joinable，销毁时不会触发 terminate
不会悬空访问：wait()阻塞直到所有子线程 return退出，子线程在wait()返回前就停止访问Pipeline成员。Pipeline对象等到wait()返回之后才会被销毁。
不会永久等待：queue_.close()保证队列空之后，阻塞在pop的worker会收到退出信号，正在处理帧的worker会完成当前帧后，下一次 pop返回false退出。join一定会完成。

2、pipeline.start()启动 producer、worker线程，started_=true，释放锁，线程并发运行。作用域结束，Pipeline 析构函数自动执行：调用queue_.close()：禁止新帧push，队列现存帧继续被worker消费；if (producer_.joinable()) producer_.join()，等待producer线程结束；遍历workers，等待所有worker处理完剩余任务并退出。
不会std::terminate：析构内部主动join所有joinable线程，等到所有子线程执行完毕，std::thread对象不再joinable之后，才销毁thread成员。
不会悬空访问：析构函数阻塞在join，直到所有子线程函数全部 return，子线程不再访问Pipeline任何成员，才继续销毁Pipeline 对象。子线程不会访问已经销毁的对象。
不会永久等待：queue_.close()唤醒等待pop的 worker，正在处理帧的worker 完成当前任务，下一次 pop 返回 false 退出，join 一定能完成。

生命周期方法重复调用的行为：start()不可重复调用。
start()内部上锁，若started_ == true，直接抛出std::runtime_error("Pipeline already started")，防止重复启动多组线程。wait()允许多次调用：第一次wait()，join 所有线程，线程变成 non-joinable。后续再次调用wait()：上锁，检测started_为true；对每个thread判断joinable()，已经join过的线程直接跳过 join，不会报错，不会阻塞。


