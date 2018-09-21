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

    Looper* TakeOutLoop();
private:
    Looper* base_loop_;

    int thread_num_;
    int next_;
    std::vector<std::unique_ptr<LooperThread>> looper_threads_;
    std::vector<Looper*> loopers_;
};

#endif // THREAD_POOL_H