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
        packet_size = 0;

        SERVER_IP = new char[server_ip.size() + 1];
        for (size_t i = 0; i < server_ip.size(); i++) {
            SERVER_IP[i] = server_ip[i];
        }
        is_active = false;
        SERVER_IP[server_ip.size()] = '\0';
        format_ip();
    }

    app_entity(const app_entity&) = delete;
    app_entity& operator=(const app_entity&) = delete;

    app_entity(app_entity&& other) noexcept {
        this->operator=(std::forward<app_entity&&>(other));
    }

    app_entity& operator=(app_entity&& other) noexcept {
        if (this != &other) {
            other.is_active = false;
            SERVER_IP = other.SERVER_IP;
            other.SERVER_IP = nullptr;
            PORT = other.PORT;
            error_status = other.error_status;
            wsData = other.wsData;
            numericIP = other.numericIP;
            servInfo = other.servInfo;
            packet_size = other.packet_size;
        }
        return *this;
    }

    ~app_entity() {
        if (is_active)
            WSACleanup();

        delete[] SERVER_IP;
    }

    virtual void messenger() = 0;

protected:
    void format_ip() {
        // converting given string api to bytes
        error_status = inet_pton(AF_INET, SERVER_IP, &numericIP);

        if (error_status <= 0) {
            throw std::runtime_error("Error in IP translation to special numeric format");
        }
    }

    void init_winSock() {
        // asking system to tell its socket version to us
        error_status = WSAStartup(MAKEWORD(2,2), &wsData);

        if (error_status != 0) {
            throw std::runtime_error("Error WinSock version initializaion #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "WinSock initialization is OK" << std::endl;
    }

    void init_serInfo() {
        //storing info about the server IP:PORT
        ZeroMemory(&servInfo, sizeof(servInfo));	// Zeroing memory for servInfo structure

        servInfo.sin_family = AF_INET;
        servInfo.sin_addr = numericIP;
        servInfo.sin_port = htons(PORT);
    }

    virtual void init_socket() = 0;

    char * SERVER_IP;
    size_t PORT; //port number
    int error_status;
    WSADATA wsData; //this struct stores data containing OC and socket versions
    in_addr numericIP; //byte to byte representation of IPv4
    sockaddr_in servInfo; //info about server (both client and server should know what he is connected to)
    size_t packet_size; //packet size is calculated while server get info about the users by recv() or vice versa
    bool is_active; //is messenger started
};


#endif //SOCKETS_SOCKET_ENTITY_H
