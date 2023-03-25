#include "app_entity.h"

template<size_t buffer_size>
class Server: public app_entity {
public:
    Server(const std::string&& server_ip, size_t port=8000) {
        PORT=port;
        CLIENT_IP = new char[22];

        SERVER_IP = new char[server_ip.size() + 1];
        for (size_t i = 0; i < server_ip.size(); i++) {
            SERVER_IP[i] = server_ip[i];
        }
        SERVER_IP[server_ip.size()] = '\0';
        format_ip();
    }

    ~Server() {
        closesocket(server_socket);
        closesocket(ClientConn);
        WSACleanup();
        delete[] CLIENT_IP;
    }

    void messenger() override {
        init_winSock();
        init_socket();
        bind_to_socket();
        listen_clients();

        while (true) {
            packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);					// Receiving packet from client. Program is waiting (system pause) until receive
            std::cout << "Client's message: " << servBuff.data() << std::endl;

            std::cout << "Your (host) message: ";
            fgets(clientBuff.data(), clientBuff.size(), stdin);

            // Check whether server would like to stop chatting
            if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
                shutdown(ClientConn, SD_BOTH);
                closesocket(server_socket);
                closesocket(ClientConn);
                WSACleanup();
                return;
            }

            packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

            if (packet_size == SOCKET_ERROR) {
                closesocket(server_socket);
                closesocket(ClientConn);
                WSACleanup();
                throw std::runtime_error("Can't send message to Client. Error #" + std::to_string(WSAGetLastError()));
            }

        }
    }

private:
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

        ClientConn = accept(server_socket, (sockaddr*)&clientInfo, &clientInfo_size);

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

    SOCKET server_socket;
    sockaddr_in clientInfo;
    char * CLIENT_IP;
    SOCKET ClientConn;
    std::array<char, buffer_size> servBuff;
    std::array<char, buffer_size> clientBuff;
};


int main() {
    Server<1024> my_server("127.0.0.1", 8000);
    my_server.messenger();
}
