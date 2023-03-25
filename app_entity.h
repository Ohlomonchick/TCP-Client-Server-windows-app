#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdio>
#include <iostream>
#include <array>

#pragma comment(lib, "Ws2_32.lib")


#ifndef SOCKETS_SOCKET_ENTITY_H
#define SOCKETS_SOCKET_ENTITY_H


class app_entity {
public:
    app_entity() = default;

    app_entity(const std::string&& server_ip, size_t port) {
        PORT=port;
        error_status = 0;

        SERVER_IP = new char[server_ip.size() + 1];
        for (size_t i = 0; i < server_ip.size(); i++) {
            SERVER_IP[i] = server_ip[i];
        }
        SERVER_IP[server_ip.size()] = '\0';
        format_ip();
    }

    ~app_entity() {
        WSACleanup();
        delete[] SERVER_IP;
    }

    virtual void messenger() = 0;

protected:
    void format_ip() {
        error_status = inet_pton(AF_INET, SERVER_IP, &numericIP);

        if (error_status <= 0) {
            throw std::runtime_error("Error in IP translation to special numeric format");
        }
    }

    void init_winSock() {
        error_status = WSAStartup(MAKEWORD(2,2), &wsData);

        if (error_status != 0) {
            throw std::runtime_error("Error WinSock version initializaion #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "WinSock initialization is OK" << std::endl;
    }

    void init_serInfo() {
        ZeroMemory(&servInfo, sizeof(servInfo));	// Zeroing memory for servInfo structure

        servInfo.sin_family = AF_INET;
        servInfo.sin_addr = numericIP;
        servInfo.sin_port = htons(PORT);
    }

    virtual void init_socket() = 0;

    char * SERVER_IP;
    size_t PORT;
    int error_status;
    WSADATA wsData;
    in_addr numericIP;
    sockaddr_in servInfo;
    size_t packet_size = 0;
};


#endif //SOCKETS_SOCKET_ENTITY_H
