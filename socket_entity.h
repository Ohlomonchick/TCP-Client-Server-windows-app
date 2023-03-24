#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")


#ifndef SOCKETS_SOCKET_ENTITY_H
#define SOCKETS_SOCKET_ENTITY_H


class app_entity {
public:
    explicit app_entity(const std::string&& server_ip, size_t port=8000) {
        PORT=port;
        SERVER_IP = new char[server_ip.size()];
        for (size_t i = 0; i < server_ip.size(); i++) {
            SERVER_IP[i] = server_ip[i];
        }
    }

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
        ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

        servInfo.sin_family = AF_INET;
        servInfo.sin_addr = numericIP;
        servInfo.sin_port = htons(PORT);
    }

    virtual void init_socket() = 0;

protected:
    char * SERVER_IP;
    size_t PORT;
    int error_status;
    WSADATA wsData;
    in_addr numericIP;
    sockaddr_in servInfo;
};


#endif //SOCKETS_SOCKET_ENTITY_H
