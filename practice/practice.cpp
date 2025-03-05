#include <windows.h>

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam,
                                    LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_SIZE:
        {
        }
        break;
        case WM_ACTIVATEAPP:
        {
        }
        break;
        case WM_DESTROY:
        {
        }
        break;
        case WM_CLOSE:
        {
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int W = Paint.rcPaint.right - Paint.rcPaint.left;
            int H = Paint.rcPaint.bottom - Paint.rcPaint.top;
            static DWORD Operation = WHITENESS;
            PatBlt(DeviceContext, X, Y, W, H, Operation);
            if (Operation == WHITENESS)
            {
                Operation = BLACKNESS;
            }
            else
            {
                Operation = WHITENESS;
            }
            EndPaint(Window, &Paint);
        }
        break;
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        }
    }
    return Result;
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
                     LPSTR CommandLine, int ShowCode)
{
    WNDCLASS WindowClass = {};
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.lpszClassName = "HandmadeHero";
    WindowClass.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
    WindowClass.hInstance = Instance;

    if (RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(
            0, WindowClass.lpszClassName, "Handmade Hero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

        if (WindowHandle)
        {
            for (;;)
            {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            // Logging
        }
    }
    else
    {
        // Logging
    }
}
