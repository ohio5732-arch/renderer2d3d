#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <src/core/window.hpp>
#include <vector>
#include <wrl/client.h>
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <src/engine/entity.hpp>
using Microsoft::WRL::ComPtr;

#include "tiny_obj_loader.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

struct CBData {
  glm::mat4 transform;
  glm::mat4 world;
  glm::mat4 normalMatrix;
  float time;
  glm::vec3 lightDir;
  glm::vec3 cameraPos;
  float pad;
};

struct MeshData {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};

class Renderer {
public:
  Window currentWindow;

  Renderer(Window &passedWindow);
  void bindRenderable(const Entity &passedEntity);
  void render();

private:
  ComPtr<ID3D11Device> device = nullptr;
  ComPtr<ID3D11DeviceContext> context = nullptr;
  ComPtr<IDXGISwapChain> swapChain = nullptr;
  ComPtr<ID3DBlob> vsBlob, psBlob, errors;
  ComPtr<ID3D11VertexShader> vs;
  ComPtr<ID3D11PixelShader> ps;
  HRESULT hr;

  std::vector<Entity> renderables;

  ComPtr<ID3D11Buffer> createBuffer(UINT byteSize, const void *initialData,
                                    UINT bindFlags,
                                    D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
                                    UINT cpuAccess = 0);
  ComPtr<ID3D11Buffer> createVertexBuffer(Vertex vertices[], UINT byteSize);
  ComPtr<ID3D11Buffer> createIndexBuffer(uint32_t indices[], UINT byteSize);
  ComPtr<ID3D11Buffer> createConstantBuffer(UINT byteSize);
  void CompileShaders(const wchar_t *vsSrc, const wchar_t *psSrc);
  MeshData buildMesh(const tinyobj::attrib_t &attrib,
                     const std::vector<tinyobj::shape_t> &shapes);
};
