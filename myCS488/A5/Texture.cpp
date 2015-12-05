#include "Texture.hpp"

Texture::Texture(const glm::vec3& kd1, const glm::vec3& kd2, const glm::vec3& ks, double shininess, double scale, double reflectioness, double refractiveIndex, int type, double bumpScale, double bumpAmount)
	: m_kd1(kd1), m_kd2(kd2), m_ks(ks), m_shininess(shininess), m_scale(scale), m_reflectioness(reflectioness), m_refractiveIndex(refractiveIndex)
{
   textureType = static_cast<TextureType>(type);
   applyBump = BumpTexture(bumpScale,bumpAmount);
}
Texture::~Texture()
{}

double Texture::getCheckerBoardColour(glm::vec3 point)
{
   int squareSize = 16;
   int coef = floor(point.x/squareSize) + floor(point.y/squareSize) + floor(point.z/squareSize);
   coef = coef % 2;
   return coef;
}

double Texture::getPerlinWoodColour(glm::vec3 point)
{
   // Side Wood
   float woodScale = 0.01f;

   point = point*woodScale;

   int rings = m_scale;
   double grain = perlin.noise(point.x, point.y, point.z) * rings;

   grain = grain - (int)grain;

   return grain;
}

double Texture::getPerlinMarbleColour(glm::vec3 point)
{
   double x = point.x * m_scale;
   double y = point.y * m_scale;
   double z = point.z * m_scale;
   double noiseCoef = 0.0;

   double alpha = 0.05;
   float sineCoef1 = 0.5f, sineCoef2 = 0.05f;
   int maxCounter = 11;
   for (int counter = 1; counter < maxCounter; counter ++) {
      noiseCoef +=  (1.0f / counter) * fabsf(perlin.noise( counter * alpha * x, counter *  (alpha+0.10) * y, counter * alpha * z
      ));
   }
   noiseCoef = sineCoef1 * sinf((x + y) * sineCoef2 + noiseCoef) + sineCoef1;
   return noiseCoef;
}

glm::vec3 Texture::getColour(glm::vec3 point)
{
   double noiseCoef = 0.0f;
   switch(textureType)
   {
      case TextureType::PerlinMarble:
         noiseCoef = getPerlinMarbleColour(point);
         break;
      case TextureType::PerlinWood:
         noiseCoef = getPerlinWoodColour(point);
         break;
      case TextureType::Checkerboard:
         noiseCoef = getCheckerBoardColour(point);
         break;
   }
   return (m_kd1 * float(noiseCoef) + m_kd2 * float(1.0f - noiseCoef));
}

glm::vec3 Texture::getNormal(glm::vec3 p, glm::vec3 n)
{
   return applyBump.getNormal(p, n);
}

glm::vec3 Texture::getSpecular()
{
	return m_ks;
}

double Texture::getShininess()
{
	return m_shininess;
}

double Texture::getReflectivity()
{
	return m_reflectioness;
}

double Texture::getRefractivity()
{
	return m_refractiveIndex;
}