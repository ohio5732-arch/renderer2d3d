#include "src/engine/app.hpp"
#include "src/engine/renderer.hpp"
#include <iostream>

App::App(const char *appName) {
  this->AppWindow = new Window(800, 800, appName);
  this->AppRenderer = new Renderer(*this->AppWindow);

  AppRenderer->render();
}

App::~App() {
  std::cout << "deleting shit\n";
  delete AppRenderer;
  delete AppWindow;
}
