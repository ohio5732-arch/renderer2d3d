#include "src/engine/app.hpp"
#include "src/engine/entity.hpp"
#include "src/engine/renderer.hpp"
#include <iostream>

App::App(const char *appName) {
  this->AppWindow = new Window(800, 800, appName);
  this->AppRenderer = new Renderer(*this->AppWindow);
}

App::~App() {
  std::cout << "deleting shit\n";
  delete AppRenderer;
  delete AppWindow;
}

void App::AppInit() { AppRenderer->render(); }

Entity App::createEntity(glm::vec3 position, glm::vec3 scale) {
  Entity newEntity{};
  newEntity.TransformProperties.position = position;
  newEntity.TransformProperties.scale = scale;

  AppRenderer->bindRenderable(newEntity);

  return newEntity;
}
