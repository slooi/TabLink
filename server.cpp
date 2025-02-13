#define SERVER_IP "192.168.1.12"  // Change this to your desired IP
#define SERVER_PORT 54000         // Change this to your desired port

#include <iostream>
#include <WS2tcpip.h>

// #pragma comment(lib, "ws2_32.lib") //  

int main() {
    // Initialize Winsock
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket\n";
        WSACleanup();
        return 1;
    }

    // Bind the socket to an IP/Port
    sockaddr_in serverHint;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(SERVER_PORT);
    serverHint.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {
        std::cerr << "Can't bind socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for a connection
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for a connection...\n";

    // Accept a connection
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Client connection failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected!\n";

    // Receive messages
    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';  // Null-terminate the received data
            std::cout << "Received: " << buffer << "\n";
        }
        else if (bytesReceived == 0) {
            std::cout << "Client disconnected gracefully.\n";
            //break;  // Properly exit the loop
        }
        else {
            int error = WSAGetLastError();
            if (error == WSAECONNRESET) {
                std::cerr << "Connection reset by client (WSAECONNRESET).\n";
            } else {
                std::cerr << "Receive failed. Error code: " << error << "\n";
            }
            //break;  // Properly exit the loop on error
        }
    }

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
