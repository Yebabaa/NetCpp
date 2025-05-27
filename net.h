#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    void init()
    {
        WSADATA wsData;
        WSAStartup(MAKEWORD(2, 2), &wsData);
    }

    namespace tcp {
        enum class TCP_CONNECTION_EXCEPTIONS {
            NET_ERROR, NET_DISCONNECT
        };

        template<typename DATATYPE> class TCPServer
        {
        public:
            TCPServer() = default;

            void Init(u_short prt)
            {
                listen_socket = socket(AF_INET, SOCK_STREAM, 0);
                hint.sin_family = AF_INET;
                port = prt;
                hint.sin_port = htons(port);
                hint.sin_addr.S_un.S_addr = INADDR_ANY;
                bind(listen_socket, (sockaddr*)&hint, sizeof(hint));
                listen(listen_socket, SOMAXCONN);
            }

            void acceptclient()
            {
                int clientsize = sizeof(client);
                client_socket = accept(listen_socket, (sockaddr*)&client, &clientsize);
                if (client_socket == INVALID_SOCKET)
                    error = TCP_CONNECTION_EXCEPTIONS::NET_ERROR;
                closesocket(listen_socket);
            }

            DATATYPE receive()
            {
                int dataSize;

                if (recv(client_socket, reinterpret_cast<char*>(&dataSize), sizeof(dataSize), 0) <= 0) {
                    error = TCP_CONNECTION_EXCEPTIONS::NET_ERROR;
                    return data;
                }

                char* buffer = new char[dataSize];
                int totalBytesReceived = 0;

                while (totalBytesReceived < dataSize)
                {
                    int bytesReceived = recv(client_socket, buffer + totalBytesReceived, dataSize - totalBytesReceived, 0);
                    if (bytesReceived <= 0) {
                        error = TCP_CONNECTION_EXCEPTIONS::NET_ERROR;
                        break;
                    }
                    totalBytesReceived += bytesReceived;
                }

                memcpy(&data, buffer, dataSize);
                delete[] buffer;
                return data;
            }

            void sendclient(const DATATYPE& DataToSend)
            {
                int dataSize = sizeof(DataToSend);
                send(client_socket, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize), 0);
                send(client_socket, reinterpret_cast<const char*>(&DataToSend), dataSize, 0);
            }

            void cleanup()
            {
                closesocket(client_socket);
                WSACleanup();
            }

        private:
            SOCKET listen_socket;
            SOCKET client_socket;
            TCP_CONNECTION_EXCEPTIONS error;
            u_short port;
            DATATYPE data;
            SOCKADDR_IN hint;
            SOCKADDR_IN client;
        };

        template<typename DATATYPE> class TCPClient
        {
        public:
            TCPClient() = default;

            void Init(std::string ipAddr, u_short prt)
            {
                ipAddress = ipAddr;
                port = prt;
                sock = socket(AF_INET, SOCK_STREAM, 0);
                hint.sin_family = AF_INET;
                hint.sin_port = htons(port);
                inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
            }

            void connectserver()
            {
                int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
            }

            DATATYPE receive()
            {
                int dataSize;

                if (recv(sock, reinterpret_cast<char*>(&dataSize), sizeof(dataSize), 0) <= 0) {
                    error = TCP_CONNECTION_EXCEPTIONS::NET_ERROR;
                    return data;
                }

                char* buffer = new char[dataSize];
                int totalBytesReceived = 0;

                while (totalBytesReceived < dataSize)
                {
                    int bytesReceived = recv(sock, buffer + totalBytesReceived, dataSize - totalBytesReceived, 0);
                    if (bytesReceived <= 0) {
                        error = TCP_CONNECTION_EXCEPTIONS::NET_ERROR;
                        break;
                    }
                    totalBytesReceived += bytesReceived;
                }

                memcpy(&data, buffer, dataSize);
                delete[] buffer;
                return data;
            }

            void sendserver(const DATATYPE& DataToSend)
            {
                int dataSize = sizeof(DataToSend);
                send(sock, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize), 0);
                send(sock, reinterpret_cast<const char*>(&DataToSend), dataSize, 0);
            }

            void cleanup()
            {
                closesocket(sock);
                WSACleanup();
            }

        private:
            std::string ipAddress;
            u_short port;
            TCP_CONNECTION_EXCEPTIONS error;
            SOCKET sock;
            SOCKADDR_IN hint;
            DATATYPE data;
        };
    }
};