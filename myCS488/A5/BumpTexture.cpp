#include "BumpTexture.hpp"
#include <iostream>
BumpTexture::BumpTexture()
{}

BumpTexture::BumpTexture(double bumpingScale, double bumpBy) : m_bumpScale(bumpingScale), m_bumpBy(bumpBy)
{
}

BumpTexture::~BumpTexture()
{}

glm::vec3 BumpTexture::getNormal(glm::vec3 p, glm::vec3 n)
{
   
   if(m_bumpScale <= 0.0)
   		return n;
   else
   {
   	  double x=p.x/m_bumpScale;
      double y=p.y/m_bumpScale;
      double z=p.z/m_bumpScale;

      glm::vec3 newNormal = glm::vec3(float(imp.noise(x,y,z)), float(imp.noise(y,z,x)), float(imp.noise(z,x,y)));
      newNormal = n + newNormal*float(m_bumpBy);
      return glm::normalize(newNormal);
   }
}