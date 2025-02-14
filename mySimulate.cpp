#include <iostream>
#include <WS2tcpip.h>
#include <Windows.h>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 54000

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

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create synthetic pointer device
    HSYNTHETICPOINTERDEVICE pointer = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_INDIRECT);
    if (!pointer) {
        std::cerr << "Error creating synthetic pointer: " << GetLastError() << std::endl;
        return 1;
    }

    POINTER_TYPE_INFO inputInfo[1] = {};
    inputInfo[0].type = PT_PEN;
    inputInfo[0].penInfo.pointerInfo.pointerType = PT_PEN;
    inputInfo[0].penInfo.pointerInfo.pointerId = 0;
    inputInfo[0].penInfo.pointerInfo.frameId = 0;
    inputInfo[0].penInfo.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN;
    inputInfo[0].penInfo.penMask = PEN_MASK_PRESSURE | PEN_MASK_TILT_X | PEN_MASK_TILT_Y;
    inputInfo[0].penInfo.pointerInfo.dwTime = 0; //!@# required
    inputInfo[0].penInfo.pointerInfo.PerformanceCount = 0; //!@# required
    inputInfo[0].penInfo.tiltX = 0;
    inputInfo[0].penInfo.tiltY = 0;
    inputInfo[0].penInfo.pressure = 1024;

    std::string inputBuffer;
    char buffer[64];
    
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            inputBuffer += buffer; // Append to buffer
    
            // Process full lines
            size_t pos;
            while ((pos = inputBuffer.find('\n')) != std::string::npos) {
                std::string line = inputBuffer.substr(0, pos);
                inputBuffer.erase(0, pos + 1); // Remove processed part
    
                std::istringstream iss(line);
                int x, y, pressure, tiltX, tiltY;
                if (iss >> x >> y >> pressure >> tiltX >> tiltY) {
                    // Process valid input
                    inputInfo[0].penInfo.pointerInfo.ptPixelLocation.x = x;
                    inputInfo[0].penInfo.pointerInfo.ptPixelLocation.y = y;
                    inputInfo[0].penInfo.pressure = pressure;
                    inputInfo[0].penInfo.tiltX = tiltX;
                    inputInfo[0].penInfo.tiltY = tiltY;
    
                    if (!InjectSyntheticPointerInput(pointer, inputInfo, 1)) {
                        std::cerr << "Error injecting input: " << GetLastError() << std::endl;
                    }
                    std::cout << "Received: " << line << "\n";
                } else {
                    std::cerr << "Invalid data format: " << line << std::endl;
                }
            }
        }
    }
    // Cleanup
    DestroySyntheticPointerDevice(pointer);
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
