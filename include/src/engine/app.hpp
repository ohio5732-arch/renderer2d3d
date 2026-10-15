#pragma once

#include <src/core/window.hpp>
#include <src/engine/renderer.hpp>

class App {
public:
  App(const char *title);
  ~App();

private:
  Window *AppWindow;
  Renderer *AppRenderer;
};
