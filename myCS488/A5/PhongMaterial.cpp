#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectioness, double refraction, double refractiveIndex, double bumpScale, double bumpAmount)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_reflectioness(reflectioness)
	, m_refraction(refraction)
 	, m_refractiveIndex(refractiveIndex)
{
	applyBump = BumpTexture(bumpScale,bumpAmount);
}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::getColour(glm::vec3 point)
{
	return m_kd;
}

glm::vec3 PhongMaterial::getNormal(glm::vec3 p, glm::vec3 n)
{	
	//return n;
	return applyBump.getNormal(p,n);
}

glm::vec3 PhongMaterial::getSpecular()
{
	return m_ks;
}

double PhongMaterial::getShininess()
{
	return m_shininess;
}

double PhongMaterial::getReflectivity()
{
	return m_reflectioness;
}

double PhongMaterial::getRefractivity()
{
	return m_refractiveIndex;
}