#include <windows.h>

LRESULT CALLBACK
MainWindowCallback(HWND window,
                   UINT message,
                   WPARAM wParam,
                   LPARAM lParam)
{
    LRESULT result = 0;
    switch(message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
            break;
        }
        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
            break;
        }
        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
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

            PatBlt(deviceContext, paint.rcPaint.left, paint.rcPaint.top, width, height, WHITENESS);

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
    WindowClass.lpfnWndProc = MainWindowCallback;
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
            for(;;)
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

