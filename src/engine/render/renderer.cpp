#define TINYOBJLOADER_IMPLEMENTATION

#include "src/engine/renderer.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm/ext/matrix_float4x4.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "glm/glm/geometric.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/matrix.hpp"
#include "src/engine/entity.hpp"
#include <combaseapi.h>
#include <cstring>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <iostream>
#include <stdexcept>
#include <winerror.h>
#include <winsock.h>
#include <wrl/client.h>

Vertex quad[24] = {
    // front (z = -0.5): red, normal (0, 0, -1)
    {{-0.5f, 0.5f, -0.5f}, {0, 0, -1}, {1, 0, 0, 1}},
    {{0.5f, 0.5f, -0.5f}, {0, 0, -1}, {1, 0, 0, 1}},
    {{0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 0, 0, 1}},
    {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 0, 0, 1}},

    // back (z = +0.5): green, normal (0, 0, 1)
    {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 1, 0, 1}},
    {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 1, 0, 1}},
    {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 1, 0, 1}},
    {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 1, 0, 1}},

    // left (x = -0.5): blue, normal (-1, 0, 0)
    {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {0, 0, 1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}, {0, 0, 1, 1}},
    {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 0, 1, 1}},
    {{-0.5f, -0.5f, 0.5f}, {-1, 0, 0}, {0, 0, 1, 1}},

    // right (x = +0.5): yellow, normal (1, 0, 0)
    {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0, 1}},
    {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {1, 1, 0, 1}},
    {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {1, 1, 0, 1}},
    {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0, 1}},

    // top (y = +0.5): cyan, normal (0, 1, 0)
    {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 1, 1, 1}},
    {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 1, 1, 1}},
    {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1, 1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1, 1, 1}},

    // bottom (y = -0.5): magenta, normal (0, -1, 0)
    {{0.5f, -0.5f, 0.5f}, {0, -1, 0}, {1, 0, 1, 1}},
    {{-0.5f, -0.5f, 0.5f}, {0, -1, 0}, {1, 0, 1, 1}},
    {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0, 1, 1}},
    {{0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0, 1, 1}},
};

