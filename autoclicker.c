#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#define ID_TIMER 1
#define ID_BUTTON_START 1001
#define ID_BUTTON_STOP 1002
#define ID_EDIT_INTERVAL 1003
#define HOTKEY_ID 1

HINSTANCE hInst;
HWND hwndMain, hwndButtonStart, hwndButtonStop, hwndEditInterval;
int isRunning = 0;
int clickInterval = 100;

void DoMouseClick()
{
    INPUT input[2] = {0};
    
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    SendInput(2, input, sizeof(INPUT));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            CreateWindowW(L"Static", L"Interval (ms):", WS_VISIBLE | WS_CHILD,
                           10, 10, 100, 20, hwnd, NULL, hInst, NULL);
            
            hwndEditInterval = CreateWindowW(L"Edit", L"100", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                            120, 10, 80, 20, hwnd, (HMENU)ID_EDIT_INTERVAL, hInst, NULL);
            
            hwndButtonStart = CreateWindowW(L"Button", L"Start (F8)", WS_VISIBLE | WS_CHILD,
                                           10, 40, 100, 30, hwnd, (HMENU)ID_BUTTON_START, hInst, NULL);
            
            hwndButtonStop = CreateWindowW(L"Button", L"Stop (F8)", WS_VISIBLE | WS_CHILD,
                                          120, 40, 100, 30, hwnd, (HMENU)ID_BUTTON_STOP, hInst, NULL);
            EnableWindow(hwndButtonStop, FALSE);
            
            // Register F8 hotkey
            if(!RegisterHotKey(hwnd, HOTKEY_ID, 0, VK_F8))
            {
                MessageBoxW(hwnd, L"Failed to register F8 as hotkey", L"Error", MB_OK | MB_ICONERROR);
            }
            
            break;
        }
        
        case WM_COMMAND:
        {
            if(LOWORD(wParam) == ID_BUTTON_START)
            {
                wchar_t intervalText[20];
                GetWindowTextW(hwndEditInterval, intervalText, 20);
                clickInterval = _wtoi(intervalText);
                
                if(clickInterval < 10) clickInterval = 10; // Minimum interval
                
                SetTimer(hwnd, ID_TIMER, clickInterval, NULL);
                isRunning = 1;
                
                EnableWindow(hwndButtonStart, FALSE);
                EnableWindow(hwndButtonStop, TRUE);
                SetFocus(hwnd);
            }
            else if(LOWORD(wParam) == ID_BUTTON_STOP)
            {
                KillTimer(hwnd, ID_TIMER);
                isRunning = 0;
                
                EnableWindow(hwndButtonStart, TRUE);
                EnableWindow(hwndButtonStop, FALSE);
            }
            break;
        }
        
        case WM_TIMER:
        {
            if(wParam == ID_TIMER)
            {
                DoMouseClick();
            }
            break;
        }
        
        case WM_HOTKEY:
        {
            if(wParam == HOTKEY_ID)
            {
                if(isRunning)
                {
                    SendMessageW(hwnd, WM_COMMAND, ID_BUTTON_STOP, 0);
                }
                else
                {
                    SendMessageW(hwnd, WM_COMMAND, ID_BUTTON_START, 0);
                }
            }
            break;
        }
        
        case WM_CLOSE:
        {
            if(isRunning) KillTimer(hwnd, ID_TIMER);
            UnregisterHotKey(hwnd, HOTKEY_ID);
            DestroyWindow(hwnd);
            break;
        }
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;
    
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = L"AutoClickerClass";
    
    if(!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Window class registration failed!", L"Error", MB_ICONERROR);
        return 0;
    }
    
    hwndMain = CreateWindowExW(0, L"AutoClickerClass", L"Auto Clicker", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 250, 150, NULL, NULL, hInstance, NULL);
    
    if(!hwndMain)
    {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
        return 0;
    }
    
    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);
    
    MSG msg;
    while(GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    return (int)msg.wParam;
}