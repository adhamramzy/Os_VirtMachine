#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024

void receiveMessages(int sock) {
    char buffer[BUFFER_SIZE];

    while (true) {
        int bytes = recv(sock, buffer, BUFFER_SIZE, 0);

        if (bytes <= 0) {
            std::cout << "\nDisconnected from server.\n";
            break;
        }

        buffer[bytes] = '\0';
        std::cout << "\nServer: " << buffer << std::endl;
    }
}

void sendMessages(int sock) {
    std::string msg;

    while (true) {
        std::getline(std::cin, msg);
        send(sock, msg.c_str(), msg.size(), 0);
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    std::cout << "Connected to server!\n";

    std::thread recvThread(receiveMessages, sock);
    std::thread sendThread(sendMessages, sock);

    recvThread.join();
    sendThread.join();

    close(sock);
}