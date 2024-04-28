#include <windows.h>
// #include <wingdi.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

global_variable bool Running = false;
struct win32_offscreen_buffer {
  BITMAPINFO Info;
  void *Memory;
  LPVOID Memoery;
  int Width;
  int Height;
  int BytesPerPixel;
  int Pitch;
};

struct win32_window_dimension {
  int Width;
  int Height;
};

global_variable win32_offscreen_buffer GlobalBackBuffer;

win32_window_dimension GetWindowDimension(HWND);

internal void RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset,
                                  int YOffset);

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width,
                                    int Height);

internal void Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth,
                                         int WindowHeight,
                                         win32_offscreen_buffer Buffer, int X,
                                         int Y, int Width, int Height);

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message,
                                         WPARAM WParam, LPARAM LParam);

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
                     LPSTR CommandLine, int ShowCode) {

  //  RECT WindowRect =
  WNDCLASS WindowsClass = {};
  WindowsClass.style = CS_HREDRAW | CS_VREDRAW;
  WindowsClass.lpfnWndProc = Win32MainWindowCallback;
  WindowsClass.hInstance = Instance;
  WindowsClass.lpszClassName = "HandmadeHeroWindowClass";

  if (RegisterClassA(&WindowsClass)) {
    HWND Window = CreateWindowEx(0, WindowsClass.lpszClassName, "Handmade Hero",
                                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, 0, 0, Instance, 0);

    if (Window) {

      Running = true;
      int XOffset = 0;
      int YOffset = 0;

      while (Running) {
        MSG Message;
        while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)) {
          if (Message.message == WM_QUIT) {
            Running = false;
          }

          TranslateMessage(&Message);
          DispatchMessageA(&Message);
        }
        RenderWeirdGradient(&GlobalBackBuffer, XOffset, YOffset);
        HDC DeviceContext = GetDC(Window);
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int WindowWidth = ClientRect.right - ClientRect.left;
        int WindowHeight = ClientRect.bottom - ClientRect.top;
        win32_window_dimension Dimension = GetWindowDimension(Window);
        Win32DisplayBufferInWindow(DeviceContext, Dimension.Width,
                                   Dimension.Height, GlobalBackBuffer, 0, 0,
                                   WindowWidth, WindowHeight);
        ReleaseDC(Window, DeviceContext);
        ++XOffset;
      }

    } else {
      // TODO: Logging
    }

  } else {
  }
  return (0);
}

win32_window_dimension GetWindowDimension(HWND Window) {
  win32_window_dimension Result;

  RECT ClientRect;
  GetClientRect(Window, &ClientRect);
  Result.Width = ClientRect.right - ClientRect.left;
  Result.Height = ClientRect.bottom - ClientRect.top;
  return Result;
}

internal void RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset,
                                  int YOffset) {
  int Width = Buffer->Width;
  int Height = Buffer->Height;
  uint8 *Row = (uint8 *)Buffer->Memory;

  for (int Y = 0; Y < Buffer->Height; ++Y) {
    uint32 *Pixel = (uint32 *)Row;

    for (int X = 0; X < Buffer->Width; ++X) {
      /*
                            0  1  2  3
         Pixel in memory: RR GG BB xx
      */
      uint8 Blue = (X + XOffset);
      uint8 Green = (Y + YOffset);

      *Pixel++ = ((Green << 8) | Blue);
    }
    Row += Buffer->Pitch;
  }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width,
                                    int Height) {
  if (Buffer->Memory) {
    VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
  }
  Buffer->Width = Width;
  Buffer->Height = Height;
  Buffer->BytesPerPixel = 4;
  Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
  Buffer->Info.bmiHeader.biWidth = Buffer->Width;
  Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
  Buffer->Info.bmiHeader.biPlanes = 1;
  Buffer->Info.bmiHeader.biBitCount = 32;
  Buffer->Info.bmiHeader.biCompression = BI_RGB;

  int BitmapMemorySize =
      (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;
  // LPVOId WINAPI VirtualAlloc(_In_opt_ LPVOId lpAddress, _In_ SIZE_T dwSize,
  // _In_ DWORD flAllocationType, _In_ DWORD flProtect);
  Buffer->Memory =
      VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

  Buffer->Pitch = Width * Buffer->BytesPerPixel;
  // RenderWeirdGradient(Buffer, 0, 0);
}

internal void Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth,
                                         int WindowHeight,
                                         win32_offscreen_buffer Buffer, int X,
                                         int Y, int Width, int Height) {
  // int WindowWidth = ClientRect.right - ClientRect.left;
  // int WindowHeight = ClientRect.bottom - ClientRect.top;
  StretchDIBits(DeviceContext, 0, 0, Buffer.Width, Buffer.Height, 0, 0,
                WindowWidth, WindowHeight, Buffer.Memory, &Buffer.Info,
                DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message,
                                         WPARAM WParam, LPARAM LParam) {

  LRESULT Result = 0;

  switch (Message) {

  case WM_SIZE: {

    win32_window_dimension Dimension = GetWindowDimension(Window);
    Win32ResizeDIBSection(&GlobalBackBuffer, Dimension.Width, Dimension.Height);
  } break;

  case WM_CLOSE: {
    Running = false;
  } break;

  case WM_ACTIVATEAPP: {

    OutputDebugStringA("WM_ACTIVATEAPP\n");

  } break;

  case WM_DESTROY: {

    Running = false;

  } break;

  case WM_PAINT: {

    PAINTSTRUCT Paint;

    HDC DeviceContext = BeginPaint(Window, &Paint);

    int X = Paint.rcPaint.left;
    int Y = Paint.rcPaint.top;
    int Width = Paint.rcPaint.right - Paint.rcPaint.left;
    int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
    win32_window_dimension Dimension = GetWindowDimension(Window);
    Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height,
                               GlobalBackBuffer, X, Y, Width, Height);
    BOOL endPaint = EndPaint(Window, &Paint);

  } break;

  default: {

    Result = DefWindowProc(Window, Message, WParam, LParam);

  } break;
  }
  return Result;
}
