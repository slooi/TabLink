#include <iostream>
#include <Windows.h>
#include <thread> // For sleep function
#include <cmath>  // For cos() and sin() functions
// Move the pen gradually to the right
#include <chrono>

int main() {
    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    int targetX = screenWidth * 3 / 4; // 1/4 from the right edge

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
    inputInfo[0].penInfo.pressure = std::round(1000);
    inputInfo[0].penInfo.pointerInfo.ptPixelLocation.x = centerX;
    inputInfo[0].penInfo.pointerInfo.ptPixelLocation.y = centerY;
    inputInfo[0].penInfo.pointerInfo.dwTime = 0;
    inputInfo[0].penInfo.pointerInfo.PerformanceCount = 0;
    

    // Press the pen down at the center
    if (!InjectSyntheticPointerInput(pointer, inputInfo, 1)) {
        std::cerr << "Error injecting input: " << GetLastError() << std::endl;
        DestroySyntheticPointerDevice(pointer);
        return 1;
    }
    std::cout << "Pen down at center\n";

    
    int steps = 50*10; // Number of steps for smooth movement
    int radius = 100;  // Radius of the circle in pixels
    int delay = 1;     // Time per step in milliseconds
    
    // Get start time
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i <= steps; ++i) {
        // Calculate angle in radians
        double angle = 2 * 3.14159 * i / steps;
        
        inputInfo[0].penInfo.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
        
        // Use parametric equations of a circle: x = centerX + r*cos(θ), y = centerY + r*sin(θ)
        inputInfo[0].penInfo.pointerInfo.ptPixelLocation.x = centerX + radius * cos(angle);
        inputInfo[0].penInfo.pointerInfo.ptPixelLocation.y = centerY + radius * sin(angle);
        
        inputInfo[0].penInfo.pointerInfo.dwTime = 0;
        inputInfo[0].penInfo.pointerInfo.PerformanceCount = 0;
        
        if (!InjectSyntheticPointerInput(pointer, inputInfo, 1)) {
            std::cerr << "Error moving pen: " << GetLastError() << std::endl;
            DestroySyntheticPointerDevice(pointer);
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    
    // Get end time and calculate duration
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Time taken to draw circle: " << duration.count() << " milliseconds" << std::endl;

    // Release the pen at the target position
    inputInfo[0].penInfo.pointerInfo.pointerFlags = POINTER_FLAG_UP;
    InjectSyntheticPointerInput(pointer, inputInfo, 1);
    std::cout << "Pen released\n";

    // Clean up
    DestroySyntheticPointerDevice(pointer);
    return 0;
}
