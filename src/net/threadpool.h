#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <memory>

class Looper;
class LooperThread;

class ThreadPool
{
public:
    ThreadPool(Looper* base_loop, int thread_num);
    ~ThreadPool();

    void Start();

    // 取出loop消费
    Looper* TakeOutLoop();
private:
    // 线程池类所在的线程
    Looper* base_loop_;

    // 线程池中线程数量
    int thread_num_;
    // 指向池中要取出的下一个线程
    int next_;

    std::vector<std::unique_ptr<LooperThread>> looper_threads_;
    std::vector<Looper*> loopers_;
};

#endif // THREAD_POOL_H