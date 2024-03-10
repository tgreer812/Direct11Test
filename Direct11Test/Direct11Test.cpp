#include "pch.h"
#include <windows.h>
#include <d3d11.h>
#include "Logging.h"
#include "TriangularPyramid.h"
#include <vector>

#pragma comment (lib, "d3d11.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")


// Global Declarations - Others
IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
ID3D11Device* dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView* backbuffer;    // the pointer to our back buffer

// Function Prototypes
void InitD3D(HWND hWnd);    // sets up and initializes Direct3D
void CleanD3D(void);        // closes Direct3D and releases memory
void RenderFrame(void);     // renders a single frame

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CompileShaders();

// the entry point for any Windows program
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(0,
        L"WindowClass",
        L"Our First Direct3D Program",
        WS_OVERLAPPEDWINDOW,
        300, 300,
        640, 480,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);

    // Compile Shaders
    CompileShaders();

    // set up and initialize Direct3D
    InitD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }

        RenderFrame();
    }

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}

// Assume pVS and pPS are already declared as global variables or class members
ID3D11VertexShader* pVS = nullptr;
ID3D11PixelShader* pPS = nullptr;
ID3DBlob* pVSBlob = nullptr; // Vertex shader bytecode blob
ID3DBlob* pPSBlob = nullptr; // Pixel shader bytecode blob, if needed

void CompileShaders()
{
    ID3DBlob* errorBlob = nullptr;

    // Compile the vertex shader
    HRESULT hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VShader", "vs_4_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &errorBlob); // Use pVSBlob directly
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        // Handle the error, e.g., log or display an error message
        return;
    }

    hr = dev->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVS);
    if (FAILED(hr))
    {
        pVSBlob->Release(); // Release only if the shader creation fails
        pVSBlob = nullptr; // Ensure the pointer is null after releasing
        // Handle the error
        return;
    }

    // Compile the pixel shader
    hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PShader", "ps_4_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &errorBlob); // Use pPSBlob directly
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        if (pVSBlob) // Only release pVSBlob if pixel shader compilation fails
        {
            pVSBlob->Release();
            pVSBlob = nullptr;
        }
        // Handle the error
        return;
    }

    hr = dev->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPS);
    if (FAILED(hr))
    {
        pPSBlob->Release(); // Release only if the shader creation fails
        pPSBlob = nullptr; // Ensure the pointer is null after releasing
        if (pVSBlob) // Additionally, clean up pVSBlob if not done already
        {
            pVSBlob->Release();
            pVSBlob = nullptr;
        }
        // Handle the error
        return;
    }

    // No need to set shader objects here if you plan to do it later, 
    // but it's okay to leave it if you want to set them right away
    //devcon->VSSetShader(pVS, nullptr, 0);
    //devcon->PSSetShader(pPS, nullptr, 0);
}
// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    HRESULT result;
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &dev,
        NULL,
        &devcon);

    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(result)) {
        LOGERROR("Failed to get back buffer from swap chain.");
        exit(1);
    }

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;

    devcon->RSSetViewports(1, &viewport);

    // Create a triangular pyramid
    TriangularPyramid pyramid = TriangularPyramid();

    std::vector<Vertex> vertices = pyramid.vertices;

    // Create the vertex buffer
    ID3D11Buffer* vertexBuffer;

    // Create the buffer description
    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * vertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    // Create the subresource data
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &vertices[0];
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    dev->CreateBuffer(&bd, &initData, &vertexBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Create and bind the index buffer
    ID3D11Buffer* indexBuffer = nullptr;
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(uint16_t) * pyramid.indices.size(); // Size of the index buffer
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iInitData = {};
    iInitData.pSysMem = pyramid.indices.data();

    dev->CreateBuffer(&ibd, &iInitData, &indexBuffer);

    // Bind the index buffer
    devcon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position),
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color),
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layout);

    ID3D11InputLayout* pVertexLayout;
    dev->CreateInputLayout(
        layout, 
        numElements, 
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), 
        &pVertexLayout
    );

    devcon->IASetInputLayout(pVertexLayout);


}

// this is the function used to render a single frame
void RenderFrame(void)
{
    // Define clear color as an XMVECTORF32 (r, g, b, a)
    DirectX::XMVECTORF32 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Clear the back buffer to the defined color
    devcon->ClearRenderTargetView(backbuffer, clearColor);

    // Additional rendering commands would go here

    // Present the back buffer to the front buffer (the screen)
    swapchain->Present(0, 0);
}

// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
    swapchain->Release();
    dev->Release();
    devcon->Release();
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
