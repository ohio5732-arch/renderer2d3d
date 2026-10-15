#include "src/engine/renderer.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm/ext/matrix_transform.hpp"
#include "glm/glm/ext/scalar_constants.hpp"
#include "glm/glm/ext/vector_float3.hpp"
#include <combaseapi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <iostream>
#include <stdexcept>
#include <winerror.h>
#include <winsock.h>
#include <wrl/client.h>

Vertex quad[] = {
    // Triangle 1
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},  // Top-left (Red)
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},   // Top-right (Green)
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // Bottom-left (Blue)

    // Triangle 2
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // Bottom-left (Blue)
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},   // Top-right (Green)
    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}   // Bottom-right (White)
};

Renderer::Renderer(Window &PassedWindow) : currentWindow(PassedWindow) {}

void Renderer::render() {
  HWND hwnd = currentWindow.getWin32Window();

  DXGI_SWAP_CHAIN_DESC sd{};
  sd.BufferCount = 2;
  sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

  hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                     0, nullptr, 0, D3D11_SDK_VERSION, &sd,
                                     &swapChain, &device, nullptr, &context);
  if (FAILED(hr)) {
    std::cout << "failed to get hresults\n";
    return;
  }

  ComPtr<ID3D11Texture2D> backBuffer;
  hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
  if (FAILED(hr)) {
    throw std::runtime_error("failed to create back buffer\n");
  }
  ComPtr<ID3D11RenderTargetView> rtv;
  hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &rtv);
  if (FAILED(hr)) {
    throw std::runtime_error("failed to create rtv\n");
  }

  CompileShaders(L"resources/vertex.hlsl", L"resources/pixel.hlsl");

  ComPtr<ID3D11Buffer> vertexBuffer = createVertexBuffer(quad, sizeof(quad));

  D3D11_INPUT_ELEMENT_DESC layout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  ComPtr<ID3D11InputLayout> inputLayout;
  hr = device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(),
                                 vsBlob->GetBufferSize(), &inputLayout);
  if (FAILED(hr)) {
    std::cout << "failed to create input layout\n";
  }

  ComPtr<ID3D11Buffer> constantBuffer = createConstantBuffer(sizeof(CBData));

  while (!currentWindow.shouldClose()) {
    float color[] = {0.0f, 0.0f, 0.0f, 0.0f};
    D3D11_VIEWPORT vp{0.0f, 0.0f, 800.0f, 800.0f, 0.0f, 1.0f};

    context->RSSetViewports(1, &vp);
    context->IASetInputLayout(inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
    context->ClearRenderTargetView(rtv.Get(), color);

    UINT stride = sizeof(Vertex), offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride,
                                &offset);

    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);

    D3D11_MAPPED_SUBRESOURCE mapped{};
    hr = context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
                      &mapped);
    if (SUCCEEDED(hr)) {
      CBData data{};
      data.time = (float)glfwGetTime();
      data.transform = glm::mat4(1.0f);
      data.transform =
          glm::rotate(data.transform, data.time, glm::vec3(0.0f, 0.0f, 1.0f));
      data.transform = glm::transpose(data.transform);
      memcpy(mapped.pData, &data, sizeof(CBData));
      context->Unmap(constantBuffer.Get(), 0);
    }

    context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    context->Draw(6, 0);
    swapChain->Present(1, 0);
    glfwPollEvents();
  }
}

ComPtr<ID3D11Buffer> Renderer::createBuffer(UINT byteSize,
                                            const void *initialData,
                                            UINT bindFlags, D3D11_USAGE usage,
                                            UINT cpuAccess) {
  D3D11_BUFFER_DESC bd{};
  bd.ByteWidth = byteSize;
  bd.Usage = usage;
  bd.BindFlags = bindFlags;
  bd.CPUAccessFlags = cpuAccess;

  D3D11_SUBRESOURCE_DATA data{};
  D3D11_SUBRESOURCE_DATA *pInitData = nullptr;
  if (initialData != nullptr) {
    data.pSysMem = initialData;
    pInitData = &data;
  }

  ComPtr<ID3D11Buffer> buffer;
  hr = device->CreateBuffer(&bd, pInitData, &buffer);
  if (FAILED(hr)) {
    std::cout << "failed to create buffer, HRESULT: 0x" << std::hex << hr
              << "\n";
    return nullptr;
  }
  return buffer;
}

ComPtr<ID3D11Buffer> Renderer::createVertexBuffer(Vertex *vertices,
                                                  UINT byteSize) {
  return createBuffer(byteSize, vertices, D3D11_BIND_VERTEX_BUFFER,
                      D3D11_USAGE_IMMUTABLE);
}

ComPtr<ID3D11Buffer> Renderer::createConstantBuffer(UINT byteSize) {
  return createBuffer(byteSize, nullptr, D3D11_BIND_CONSTANT_BUFFER,
                      D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void Renderer::CompileShaders(const wchar_t *vsSrc, const wchar_t *psSrc) {
  hr = D3DCompileFromFile(vsSrc, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                          "VSMain", "vs_5_0", 0, 0, &vsBlob, &errors);
  if (FAILED(hr)) {
    if (errors != nullptr) {
      std::cout << "Shader Compilation Error:\n"
                << (char *)errors->GetBufferPointer() << std::endl;
    } else {
      std::cout << "Failed to compile shader. HRESULT: 0x" << std::hex << hr
                << std::endl;
    }
  }

  errors.Reset();

  hr = D3DCompileFromFile(psSrc, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                          "PSMain", "ps_5_0", 0, 0, &psBlob, &errors);
  if (FAILED(hr)) {
    if (errors != nullptr) {
      std::cout << "Shader Compilation Error:\n"
                << (char *)errors->GetBufferPointer() << std::endl;
    } else {
      std::cout << "Failed to compile shader. HRESULT: 0x" << std::hex << hr
                << std::endl;
    }
  }

  hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
                                  vsBlob->GetBufferSize(), nullptr, &vs);
  if (FAILED(hr)) {
    std::cout << "failed to create vertex shader\n";
  }

  hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
                                 psBlob->GetBufferSize(), nullptr, &ps);
  if (FAILED(hr)) {
    std::cout << "failed to create pixel shdaer\n";
  }
}
