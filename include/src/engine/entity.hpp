#pragma once
#include "glm/glm/glm.hpp"

struct Transform {
  glm::vec2 position;
  glm::vec2 scale;
};

struct Entity {
  Transform TransformProperties;
};
