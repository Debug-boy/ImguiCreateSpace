#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "M_TYPE_DEF.h"
#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>

#pragma comment(lib,"Dwmapi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")

// Data
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern WNDCLASSEX g_wndclassEx;
extern HWND g_mImguiDrawWindow;
extern M_RECT<LONG>g_mWindowRect;
extern HWND g_bindWndObject;


// Forward declarations of helper functions
HWND InitCreateDrawWindow(HWND bindWndObject=NULL);
bool InitCreateDeviceD3D();
bool InitImguiDrawContext();
void MoveDxWindow(HWND bindGameHwnd);
void SetRenderCallback(VOID(*m_Render)());

DWORD WINAPI ThreadMove(LPVOID lpThreadParameter);

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void DestoryImgui();


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);