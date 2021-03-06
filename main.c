#include <windows.h>
#include <stdint.h>

typedef uint32_t u32;

int running = 1;
int client_width = 0;
int client_height = 0;

int player_x = 0;
int player_y = 0;
int tile_size = 25;

void* memory;
BITMAPINFO bitmap_info;

void clear_screen(u32 color)
{
    
    u32 *pixel = (u32 *)memory; 
    
    for(int pixel_number = 0;
        pixel_number < client_width * client_height;
        ++pixel_number)
    {
        *pixel++ = color;
    }
}

void draw_rectangle(int rec_x,
                    int rec_y,
                    int rec_width,
                    int rec_height,
                    u32 color)
{
    u32 *pixel = (u32 *)memory;
    pixel += rec_y * client_width + rec_x;
    
    for(int y = 0;
        y < rec_height;
        ++y)
    {
        for(int x = 0;
            x < rec_width;
            ++x)
        {
            *pixel++ = color;
        }
        
        pixel += client_width - rec_width;
    }
}


LRESULT CALLBACK 
WindowProc(HWND window, 
           UINT message, 
           WPARAM w_param, 
           LPARAM l_param)
{
    LRESULT result;
    switch(message)
    {
        case WM_CLOSE:
        {
            running = 0;
        } break;
        
        case WM_KEYDOWN:
        {
            switch(w_param)
            {
                case VK_RIGHT:
                {
                    player_x += tile_size;
                } break;
                
                case VK_LEFT:
                {
                    player_x -= tile_size;
                } break;
                
                case VK_UP:
                {
                    player_y += tile_size;
                } break;
                
                case VK_DOWN:
                {
                    player_y -= tile_size;
                } break;
            }
        } break;
        
        default:
        {
            result = DefWindowProc(window,
                                   message, 
                                   w_param, 
                                   l_param);
        } break;
    }
    
    return result;
}

int WINAPI 
wWinMain(HINSTANCE instance, 
         HINSTANCE prev_instance, 
         PWSTR cmd_line, 
         int cmd_show)
{
    WNDCLASS window_class = {0};
    
    wchar_t class_name[] = L"GameWindowClass";
    
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    
    RegisterClass(&window_class);
    
    HWND window = CreateWindowEx(0,
                                 class_name,
                                 L"Game",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    RECT rect;
    GetClientRect(window, &rect);
    client_width = rect.right - rect.left;
    client_height = rect.bottom - rect.top;
    
    memory = VirtualAlloc(0,
                          client_width * client_height * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );
    
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = GetDC(window);
    
    while(running)
    {
        MSG message;
        while(PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        
        clear_screen(0x111111);
        
        draw_rectangle(100, 100, 50, 50, 0x222222);
        draw_rectangle(150, 150, 25, 25, 0x333333);
        draw_rectangle(175, 175, 15, 15, 0x444444);
        
        draw_rectangle(player_x, 
                       player_y, 
                       tile_size, 
                       tile_size, 
                       0xff00f7);
        
        StretchDIBits(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      client_width,
                      client_height,
                      memory,
                      &bitmap_info,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
    }
    
    return 0;
}
