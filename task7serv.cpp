#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024
// this function helps to receieve the messages
void recmes(int client) {
    char buffer[BUFFER_SIZE];

    while (true) {
        int bytes = recv(client, buffer, BUFFER_SIZE, 0);

        if (bytes <= 0) {
            std::cout << "\nClient disconnected.\n";
            break;
        }

        buffer[bytes] = '\0';
        std::cout << "\nClient: " << buffer << std::endl;
    }
}
// handles sending messagesto the client
void senmes(int client) {
    std::string msg;

    while (true) {
        std::getline(std::cin, msg);
        send(client, msg.c_str(), msg.size(), 0);
    }
}
//create tcp socket
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//structure to store server address
    sockaddr_in ad{};
    ad.sin_family = AF_INET;
    ad.sin_port = htons(PORT);
    ad.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&ad, sizeof(ad));
    listen(server_fd, 5);

    std::cout << "Server running on port 8080...\n";

    while (true) {
        std::cout << "Waiting for a client...\n";

        int client = accept(server_fd, nullptr, nullptr);
        std::cout << "Client connected!\n";

        // create da threads
        std::thread recvThread(recmes, client);
        std::thread sendThread(senmes, client);

        recvThread.join();
        sendThread.join();

        close(client);
    }

    close(server_fd);
}