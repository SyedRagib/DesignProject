#pragma once
#include "ImprovedPerlinNoise.hpp"
#include <glm/glm.hpp>

class BumpTexture
{
public:
	glm::vec3 getNormal(glm::vec3 p, glm::vec3 n);
	BumpTexture();
	BumpTexture(double bumpingScale, double bumpBy);
	~BumpTexture();

private:
	double m_bumpScale;
	double m_bumpBy;
	ImprovedPerlinNoise imp;
};