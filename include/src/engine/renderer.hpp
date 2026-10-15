#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <src/core/window.hpp>
#include <wrl/client.h>
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
using Microsoft::WRL::ComPtr;

struct Vertex {
  glm::vec3 position;
  glm::vec4 color;
};

struct CBData {
  glm::mat4 transform; // bytes 0-63  -> u_transform
  float time;          // bytes 64-67 -> u_time
  float padding[3];    // bytes 68-79, keeps the size a multiple of 16
};

class Renderer {
public:
  Window currentWindow;

  Renderer(Window &passedWindow);
  void render();

private:
  ComPtr<ID3D11Device> device = nullptr;
  ComPtr<ID3D11DeviceContext> context = nullptr;
  ComPtr<IDXGISwapChain> swapChain = nullptr;
  ComPtr<ID3DBlob> vsBlob, psBlob, errors;
  ComPtr<ID3D11VertexShader> vs;
  ComPtr<ID3D11PixelShader> ps;
  HRESULT hr;

  ComPtr<ID3D11Buffer> createBuffer(UINT byteSize, const void *initialData,
                                    UINT bindFlags,
                                    D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
                                    UINT cpuAccess = 0);
  ComPtr<ID3D11Buffer> createVertexBuffer(Vertex vertices[], UINT byteSize);
  ComPtr<ID3D11Buffer> createConstantBuffer(UINT byteSize);
  void CompileShaders(const wchar_t *vsSrc, const wchar_t *psSrc);
};
