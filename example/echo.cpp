#include "server.h"
#include <iostream>

void OnConnection(const std::shared_ptr<Connection>& conn)
{
    std::cout << "OnConnection" << std::endl;
}

void OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf, Timestamp t)
{
    std::cout << "OnMessage" << std::endl;
    conn->Send(buf->GetReadablePtr(), buf->GetReadableSize());
    buf->RetrieveAll();
}

void OnReply(const std::shared_ptr<Connection>& conn)
{
    std::cout << "OnReply" << std::endl;
}

void OnClose(const std::shared_ptr<Connection>& conn)
{
    std::cout << "OnClose" << std::endl;
}

int main()
{
    Looper loop;
    Server s(&loop, 8888, 4);

    s.SetConnectionEstablishedCB(OnConnection);
    s.SetMessageArrivalCB(OnMessage);
    s.SetReplyCompleteCB(OnReply);
    s.SetConnectionCloseCB(OnClose);

    s.Start();
    loop.Start();
}