
#include <src/engine/app.hpp>

int main() {
  App CurrentApp("title");
  CurrentApp.createEntity(glm::vec3(0.0f, -1.5f, 0.0f),
                          glm::vec3(10.0f, 10.0f, 10.0f));

  CurrentApp.AppInit();

  return 0;
}
