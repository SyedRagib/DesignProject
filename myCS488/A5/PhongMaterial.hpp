#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "BumpTexture.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectioness, double refraction, double refractiveIndex, double bumpScale, double bumpAmount);
  virtual ~PhongMaterial();
  
  virtual glm::vec3 getColour(glm::vec3 point);
  virtual glm::vec3 getSpecular();
  virtual double getShininess();
  virtual double getReflectivity();
  virtual double getRefractivity();
  virtual glm::vec3 getNormal(glm::vec3 p, glm::vec3 n);

  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;

  double m_reflectioness;
  double m_refraction;
  double m_refractiveIndex;
private:
  BumpTexture applyBump;
};
