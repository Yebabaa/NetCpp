#include <iostream>

#include "net.h"

struct data {
    char str[64];
};

int main()
{
    net::init();
    net::tcp::TCPClient<data> client;
    client.Init("127.0.0.1", 7534);
    client.connectserver();
    data d;

    while (true)
    {
        ZeroMemory(d.str, 64);
        d = client.receive();
        std::cout << "\nSERVER: " << d.str << '\n';
    }

    client.cleanup();

    return 0;
}