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
   //Return normal
   if(m_bumpScale <= 0.0)
   		return n;
   else
   {
      //Calculate the bumped normal
      p = float(1/m_bumpScale)*p;
      glm::vec3 bumppedNormal = float(m_bumpBy)*
                            glm::vec3(float(imp.noise(p.x,p.y,p.z)), 
                                      float(imp.noise(p.y,p.z,p.x)), 
                                      float(imp.noise(p.z,p.x,p.y)))+
                            n;
      return glm::normalize(bumppedNormal);
   }
}