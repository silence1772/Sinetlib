#include "looper.h"
#include "eventbase.h"
#include "threadpool.h"
#include <string.h> // for bzero()
#include <unistd.h> // for close()
#include <sys/timerfd.h>
#include <iostream>

Looper* g_loop;

void timeout()
{
    std::cout << "==============================" << std::endl;
    std::cout << "user function callback in here" << std::endl;
    std::cout << "this will quit the EventLoop" << std::endl;
    std::cout << "==============================" << std::endl;
    g_loop->Quit();
}


int main()
{
    Looper loop;
    g_loop = &loop;
    
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    std::shared_ptr<EventBase> eventbase = std::make_shared<EventBase>(timerfd);
    eventbase->SetReadCallback(timeout);
    eventbase->EnableReadEvents();
    loop.AddEventBase(eventbase);

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 3;
    timerfd_settime(timerfd, 0, &howlong, NULL);

    ThreadPool tp(&loop, 5);
    tp.Start();

    loop.Start();

    close(timerfd);
    return 0;
}