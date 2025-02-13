#define SERVER_IP "127.0.0.1"// "192.168.1.12"  // Change this to your desired IP
#define SERVER_PORT 54000         // Change this to your desired port


#include <iostream>
#define WIN32_WINNT 0x0A00 
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Initialize Winsock
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket\n";
        WSACleanup();
        return 1;
    }

    // Server address
    sockaddr_in serverHint;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverHint.sin_addr); // 127.0.0.1   "192.168.1.12"

    // Connect to the server
    if (connect(clientSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!\n";

    // Loop to send messages every second
    while (true) {
        std::string message = "Hello from client!";
        send(clientSocket, message.c_str(), static_cast<int>(message.size() + 1), 0);
        std::cout << "Sent message" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait 1 second before sending again
    }
    //shutdown(clientSocket, SD_SEND);  // Stop sending data
    // Cleanup (never actually reached unless the program is manually closed)
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
