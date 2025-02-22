/*
win32window.c - Win32 VM Window
Copyright (C) 2021  LekKit <github.com/LekKit>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "fb_window.h"
#include "ps2-mouse.h"
#include "ps2-keyboard.h"

#include <windows.h>

#define KEYMAP_PAUSE VK_PAUSE
#define KEYMAP_PRINT VK_PRINT

#include "keymap.h"

typedef struct {
    HWND hwnd;
    POINTS lastcur;
    struct mouse_btns btns;
    int x;
	int y;
    char name[256];
} win32fb_data;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CLOSE) {
        DestroyWindow(hwnd);
        exit(0);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static ATOM winclass_atom = 0;

static void init_keycodes()
{
    init_keymap();
    
    init_keycode(0x41, 0x1C, 1);
    init_keycode(0x42, 0x32, 1);
    init_keycode(0x43, 0x21, 1);
    init_keycode(0x44, 0x23, 1);
    init_keycode(0x45, 0x24, 1);
    init_keycode(0x46, 0x2B, 1);
    init_keycode(0x47, 0x34, 1);
    init_keycode(0x48, 0x33, 1);
    init_keycode(0x49, 0x43, 1);
    init_keycode(0x4A, 0x3B, 1);
    init_keycode(0x4B, 0x42, 1);
    init_keycode(0x4C, 0x4B, 1);
    init_keycode(0x4D, 0x3A, 1);
    init_keycode(0x4E, 0x31, 1);
    init_keycode(0x4F, 0x44, 1);
    init_keycode(0x50, 0x4D, 1);
    init_keycode(0x51, 0x15, 1);
    init_keycode(0x52, 0x2D, 1);
    init_keycode(0x53, 0x1B, 1);
    init_keycode(0x54, 0x2C, 1);
    init_keycode(0x55, 0x3C, 1);
    init_keycode(0x56, 0x2A, 1);
    init_keycode(0x57, 0x1D, 1);
    init_keycode(0x58, 0x22, 1);
    init_keycode(0x59, 0x35, 1);
    init_keycode(0x5A, 0x1A, 1);
    init_keycode(0x30, 0x45, 1);
    init_keycode(0x31, 0x16, 1);
    init_keycode(0x32, 0x1E, 1);
    init_keycode(0x33, 0x26, 1);
    init_keycode(0x34, 0x25, 1);
    init_keycode(0x35, 0x2E, 1);
    init_keycode(0x36, 0x36, 1);
    init_keycode(0x37, 0x3D, 1);
    init_keycode(0x38, 0x3E, 1);
    init_keycode(0x39, 0x46, 1);
    init_keycode(VK_OEM_3, 0x0E, 1);
    init_keycode(VK_OEM_MINUS, 0x4E, 1);
    init_keycode(VK_OEM_PLUS, 0x55, 1);
    init_keycode(VK_OEM_5, 0x5D, 1);
    init_keycode(VK_BACK, 0x66, 1);
    init_keycode(VK_SPACE, 0x29, 1);
    init_keycode(VK_TAB, 0x0D, 1);
    init_keycode(VK_CAPITAL, 0x58, 1);
    init_keycode(VK_SHIFT, 0x12, 1);
    init_keycode(VK_CONTROL, 0x14, 1);
    init_keycode(VK_LWIN, 0xE01F, 2);
    init_keycode(VK_MENU, 0x11, 1);
    init_keycode(VK_RSHIFT, 0x59, 1);
    init_keycode(VK_RCONTROL, 0x14E0, 2);
    init_keycode(VK_RWIN, 0x27E0, 2);
    init_keycode(VK_RMENU, 0x11E0, 2);
    init_keycode(VK_APPS, 0x2FE0, 2); // APPS)
    init_keycode(VK_RETURN, 0x5A, 1);
    init_keycode(VK_ESCAPE, 0x76, 1);
    init_keycode(VK_F1, 0x05, 1);
    init_keycode(VK_F2, 0x06, 1);
    init_keycode(VK_F3, 0x04, 1);
    init_keycode(VK_F4, 0x0C, 1);
    init_keycode(VK_F5, 0x03, 1);
    init_keycode(VK_F6, 0x0B, 1);
    init_keycode(VK_F7, 0x83, 1);
    init_keycode(VK_F8, 0x0A, 1);
    init_keycode(VK_F9, 0x01, 1);
    init_keycode(VK_F10, 0x09, 1);
    init_keycode(VK_F11, 0x78, 1);
    init_keycode(VK_F12, 0x07, 1);
    /* VK_PRINT is too big, handled separately */
    init_keycode(VK_SCROLL, 0x7E, 1);
    /* VK_PAUSE is too big, handled separately */
    init_keycode(VK_OEM_4, 0x54, 1);
    init_keycode(VK_INSERT, 0x70E0, 2);
    init_keycode(VK_HOME, 0x6CE0, 2);
    init_keycode(VK_PRIOR, 0x7DE0, 2);
    init_keycode(VK_DELETE, 0x71E0, 2);
    init_keycode(VK_END, 0x69E0, 2);
    init_keycode(VK_NEXT, 0x7AE0, 2);
    init_keycode(VK_UP, 0x75E0, 2);
    init_keycode(VK_LEFT, 0x6BE0, 2);
    init_keycode(VK_DOWN, 0x72E0, 2);
    init_keycode(VK_RIGHT, 0x74E0, 2);
    init_keycode(VK_NUMLOCK, 0x77, 1);
    init_keycode(VK_DIVIDE, 0x4AE0, 2);
    init_keycode(VK_MULTIPLY, 0x7C, 1);
    init_keycode(VK_SUBTRACT, 0x7B, 1);
    init_keycode(VK_ADD, 0x79, 1);
    // need to handle disabled numlock somehow...
    //init_keycode(VK_RETURN, 0x5AE0, 2);
    init_keycode(VK_DECIMAL, 0x71, 1); //init_keycode(XK_KP_Delete, 0x71, 1);
    init_keycode(VK_NUMPAD0, 0x70, 1); //init_keycode(XK_KP_Insert, 0x70, 1);
    init_keycode(VK_NUMPAD1, 0x69, 1); //init_keycode(XK_KP_End, 0x69, 1);
    init_keycode(VK_NUMPAD2, 0x72, 1); //init_keycode(XK_KP_Down, 0x72, 1);
    init_keycode(VK_NUMPAD3, 0x7A, 1); //init_keycode(XK_KP_Page_Down, 0x7A, 1);
    init_keycode(VK_NUMPAD4, 0x6B, 1); //init_keycode(XK_KP_Left, 0x6B, 1);
    init_keycode(VK_NUMPAD5, 0x73, 1); //init_keycode(XK_KP_Begin, 0x73, 1);
    init_keycode(VK_NUMPAD6, 0x74, 1); //init_keycode(XK_KP_Right, 0x74, 1);
    init_keycode(VK_NUMPAD7, 0x6C, 1); //init_keycode(XK_KP_Home, 0x6C, 1);
    init_keycode(VK_NUMPAD8, 0x75, 1); //init_keycode(XK_KP_Up, 0x75, 1);
    init_keycode(VK_NUMPAD9, 0x7D, 1); //init_keycode(XK_KP_Page_Up, 0x7D, 1);
    init_keycode(VK_OEM_6, 0x5B, 1);
    init_keycode(VK_OEM_1, 0x4C, 1);
    init_keycode(VK_OEM_7, 0x52, 1);
    init_keycode(VK_OEM_COMMA, 0x41, 1);
    init_keycode(VK_OEM_PERIOD, 0x49, 1);
    init_keycode(VK_OEM_2, 0x4A, 1);
}

