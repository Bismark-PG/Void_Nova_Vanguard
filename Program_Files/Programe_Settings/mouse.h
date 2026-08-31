/*==============================================================================

    Mouse Input Manager [mouse.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef MOUSE_H
#define MOUSE_H
#pragma once

// Mouse Position Mode
typedef enum Mouse_PositionMode_tag
{
	MOUSE_POSITION_MODE_ABSOLUTE, // Absolute coordinate mode
	MOUSE_POSITION_MODE_RELATIVE, // Relative coordinate mode
} Mouse_PositionMode;


// Mouse State Info
typedef struct MouseState_tag
{
    bool leftButton;
    bool middleButton;
    bool rightButton;
    bool xButton1;
    bool xButton2;
    int x;
    int y;
    int scrollWheelValue;
    Mouse_PositionMode positionMode;
} Mouse_State;

// Init
void Mouse_Initialize(HWND window);
void Mouse_Finalize(void);

// Mouse State Getter
void Mouse_GetState(Mouse_State* pState);

// Scroll Wheel Value Reset
void Mouse_ResetScrollWheelValue(void);

// Mouse Position Mode Setter
void Mouse_SetMode(Mouse_PositionMode mode);

// Connection Check
bool Mouse_IsConnected(void);

// Visibility Control
bool Mouse_IsVisible(void);
void Mouse_SetVisible(bool visible);

// Mouse Message Process For Window Message Procedure
void Mouse_ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

// Debug Tools
void Debug_Mode_Switcher();
void Debug_Mode_Set();

// Mouse POS Check And Draw
bool Is_Mouse_In_RECT(float mx, float my, float x, float y, float w, float h);

// How To Use This Mouse Input Manager
//
// If the window is created, call the initialization function with the window handle as an argument.
//
// Mouse_Initialize(hwnd);
//
// Mouse input manager will automatically process mouse input messages from the window message procedure.
//
// LResult CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     switch (message)
//     {
//     case WM_ACTIVATEAPP:
//     case WM_INPUT:
//     case WM_MOUSEMOVE:
//     case WM_LBUTTONDOWN:
//     case WM_LBUTTONUP:
//     case WM_RBUTTONDOWN:
//     case WM_RBUTTONUP:
//     case WM_MBUTTONDOWN:
//     case WM_MBUTTONUP:
//     case WM_MOUSEWHEEL:
//     case WM_XBUTTONDOWN:
//     case WM_XBUTTONUP:
//     case WM_MOUSEHOVER:
//         Mouse_ProcessMessage(message, wParam, lParam);
//         break;
//
//     }
// }
//

#endif // MOUSE_H
