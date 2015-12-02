#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "ImprovedPerlinNoise.hpp"
#include "BumpTexture.hpp"

enum TextureType { PerlinMarble=0, PerlinWood=1, Checkerboard=2, PerlinBump=3};

class Texture : public Material
{
public:
  Texture(const glm::vec3& kd1, const glm::vec3& kd2, const glm::vec3& ks, double shininess, double scale, double reflectioness, double refractiveIndex, int type, double bumpScale, double bumpAmount);
  virtual ~Texture();

  virtual glm::vec3 getColour(glm::vec3 point);
  virtual glm::vec3 getSpecular();
  virtual double getShininess();
  virtual double getReflectivity();
  virtual double getRefractivity();
  virtual glm::vec3 getNormal(glm::vec3 p, glm::vec3 n);

  glm::vec3 m_kd1;
  glm::vec3 m_kd2;
  glm::vec3 m_ks;

  double m_shininess;

  double m_reflectioness;
  double m_refractiveIndex;

private:
  glm::vec3 getPerlinMarbleColour(glm::vec3 point);
  glm::vec3 getPerlinWoodColour(glm::vec3 point);
  glm::vec3 getCheckerBoardColour(glm::vec3 point);
  

  TextureType textureType;
	ImprovedPerlinNoise perlin;
	double m_scale;
  BumpTexture applyBump;
};