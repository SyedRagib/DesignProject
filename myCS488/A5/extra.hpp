#pragma once

#include <glm/glm.hpp>
#include "Material.hpp"

struct Ray
{
	glm::vec4 rayDirection;
	glm::vec4 rayPosition;
	bool returnOutput;
};

struct Intersection
{
	glm::vec4 point;
	glm::vec4 normal;
	Material *material;
	double distance;
	bool hit;
};

typedef struct Ray Ray;
typedef struct Intersection Intersection;
typedef glm::vec3 Colour;