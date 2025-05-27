#include <iostream>

#include "net.h"

struct data {
    char str[64];
};

int main()
{
    net::init();

    net::tcp::TCPServer<data> server;
    server.Init(7534);
    server.acceptclient();
    data d;

    while (true)
    {
        std::cout << "\n> ";
        ZeroMemory(d.str, 64);
        std::cin.getline(d.str, 64);
        server.sendclient(d);
    }

    server.cleanup();

    return 0;
}