#define SERVER_IP "192.168.1.12"
#define SERVER_PORT 54000

#include <windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <random>

#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;
bool connected = false;

void sendData(UINT pressure, int x, int y, int tiltX, int tiltY, char eventType) {
    if (!connected) return;

    std::string message = std::to_string(x) + " " + std::to_string(y) + " " +
                          std::to_string(pressure) + " " +
                          std::to_string(tiltX) + " " + std::to_string(tiltY) + " " + eventType + "\n";

    send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
    std::cout << message << std::endl;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static UINT pressure = 0;
    static int x = 0, y = 0, tiltX = 0, tiltY = 0;
    
    switch (message) {
    case WM_POINTERDOWN:
    case WM_POINTERUPDATE:
    case WM_POINTERUP: {
        POINTER_PEN_INFO ppi;
        ZeroMemory(&ppi, sizeof(POINTER_PEN_INFO));
        if (GetPointerPenInfo(GET_POINTERID_WPARAM(wParam), &ppi)) {
            pressure = ppi.pressure;
            x = ppi.pointerInfo.ptPixelLocation.x;
            y = ppi.pointerInfo.ptPixelLocation.y;
            tiltX = ppi.tiltX;
            tiltY = ppi.tiltY;

            char eventType = (message == WM_POINTERDOWN) ? 'D' :
                             (message == WM_POINTERUP) ? 'U' : 'M';

            sendData(pressure, x, y, tiltX, tiltY, eventType);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverHint;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverHint.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverHint, sizeof(serverHint)) != SOCKET_ERROR) {    
        int flag = 1;
        setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
        connected = true;
        std::cout << "Connected to server!" << std::endl;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"PenPressureClass";
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(L"PenPressureClass", L"Pen Data Sender", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 500, 500, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