void fb_create_window(struct fb_data *data, unsigned width, unsigned height, const char* name)
{
    win32fb_data* wdata = safe_calloc(1, sizeof(win32fb_data));
	data->winsys_data = (void*)wdata;
    data->framebuffer = safe_calloc((size_t)width * height, 4);
    
    wdata->hwnd = NULL;
    wdata->x = width;
    wdata->y = height;
    strncpy(wdata->name, name, sizeof(wdata->name) - 1);
    wdata->name[sizeof(wdata->name) - 1] = 0;
    
    if (winclass_atom == 0) {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc   = WindowProc;
        wc.hInstance     = GetModuleHandle(NULL);
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = "RVVM_window";
        winclass_atom = RegisterClass(&wc);
        if (winclass_atom == 0) {
            MessageBox(NULL, "Failed to register window class!", "RVVM Error", MB_OK | MB_ICONERROR);
            return;
        }
        
        init_keycodes();
    }
    /* 
     * It's not possible to work with window from thread other than the window creator in win32,
     * so the hacky workaround is to move window creation to fb_update...
     */
}

void fb_close_window(struct fb_data *data)
{
    if (winclass_atom == 0) return;
    win32fb_data* wdata = (win32fb_data*)data->winsys_data;
    DestroyWindow(wdata->hwnd);
    free(wdata);
}

