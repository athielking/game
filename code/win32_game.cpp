#include <windows.h>
#include <stdint.h>

#define internal static 
#define local_persist static 
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int xOffset, int yOffset)
{    
    int pitch = BitmapWidth*BytesPerPixel;
    uint8 *row = (uint8 *)BitmapMemory;    
    for(int y = 0; y < BitmapHeight; ++y)
    {
        uint32 *pixel = (uint32 *)row;
        for(int x = 0; x < BitmapWidth; ++x)
        {
            uint8 blue = (x + xOffset);
            uint8 green = (y + yOffset);
            
            *pixel++ = ((green << 8) | blue);
        }

        row += pitch;
    }
}

internal void
Win32ResizeDIBSection(int width, int height)
{
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = width;
    BitmapHeight = height;
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC deviceContext, RECT *clientRect, int x, int y, int width, int height)
{    
    int windowWidth = clientRect->right - clientRect->left;
    int windowHeight = clientRect->bottom - clientRect->top;
    StretchDIBits(deviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  */
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, windowWidth, windowHeight,
                  BitmapMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND window,
                        UINT message,
                        WPARAM wParam,
                        LPARAM lParam)
{
    LRESULT result = 0;

    switch(message)
    {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            Win32ResizeDIBSection(width, height);
        } break;

        case WM_CLOSE:
        {
            // TODO(casey): Handle this with a message to the user?
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            // TODO(casey): Handle this as an error - recreate window?
            Running = false;
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;

            RECT clientRect;
            GetClientRect(window, &clientRect);

            Win32UpdateWindow(deviceContext, &clientRect, x, y, width, height);
            EndPaint(window, &paint);
        } break;

        default:
        {
//            OutputDebugStringA("default\n");
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }
    
    return (result);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    WNDCLASS windowClass = {};
    
    // TODO(casey): Check if HREDRAW/VREDRAW/OWNDC still matter
    windowClass.lpfnWndProc = Win32MainWindowCallback;
    windowClass.hInstance = Instance;
//    WindowClass.hIcon;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClassA(&windowClass))
    {
        HWND window =
            CreateWindowExA(
                0,
                windowClass.lpszClassName,
                "Handmade Hero",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
        if(window)
        {
            int xOffset = 0;
            int yOffset = 0;

            Running = true;
            while(Running)
            {
                MSG message;
                while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                {
                    if(message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }

                RenderWeirdGradient(xOffset, yOffset);

                HDC deviceContext = GetDC(window);
                RECT clientRect;
                GetClientRect(window, &clientRect);
                int WindowWidth = clientRect.right - clientRect.left;
                int WindowHeight = clientRect.bottom - clientRect.top;
                Win32UpdateWindow(deviceContext, &clientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(window, deviceContext);
                
                ++xOffset;
                //YOffset += 2;
            }
        }
        else
        {
            // TODO(casey): Logging
        }
    }
    else
    {
        // TODO(casey): Logging
    }
    
    return(0);
}