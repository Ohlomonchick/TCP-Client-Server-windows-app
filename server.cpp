#include "socket_entity.h"

template<size_t buffer_size>
class Server: public app_entity {
public:
    Server(const std::string&& server_ip, size_t port=8000) {
        CLIENT_IP = new char[22];

        format_ip();
        init_winSock();
        init_socket();
        bind_to_socket();
        listen_clients();

    }

    void init_socket() override {
        // TCP IPv4 socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket == INVALID_SOCKET) {
            closesocket(server_socket);
            WSACleanup();
            throw std::runtime_error("Error initialization socket #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "Server socket initialization is OK" << std::endl;

    }

    void bind_to_socket() {
        init_serInfo();
        error_status = bind(server_socket, (sockaddr*)&servInfo, sizeof(servInfo));

        if (error_status != 0) {
            closesocket(server_socket);
            WSACleanup();
            throw std::runtime_error("Error Socket binding to server info. Error #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "Binding socket to Server info is OK" << std::endl;
    }

    void listen_clients() {
        error_status = listen(server_socket, SOMAXCONN);

        if (error_status != 0) {
            closesocket(server_socket);
            WSACleanup();
            throw std::runtime_error("Can't start to listen to. Error #" + std::to_string(WSAGetLastError()));
        }
        else {
            std::cout << "Listening..." << std::endl;
        }

        //Client socket creation and acception in case of connection
        ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

        int clientInfo_size = sizeof(clientInfo);

        SOCKET ClientConn = accept(server_socket, (sockaddr*)&clientInfo, &clientInfo_size);

        if (ClientConn == INVALID_SOCKET) {
            closesocket(server_socket);
            closesocket(ClientConn);
            WSACleanup();
            throw std::runtime_error("Client detected, but can't connect to a client. Error #" + std::to_string(WSAGetLastError()));
        }
        else {
            // Convert connected client's IP to standard string format
            inet_ntop(AF_INET, &clientInfo.sin_addr, CLIENT_IP, INET_ADDRSTRLEN);
            std::cout << "Connection to a client established successfully with IP address " << CLIENT_IP  << std::endl;

        }
    }

private:
    SOCKET server_socket;
    sockaddr_in clientInfo;
    char * CLIENT_IP;

};
