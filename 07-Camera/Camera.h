#pragma once
#include <glm/glm.hpp>

struct Camera
{
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;

  float speed;

  Camera(const glm::vec3& cameraPos,
         const glm::vec3& front,
         const glm::vec3& up)
         : pos(cameraPos),
           front(front),
           up(up),
           speed(0.0f) {}

  glm::vec3 cal_direction() { return glm::normalize(pos + front); }
  glm::vec3 cal_right() { return glm::normalize(glm::cross(front, up)); }
};

