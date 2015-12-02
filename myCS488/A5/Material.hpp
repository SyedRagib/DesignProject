#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual ~Material();
  virtual glm::vec3 getColour(glm::vec3 point) = 0;
  virtual glm::vec3 getNormal(glm::vec3 p, glm::vec3 n) = 0;
  virtual glm::vec3 getSpecular() = 0;
  virtual double getShininess() = 0;
  virtual double getReflectivity() = 0;
  virtual double getRefractivity() = 0;
protected:
  Material();
};