uint16_t indices[36] = {
    0,  1,  2,  0,  2,  3,  // front
    4,  5,  6,  4,  6,  7,  // back
    8,  9,  10, 8,  10, 11, // left
    12, 13, 14, 12, 14, 15, // right
    16, 17, 18, 16, 18, 19, // top
    20, 21, 22, 20, 22, 23, // bottom
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

  D3D11_TEXTURE2D_DESC depthDesc{};
  depthDesc.Width = 800.0f;
  depthDesc.Height = 800.0f;
  depthDesc.MipLevels = 1;
  depthDesc.ArraySize = 1;
  depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
  depthDesc.SampleDesc.Count = 1;
  depthDesc.Usage = D3D11_USAGE_DEFAULT;
  depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

  ComPtr<ID3D11Texture2D> depthTex;
  device->CreateTexture2D(&depthDesc, nullptr, &depthTex);

  ComPtr<ID3D11DepthStencilView> DepthStencilView;
  device->CreateDepthStencilView(depthTex.Get(), nullptr, &DepthStencilView);

  D3D11_DEPTH_STENCIL_DESC dsDesc{};
  dsDesc.DepthEnable = TRUE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

  ComPtr<ID3D11DepthStencilState> dsState;
  device->CreateDepthStencilState(&dsDesc, &dsState);
  context->OMSetDepthStencilState(dsState.Get(), 0);

  CompileShaders(L"resources/vertex.hlsl", L"resources/pixel.hlsl");

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "bunny.obj");
  if (!err.empty())
    std::cout << err << "\n";
  if (!ok)
    std::cout << "failed to load\n";

  std::cout << "positions: " << attrib.vertices.size() / 3 << "\n";
  std::cout << "normals:   " << attrib.normals.size() / 3 << "\n";
  std::cout << "shapes:    " << shapes.size() << "\n";

  MeshData bunny = buildMesh(attrib, shapes);
  std::cout << "vertices: " << bunny.vertices.size()
            << ", indices: " << bunny.indices.size() << "\n";

  ComPtr<ID3D11Buffer> vertexBuffer = createVertexBuffer(
      bunny.vertices.data(), (UINT)(bunny.vertices.size() * sizeof(Vertex)));

  D3D11_INPUT_ELEMENT_DESC layout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  ComPtr<ID3D11InputLayout> inputLayout;
  hr = device->CreateInputLayout(layout, 3, vsBlob->GetBufferPointer(),
                                 vsBlob->GetBufferSize(), &inputLayout);
  if (FAILED(hr)) {
    std::cout << "failed to create input layout\n";
  }

  ComPtr<ID3D11Buffer> constantBuffer = createConstantBuffer(sizeof(CBData));

  ComPtr<ID3D11Buffer> indexBuffer = createIndexBuffer(
      bunny.indices.data(), (UINT)(bunny.indices.size() * sizeof(uint32_t)));

  while (!currentWindow.shouldClose()) {
    float color[] = {0.0f, 0.0f, 0.0f, 0.0f};
    D3D11_VIEWPORT vp{0.0f, 0.0f, 800.0f, 800.0f, 0.0f, 1.0f};

    context->RSSetViewports(1, &vp);
    context->IASetInputLayout(inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->OMSetRenderTargets(1, rtv.GetAddressOf(), DepthStencilView.Get());
    context->ClearRenderTargetView(rtv.Get(), color);
    context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH,
                                   1.0f, 0);

    UINT stride = sizeof(Vertex), offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride,
                                &offset);
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);

    context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    glm::vec3 eye =
        glm::vec3(0.0f, 0.0f, -3.0f); // 3 units back from the origin
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // looking at the origin
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::lookAtLH(eye, target, up);

    glm::mat4 proj = glm::perspectiveLH_ZO(glm::radians(60.0f), 800.0f / 800.0f,
                                           0.1f, 100.0f);

    for (const auto &renderable : renderables) {
      D3D11_MAPPED_SUBRESOURCE mapped{};
      hr = context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
                        &mapped);
      if (SUCCEEDED(hr)) {
        CBData data{};
        data.time = (float)glfwGetTime();
        glm::mat4 world = glm::mat4(1.0f);

        data.transform = glm::mat4(1.0f);
        world = glm::translate(
            world, glm::vec3(renderable.TransformProperties.position.x,
                             renderable.TransformProperties.position.y,
                             renderable.TransformProperties.position.z));
        world = glm::rotate(world, data.time, glm::vec3(0.0f, 1.0f, 0.0f));
        world = glm::scale(world,
                           glm::vec3(renderable.TransformProperties.scale.x,
                                     renderable.TransformProperties.scale.y,
                                     renderable.TransformProperties.scale.z));

        data.world = world;
        data.normalMatrix = glm::transpose(glm::inverse(world));
        data.transform = proj * view * world;

        data.lightDir = glm::vec3(cos(data.time), 1.0f, sin(data.time));
        data.cameraPos = eye;
        memcpy(mapped.pData, &data, sizeof(CBData));

        context->Unmap(constantBuffer.Get(), 0);

        context->DrawIndexed((UINT)bunny.indices.size(), 0, 0);
      }
    }

    swapChain->Present(1, 0);
    glfwPollEvents();
  }
}

void Renderer::bindRenderable(const Entity &PassedEntity) {
  std::cout << "binded a entity\n";
  renderables.push_back(PassedEntity);
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

ComPtr<ID3D11Buffer> Renderer::createIndexBuffer(uint32_t *indices,
                                                 UINT byteSize) {
  return createBuffer(byteSize, indices, D3D11_BIND_INDEX_BUFFER,
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

MeshData Renderer::buildMesh(const tinyobj::attrib_t &attrib,
                             const std::vector<tinyobj::shape_t> &shapes) {
  MeshData data{};

  data.vertices.resize(attrib.vertices.size() / 3);
  for (size_t i = 0; i < data.vertices.size(); i++) {
    data.vertices[i].position = {attrib.vertices[3 * i + 0],
                                 attrib.vertices[3 * i + 1],
                                 attrib.vertices[3 * i + 2]};
    data.vertices[i].normal = {0.0f, 0.0f, 0.0f};
    data.vertices[i].color = {0.8f, 0.8f, 0.8f, 1.0f};
  }

  for (const auto &shape : shapes)
    for (const auto &idx : shapes.data()->mesh.indices)
      data.indices.push_back((uint32_t)idx.vertex_index);

  for (size_t i = 0; i + 2 < data.indices.size(); i += 3) {
    Vertex &a = data.vertices[data.indices[i + 0]];
    Vertex &b = data.vertices[data.indices[i + 1]];
    Vertex &c = data.vertices[data.indices[i + 2]];

    glm::vec3 n = glm::cross(b.position - a.position, c.position - a.position);

    a.normal += n;
    b.normal += n;
    c.normal += n;
  }
  for (auto &v : data.vertices)
    v.normal = glm::normalize(v.normal);

  return data;
}
