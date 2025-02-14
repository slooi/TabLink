#define SERVER_IP "192.168.1.12"
#define SERVER_PORT 54000

#include <windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <random>  // For random number generation
#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;
bool connected = false;
void sendData(UINT pressure, int x, int y, int tiltX, int tiltY) {
    if (!connected) return;

    std::string message = std::to_string(x) + " " + std::to_string(y) + " " +
                          std::to_string(pressure) + " " +
                          std::to_string(tiltX) + " " + std::to_string(tiltY) + "\n";

    // Introduce a 5% chance of a 5ms delay
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    // if (dist(gen) < 0.05) {  // 5% chance
    //     std::this_thread::sleep_for(std::chrono::milliseconds(5));
    // }

    send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);

    std::cout << message << std::endl;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static UINT pressure = 0;
    static int x = 0, y = 0, tiltX = 0, tiltY = 0;
    
    switch (message) {
    case WM_POINTERUPDATE:
    case WM_POINTERDOWN:
    case WM_POINTERUP: {
        POINTER_PEN_INFO ppi;
        ZeroMemory(&ppi, sizeof(POINTER_PEN_INFO));
        if (GetPointerPenInfo(GET_POINTERID_WPARAM(wParam), &ppi)) {
            pressure = ppi.pressure;
            x = ppi.pointerInfo.ptPixelLocation.x;
            y = ppi.pointerInfo.ptPixelLocation.y;
            tiltX = ppi.tiltX;
            tiltY = ppi.tiltY;
            sendData(pressure, x, y, tiltX, tiltY);
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
    // Set process priority to real-time
    if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
        std::cerr << "Failed to set real-time priority: " << GetLastError() << std::endl;
    }

    // Set main thread priority to highest real-time level
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
        std::cerr << "Failed to set thread priority: " << GetLastError() << std::endl;
    }

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
