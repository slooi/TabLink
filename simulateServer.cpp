#define WIN32_LEAN_AND_MEAN  // Prevents unnecessary headers from being included
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>  // Include AFTER winsock2.h to prevent winsock.h conflicts
#include <iostream>
#include <thread>
#include <cmath>

// Define this before including Windows.h to prevent winsock.h from being included
#define _WINSOCKAPI_

#pragma comment(lib, "Ws2_32.lib")

#define PORT 54000
#define BUFFER_SIZE 1024

struct PenData {
    int x;
    int y;
    int pressure;
    int tiltX;
    int tiltY;
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed" << std::endl;
        closesocket(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    HSYNTHETICPOINTERDEVICE pointer = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_INDIRECT);
    if (!pointer) {
        std::cerr << "Error creating synthetic pointer: " << GetLastError() << std::endl;
        closesocket(serverSocket);
        return 1;
    }

    POINTER_TYPE_INFO inputInfo[1] = {};
    inputInfo[0].type = PT_PEN;
    inputInfo[0].penInfo.pointerInfo.pointerType = PT_PEN;
    inputInfo[0].penInfo.pointerInfo.pointerId = 0;
    inputInfo[0].penInfo.penMask = PEN_MASK_PRESSURE | PEN_MASK_TILT_X | PEN_MASK_TILT_Y;
    inputInfo[0].penInfo.pointerInfo.dwTime = 0;
    inputInfo[0].penInfo.pointerInfo.PerformanceCount = 0;

    while (true) {
        int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error receiving data" << std::endl;
            continue;
        }

        if (bytesReceived == sizeof(PenData)) {
            PenData penData;
            memcpy(&penData, buffer, sizeof(PenData));

            inputInfo[0].penInfo.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
            inputInfo[0].penInfo.pointerInfo.ptPixelLocation.x = penData.x;
            inputInfo[0].penInfo.pointerInfo.ptPixelLocation.y = penData.y;
            inputInfo[0].penInfo.pressure = penData.pressure;
            inputInfo[0].penInfo.tiltX = penData.tiltX;
            inputInfo[0].penInfo.tiltY = penData.tiltY;
            inputInfo[0].penInfo.pointerInfo.dwTime = 0;
            inputInfo[0].penInfo.pointerInfo.PerformanceCount = 0;

            if (!InjectSyntheticPointerInput(pointer, inputInfo, 1)) {
                std::cerr << "Error injecting input: " << GetLastError() << std::endl;
            }
        }
    }

    DestroySyntheticPointerDevice(pointer);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
