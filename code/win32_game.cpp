#include <windows.h>

#define internal static 
#define local_persist static 
#define global_variable static 

global_variable bool running;

internal void
Win32ResizeDIBSection(int width, int height) 
{
    CreateDIBSection();
}

internal void 
Win32UpdateWindow(HDC dc, int x, int y, int width, int height )
{
    StretchDIBits(dc, 
        x, y, width, height,
        x, y, width, height);
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
            int height = clientRect.bottom - clientRect.top;
            int width = clientRect.right - clientRect.left;

            Win32ResizeDIBSection(width, height);
            OutputDebugStringA("WM_SIZE\n");
            break;
        }
        case WM_DESTROY:
        {
            running = false;
            break;
        }
        case WM_CLOSE:
        {
            running = false;
            break;
        }
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;

            Win32UpdateWindow(window, paint.rcPaint.left, paint.rcPaint.top, width, height);

            EndPaint(window, &paint);

        }
        
        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
            break;
        }
    }
    return result;
}

INT CALLBACK
WinMain(HINSTANCE instance, 
        HINSTANCE prevInstance,
        PSTR commandLine, 
        INT showCode)
{
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = "GameWindowClass";

    if(RegisterClass(&WindowClass))
    {
        HWND windowHandle = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Game Window",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            instance,
            0
        );

        if(windowHandle) 
        {
            MSG message;
            running = true;
            while(running)
            {

                BOOL messageResult = GetMessage(&message, 0, 0, 0);
                if(messageResult > 0) 
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                else
                {
                    break;
                }
            }
        }
        else 
        {
            //TODO - Logging window handle failed
        }
    }
    else
    {
        //TODO Logging - Window Registration Failed
    }
    return 0;
}

