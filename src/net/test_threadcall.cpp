#include "looper.h"
#include "eventbase.h"
#include "threadpool.h"
#include <string.h> // for bzero()
#include <unistd.h> // for close()
#include <sys/timerfd.h>
#include <iostream>

ThreadPool* g_tp;
int g_timefd;

void task()
{
    std::cout << "In " << CurrentThread::GetTid() << ": handle task" << std::endl;
}

void timeout()
{
    close(g_timefd);
    std::cout << "In " << CurrentThread::GetTid() << ": dispatch task" << std::endl;
    g_tp->TakeOutLoop()->RunTask(task);
}


int main()
{
    Looper loop;
    
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    g_timefd = timerfd;
    std::shared_ptr<EventBase> eventbase = std::make_shared<EventBase>(timerfd);
    eventbase->SetReadCallback(timeout);
    eventbase->EnableReadEvents();
    loop.AddEventBase(eventbase);
    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 3;
    timerfd_settime(timerfd, 0, &howlong, NULL);

    ThreadPool tp(&loop, 2);
    g_tp = &tp;
    tp.Start();

    loop.Start();

    
    return 0;
}