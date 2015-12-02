#include "Texture.hpp"

Texture::Texture(const glm::vec3& kd1, const glm::vec3& kd2, const glm::vec3& ks, double shininess, double scale, double reflectioness, double refractiveIndex, int type, double bumpScale, double bumpAmount)
	: m_kd1(kd1), m_kd2(kd2), m_ks(ks), m_shininess(shininess), m_scale(scale), m_reflectioness(reflectioness), m_refractiveIndex(refractiveIndex)
{
   textureType = static_cast<TextureType>(type);
   applyBump = BumpTexture(bumpScale,bumpAmount);
}
Texture::~Texture()
{}

glm::vec3 Texture::getCheckerBoardColour(glm::vec3 point)
{
   int squareSize = 16;
   int coef = floor(point.x/squareSize) + floor(point.y/squareSize) + floor(point.z/squareSize);
   coef = coef % 2;
   return m_kd1 * float(coef) + m_kd2 * float(1 - coef);
}

glm::vec3 Texture::getPerlinWoodColour(glm::vec3 point)
{
   // Side Wood
   double x = point.x * m_scale;
   double y = point.y * m_scale;
   double z = point.z * m_scale;

   double grain = perlin.noise(x, y, z) * 5;

   grain = grain - (int)grain;

   return m_kd1 * float(grain) + m_kd2 * float(1.0f - grain);
   

   //double alpha = 0.05;

   /* Ring wood
   double d = sqrt(point.z*point.z+point.y*point.y) + m_scale*perlin.noise(point.x,point.y,point.z);
   double grain = cos(2*3.14*(d - floor(d)));
   return m_kd1 * float(grain) + m_kd2 * float(1.0f - grain);
   */
}

glm::vec3 Texture::getPerlinMarbleColour(glm::vec3 point)
{
   double x = point.x * m_scale;
   double y = point.y * m_scale;
   double z = point.z * m_scale;
   double noiseCoef = 0;

   double alpha = 0.05;

   for (int level = 1; level < 10; level ++) {
      noiseCoef +=  (1.0f / level) * fabsf(perlin.noise(
         level * alpha * x,
         level *  (alpha+0.10) * y,
         level * alpha * z
      ));
   }

   //it was 0.5 0.05 0.5
   noiseCoef = 0.5f * sinf((x + y) * 0.05f + noiseCoef) + 0.5f;
   return m_kd1 * float(noiseCoef) + m_kd2 * float(1.0f - noiseCoef);
}

glm::vec3 Texture::getColour(glm::vec3 point)
{
   glm::vec3 rv = glm::vec3(0.0f);
   switch(textureType)
   {
      case TextureType::PerlinMarble:
         rv = getPerlinMarbleColour(point);
         break;
      case TextureType::PerlinWood:
         rv = getPerlinWoodColour(point);
         break;
      case TextureType::Checkerboard:
         rv = getCheckerBoardColour(point);
         break;
   }
   return rv;
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