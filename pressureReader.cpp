#include <windows.h>
#include <iostream>
#include <strsafe.h>  // For safer string functions

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static UINT newPressure = 0;
    switch (message) {
    case WM_POINTERUPDATE:
    case WM_POINTERDOWN:
    case WM_POINTERUP: {
        POINTER_PEN_INFO ppi;
        ZeroMemory(&ppi, sizeof(POINTER_PEN_INFO));  // Ensures all fields are initialized to zero
        if (GetPointerPenInfo(GET_POINTERID_WPARAM(wParam), &ppi)) {
            newPressure = ppi.pressure;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }
    case WM_PAINT: {     
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        WCHAR szOutput[256];  // Use WCHAR instead of TCHAR
        wsprintfW(szOutput, L"Pressure: %u", newPressure); // Explicitly use wsprintfW
        TextOutW(hDC, 10, 10, szOutput, lstrlenW(szOutput)); // Use TextOutW for wide strings
        EndPaint(hWnd, &ps);
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
    WNDCLASSW wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW +1);
    wc.lpszClassName = L"PenPressureClass";

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        L"PenPressureClass",
        L"Pen Pressure",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 300, 200,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
