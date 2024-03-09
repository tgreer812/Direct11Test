// Direct11Test.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "Direct11Test.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#define DEBUGSTRING
#include "Logging.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#include <dxgi1_6.h> // Include the appropriate DXGI header
#include <iostream>

IDXGIAdapter* findGraphicsAdapter() {
    IDXGIFactory* dxgiFactory = nullptr;
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

    LOGDEBUGW(L"Enumerating graphics adapters!!");

    if (FAILED(hr)) {
        LOGERRORW(L"Error creating DXGI factory.");
        return nullptr;
    }

    IDXGIAdapter* foundAdapter = nullptr; // Store the RTX adapter
    UINT adapterIndex = 0;
    IDXGIAdapter* adapter = nullptr;

    while (dxgiFactory->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        // Print adapter information
        LOGDEBUGW(L"Adapter " + std::to_wstring(adapterIndex) + L": " + desc.Description);

        // Check if the adapter has "RTX" in its name
        if (std::wstring(desc.Description).find(L"RTX") != std::string::npos) {
            foundAdapter = adapter;  // Store the found RTX adapter, don't release yet
        }

        adapter->Release();
        adapterIndex++;
    }

    dxgiFactory->Release();
    return foundAdapter; // Return the RTX adapter (could be nullptr if not found)
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DIRECT11TEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIRECT11TEST));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECT11TEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DIRECT11TEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// This creates a hardcoded vertex buffer
ID3D11Buffer* CreateVertexBuffer(ID3D11Device &graphicsDevice) {
    ID3D11Buffer* pVertexBuffer;

    // Define the data-type that
    // describes a vertex.
    struct SimpleVertexCombined
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Color;
    };

    // Supply the actual vertex data.
    SimpleVertexCombined verticesCombo[] =
    {
        DirectX::XMFLOAT3(0.0f, 0.5f, 0.5f),
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.5f),
        DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
        DirectX::XMFLOAT3(0.5f, 0.0f, 0.0f),
        DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
        DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f),
    };

    // Fill in a buffer description.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(SimpleVertexCombined) * 3;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = verticesCombo;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    // Create the vertex buffer.
    HRESULT hr = graphicsDevice.CreateBuffer(&bufferDesc, &InitData, &pVertexBuffer);

    if (hr != S_OK) {
        LOGERROR("Failed to create vertex buffer!");
        exit(1);
    }
    return pVertexBuffer;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(
       szWindowClass, 
       szTitle, 
       WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 
       0, 
       CW_USEDEFAULT, 
       0, 
       nullptr, 
       nullptr, 
       hInstance, 
       nullptr
   );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   IDXGIAdapter* myAdapter = findGraphicsAdapter();

   if (myAdapter == nullptr) {
       LOGERROR("Failed to find RTX graphics adapter!");
       exit(1);
   }

   ID3D11Device** ppGraphicsDevice = nullptr;
   ID3D11DeviceContext** ppDeviceContext = nullptr;

   DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
   ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
   // Initialize swapChainDesc with appropriate values
   swapChainDesc.BufferCount = 2;
   swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swapChainDesc.OutputWindow = hWnd;
   swapChainDesc.SampleDesc.Count = 1;
   swapChainDesc.Windowed = TRUE;

   IDXGISwapChain* pSwapChain = nullptr;
   ID3D11Device* pGraphicsDevice = nullptr;
   ID3D11DeviceContext* pDeviceContext = nullptr;

   // init device and swap chain
   HRESULT result = D3D11CreateDeviceAndSwapChain(
       nullptr,
       D3D_DRIVER_TYPE_HARDWARE,
       NULL,
       NULL,                            // You can put flags here to enable debug if necessary: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
       nullptr,                         // pFeatureLevels
       0,                               // FeatureLevels - number of levels in pFeatureLevels (if that is NULL then the default has 6)
       D3D11_SDK_VERSION,
       &swapChainDesc,                  // pSwapChainDesc,
       &pSwapChain,                     // OUT - gives us the address of the swap chain
       &pGraphicsDevice,                // OUT - gives us the address of the graphics device
       nullptr,
       &pDeviceContext
   );

   if (result != S_OK) {
       LOGERROR("Failed to create swap chain!!");
   }

   LOGINFO("Swap chain created successfully!");

   ID3D11Texture2D* pBackBuffer;
   // Get a pointer to the back buffer
   result = pSwapChain->GetBuffer(
       0, 
       __uuidof(ID3D11Texture2D),
       (LPVOID*)&pBackBuffer
   );

   ID3D11RenderTargetView* pRenderTargetView;
   // Create a render-target view
   pGraphicsDevice->CreateRenderTargetView(
       pBackBuffer, 
       NULL,
       &pRenderTargetView
   );

   // Bind the view
   pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

   // Setup the viewport
   D3D11_VIEWPORT vp;
   vp.Width = 640;
   vp.Height = 480;
   vp.MinDepth = 0.0f;
   vp.MaxDepth = 1.0f;
   vp.TopLeftX = 0;
   vp.TopLeftY = 0;
   pDeviceContext->RSSetViewports(1, &vp);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
