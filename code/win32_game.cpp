#include <windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    MessageBoxA(0, "This is a message box", "Message", MB_OK|MB_ICONINFORMATION);
    return 0;
}