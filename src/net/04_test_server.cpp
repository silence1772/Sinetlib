#include "server.h"
#include "looper.h"

int main()
{
    Looper loop;

    Server s(&loop, 8888, 3);
    s.Start();

    loop.Start();
}