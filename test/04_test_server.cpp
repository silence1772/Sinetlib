#include "server.h"
#include "looper.h"
#include "currentthread.h"
#include <iostream>
#include "timestamp.h"

void OnMessage(const std::shared_ptr<Connection>& conn)
{
    std::cout << CurrentThread::GetTid() << " OnMessage" << std::endl;
    conn->Send("Hi");
}

void test()
{
    std::cout << "test" << std::endl;
}

int main()
{
    Looper loop;

    Server s(&loop, 8888, 3);
    s.SetMessageArrivalCB(OnMessage);
    s.Start();

    loop.RunTaskAfter(test, Second(3));
    loop.Start();
}