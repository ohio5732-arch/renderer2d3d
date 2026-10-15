#pragma once

#include "glm/glm/ext/vector_float2.hpp"
#include "src/engine/entity.hpp"
#include <src/core/window.hpp>
#include <src/engine/renderer.hpp>

class App {
  using vec2 = glm::vec2;

public:
  App(const char *title);
  ~App();

  void AppInit();
  Entity createEntity(vec2 position, vec2 scale);

private:
  Window *AppWindow;
  Renderer *AppRenderer;
};
