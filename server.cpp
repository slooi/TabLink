
// Server.cpp
#define SERVER_PORT 54000

#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverHint;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(SERVER_PORT);
    serverHint.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, (sockaddr*)&serverHint, sizeof(serverHint));

    listen(serverSocket, SOMAXCONN);
    std::cout << "Waiting for connection..." << std::endl;

    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    std::cout << "Client connected!" << std::endl;

    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Received: " << buffer << "\n";
        }
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
