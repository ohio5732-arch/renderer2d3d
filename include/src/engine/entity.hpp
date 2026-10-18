#pragma once
#include "glm/glm/ext/vector_float3.hpp"
#include "glm/glm/glm.hpp"

struct Transform {
  glm::vec3 position;
  glm::vec3 scale;
};

struct Physical {};

struct Entity {
  Transform TransformProperties;
  Physical PhysicalPropertiel;
};