void fb_update(struct fb_data *data)
{
    if (winclass_atom == 0) return;
    win32fb_data* wdata = (win32fb_data*)data->winsys_data;
    
    if (wdata->hwnd == NULL) {
        RECT rect;
        rect.top = 0;
        rect.left = 0;
        rect.right = wdata->x;
        rect.bottom = wdata->y;
        AdjustWindowRect(&rect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, 0);
        
        wdata->hwnd = CreateWindow(
            "RVVM_window",
            wdata->name,
            WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, GetModuleHandle(NULL), NULL);
    }

    InvalidateRect(wdata->hwnd, NULL, 1);
    MSG Msg;
    while (PeekMessage(&Msg, wdata->hwnd, 0, 0, PM_REMOVE)) {
        switch (Msg.message) {
            case WM_MOUSEMOVE: {
                POINTS cur = MAKEPOINTS(Msg.lParam);
                ps2_handle_mouse(data->mouse, cur.x - wdata->lastcur.x, wdata->lastcur.y - cur.y, &wdata->btns);
                wdata->lastcur = cur;
                break;
            }
            case WM_KEYDOWN:
                // Disable autorepeat keypresses
                if ((Msg.lParam & KF_REPEAT) == 0) {
                    struct key k = keysym2makecode(Msg.wParam);
                    if (k.len) ps2_handle_keyboard(data->keyboard, &k, true);
                }
                break;
            case WM_KEYUP: {
                    struct key k = keysym2makecode(Msg.wParam);
                    if (k.len) ps2_handle_keyboard(data->keyboard, &k, false);
                }
                break;
            case WM_LBUTTONDOWN:
                wdata->btns.left = true;
                ps2_handle_mouse(data->mouse, 0, 0, &wdata->btns);
                break;
            case WM_LBUTTONUP:
                wdata->btns.left = false;
                ps2_handle_mouse(data->mouse, 0, 0, &wdata->btns);
                break;
            case WM_RBUTTONDOWN:
                wdata->btns.right = true;
                ps2_handle_mouse(data->mouse, 0, 0, &wdata->btns);
                break;
            case WM_RBUTTONUP:
                wdata->btns.right = false;
                ps2_handle_mouse(data->mouse, 0, 0, &wdata->btns);
                break;
            case WM_PAINT: {
                PAINTSTRUCT ps;
                RECT rect;
                HBITMAP hBitmap = CreateBitmap(wdata->x, wdata->y, 1, 32, data->framebuffer);
                HDC hdc = GetDC(wdata->hwnd);
                HDC hdcc = CreateCompatibleDC(hdc);
                ReleaseDC(wdata->hwnd, hdc);
                SelectObject(hdcc, hBitmap);
                hdc = BeginPaint(wdata->hwnd, &ps);
                GetClientRect(wdata->hwnd, &rect);
                SetStretchBltMode(hdc, STRETCH_HALFTONE);
                StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hdcc, 0, 0, wdata->x, wdata->y, SRCCOPY);
                
                EndPaint(wdata->hwnd, &ps);
                
                DeleteObject(hBitmap);
                DeleteDC(hdcc);
                break;
            }
            default:
                DispatchMessage(&Msg);
                break;
        }
    }
}
