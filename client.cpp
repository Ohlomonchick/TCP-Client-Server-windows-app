#include "socket_entity.h"

template<size_t buffer_size>
class Client: public app_entity {
public:
    Client(const std::string&& server_ip, size_t port=8000) {
        format_ip();
        init_winSock();
        init_socket();
        connect_to_server();
        listen_clients();
    }

    void init_socket() override {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (client_socket == INVALID_SOCKET) {
            closesocket(client_socket);
            WSACleanup();
            throw std::runtime_error("Error initialization socket #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "Client socket initialization is OK" << std::endl;
    }

    void connect_to_server() {
        init_serInfo();
        error_status = connect(client_socket, (sockaddr*)&servInfo, sizeof(servInfo));

        if (error_status != 0) {
            closesocket(client_socket);
            WSACleanup();
            throw std::runtime_error("Connection to Server is FAILED. Error #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << std::endl;
    }

private:
    SOCKET client_socket;

};