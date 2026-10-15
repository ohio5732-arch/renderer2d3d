#include "glm/glm/ext/vector_float2.hpp"
#include <iostream>
#include <src/engine/app.hpp>

int main() {
  App CurrentApp("title");
  CurrentApp.createEntity(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
  CurrentApp.createEntity(glm::vec2(0.0f, 0.5f), glm::vec2(0.0f, 0.0f));

  CurrentApp.AppInit();

  return 0;
}
