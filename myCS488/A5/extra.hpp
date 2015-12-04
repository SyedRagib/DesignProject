#pragma once

#include <glm/glm.hpp>
#include "Material.hpp"

struct SamplePoissonPoint
{
	glm::vec2 p1;
	glm::vec2 p2;
	glm::vec2 p3; 
};

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
typedef struct SamplePoissonPoint SamplePoissonPoint;