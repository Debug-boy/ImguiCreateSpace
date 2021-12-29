#include "M_IMGUI.h"

ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
WNDCLASSEX g_wndclassEx;
HWND g_mImguiDrawWindow = NULL;
M_RECT<LONG>g_mWindowRect = { 0,0,1600,800 };
HWND g_bindWndObject = NULL;

// Helper functions

HWND InitCreateDrawWindow(HWND bindWndObject)
{
    ZeroMemory(&g_wndclassEx, sizeof(WNDCLASSEX));
    g_wndclassEx = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Infinity 3DSpace demo"), NULL };
    ::RegisterClassEx(&g_wndclassEx);
    g_mImguiDrawWindow = ::CreateWindowEx(WS_EX_TRANSPARENT, g_wndclassEx.lpszClassName, _T("Infinity 3DSpace"), WS_OVERLAPPEDWINDOW, g_mWindowRect.x, g_mWindowRect.y, g_mWindowRect.w, g_mWindowRect.h, NULL, NULL, g_wndclassEx.hInstance, NULL);
    //::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX12 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    if (!InitCreateDeviceD3D())
        return NULL;

    if (!InitImguiDrawContext())
        return NULL;

    if (bindWndObject) {
        DWM_BLURBEHIND dwm_blurbehind = { DWM_BB_ENABLE | DWM_BB_BLURREGION , true , CreateRectRgn(0 , 0 , -1 , -1), true };
        ::SetLayeredWindowAttributes(g_mImguiDrawWindow, NULL, NULL, NULL);
        ::DwmEnableBlurBehindWindow(g_mImguiDrawWindow, &dwm_blurbehind);

        g_bindWndObject = bindWndObject;
        MoveDxWindow(g_bindWndObject);
    }

    return g_mImguiDrawWindow;
}

bool InitCreateDeviceD3D()
{

    if (g_mImguiDrawWindow == NULL)
        return false;

    if (!CreateDeviceD3D(g_mImguiDrawWindow))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(g_wndclassEx.lpszClassName, g_wndclassEx.hInstance);
        return false;
    }

    ::ShowWindow(g_mImguiDrawWindow, SW_SHOWDEFAULT);
    ::UpdateWindow(g_mImguiDrawWindow);

    return true;
}

bool InitImguiDrawContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    bool b1 = ImGui_ImplWin32_Init(g_mImguiDrawWindow);
    bool b2 = ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    return b1 && b2;
}

void MoveDxWindow(HWND bindGameHwnd)
{
    if (g_mImguiDrawWindow)
    {
        BOOL getState = GetWindowClientAreaData(bindGameHwnd,
            g_mWindowRect.x, g_mWindowRect.y,
            g_mWindowRect.w, g_mWindowRect.h);

        if (getState)
            MoveWindow(g_mImguiDrawWindow,
                g_mWindowRect.x, g_mWindowRect.y,
                g_mWindowRect.w, g_mWindowRect.h, false);
    }
}

DWORD WINAPI ThreadMove(LPVOID lpThreadParameter)
{
    while (lpThreadParameter)
    {
        MoveDxWindow(g_bindWndObject);
        Sleep(3000);
    }
    return 0;
}


void SetRenderCallback(VOID(*m_Render)())
{
    HANDLE threadMove = CreateThread(NULL, 0, ThreadMove, (LPVOID)g_bindWndObject, 0, NULL);

    while (true) {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
             
            if (msg.message == WM_QUIT) {
                break;
            }

            DispatchMessage(&msg);
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        m_Render();

        // Rendering
        ImGui::Render();
        const static float clear_color_with_alpha[4] = { 0,0,0,0 };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }
    DestoryImgui();
}

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void DestoryImgui()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(g_mImguiDrawWindow);
    ::UnregisterClass(g_wndclassEx.lpszClassName, g_wndclassEx.hInstance);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
