#include "app_entity.h"

template<size_t buffer_size>
class Client: public app_entity {
public:
    Client(const std::string&& server_ip, size_t port=8000) {
        PORT=port;

        SERVER_IP = new char[server_ip.size()];
        for (size_t i = 0; i < server_ip.size(); i++) {
            SERVER_IP[i] = server_ip[i];
        }
        SERVER_IP[server_ip.size()] = '\0';
        format_ip();
    }

    ~Client() {
        closesocket(client_socket);
        WSACleanup();
    }

    void messenger() override {
        init_winSock();
        init_socket();
        connect_to_server();

        while (true) {

            std::cout << "Your (Client) message to Server: ";
            fgets(clientBuff.data(), clientBuff.size(), stdin);

            // Check whether client like to stop chatting
            if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
                shutdown(client_socket, SD_BOTH);
                closesocket(client_socket);
                WSACleanup();
                return;
            }

            packet_size = send(client_socket, clientBuff.data(), clientBuff.size(), 0);

            if (packet_size == SOCKET_ERROR) {
                closesocket(client_socket);
                WSACleanup();
                throw std::runtime_error("Can't send message to Server. Error #" + std::to_string(WSAGetLastError()));
            }

            packet_size = recv(client_socket, servBuff.data(), servBuff.size(), 0);

            if (packet_size == SOCKET_ERROR) {
                closesocket(client_socket);
                WSACleanup();
                throw std::runtime_error("Can't receive message from Server. Error #" + std::to_string(WSAGetLastError()));
            }
            else
                std::cout << "Server message: " << servBuff.data() << std::endl;

        }
    }

private:
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

    SOCKET client_socket;
    std::array<char, buffer_size> servBuff;
    std::array<char, buffer_size> clientBuff;

};

int main() {
    Client<1024> my_client("127.0.0.1", 8000);
    my_client.messenger();
}