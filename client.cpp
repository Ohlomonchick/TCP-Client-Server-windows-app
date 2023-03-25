#include "app_entity.h"

template<size_t buffer_size>
class Client: public app_entity {
public:
    Client(const std::string&& server_ip, size_t port=8000): app_entity(std::forward<const std::string&&>(server_ip), port) {}

    Client(const Client<buffer_size>& other) = delete;
    Client<buffer_size>& operator=(const Client<buffer_size>& other) = delete;

    Client(Client<buffer_size>&& other) noexcept {
        this->operator=(std::forward<Client<buffer_size>&&>(other));
    }

    Client<buffer_size>& operator=(Client<buffer_size>&& other) noexcept {
        app_entity::operator=(std::forward<Client<buffer_size>&&>(other));
        if (this != &other) {
            client_socket = other.client_socket;
            servBuff = std::move(other.servBuff);
            clientBuff = std::move(other.clientBuff);;
        }
        return *this;
    }



    ~Client() {
        //sockets will be closed since destructor call is guaranteed after exception trows
        if (is_active)
            closesocket(client_socket);
    }

    void messenger() override {
        init_winSock();
        init_socket();
        connect_to_server();
        is_active = true;

        while (true) {

            std::cout << "Your (Client) message to Server: ";
            fgets(clientBuff.data(), clientBuff.size(), stdin);

            // Check whether client like to stop chatting
            if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
                shutdown(client_socket, SD_BOTH);
                WSACleanup();
                closesocket(client_socket);
                is_active = false;
                return;
            }

            packet_size = send(client_socket, clientBuff.data(), clientBuff.size(), 0);

            if (packet_size == SOCKET_ERROR) {
                throw std::runtime_error("Can't send message to Server. Error #" + std::to_string(WSAGetLastError()));
            }

            packet_size = recv(client_socket, servBuff.data(), servBuff.size(), 0);

            if (packet_size == SOCKET_ERROR) {
                throw std::runtime_error("Can't receive message from Server. Error #" + std::to_string(WSAGetLastError()));
            }
            else
                std::cout << "Server message: " << servBuff.data() << std::endl;

        }
    }

private:
    void init_socket() override {
        //initializing client_socket
        client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (client_socket == INVALID_SOCKET) {
            throw std::runtime_error("Error initialization socket #" + std::to_string(WSAGetLastError()));
        }
        else
            std::cout << "Client socket initialization is OK" << std::endl;
    }

    void connect_to_server() {
        //trying to connect to server
        init_serInfo();
        error_status = connect(client_socket, (sockaddr*)&servInfo, sizeof(servInfo));

        if (error_status != 0) {
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