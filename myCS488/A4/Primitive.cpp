#include "Primitive.hpp"
#include "polyroots.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}


bool NonhierBox::checkIntersectionHelper(double rayPosition, double rayDirection, double cubePosition, Intersection &i, int dimension, double &minNear, double &maxFar)
{
	bool Intersected = false;

	double intersection1 = (cubePosition - rayPosition) / rayDirection;
	double intersection2 = (cubePosition + m_size - rayPosition) / rayDirection;

	//Swap the intersection
	if(intersection1 > intersection2)
	{
		double copy = intersection1;
		intersection1 = intersection2;
		intersection2 = copy;
	}

	if(intersection2 < maxFar)
		maxFar = intersection2;

	if(intersection1 > minNear)
	{
		glm::vec3 newNormal = glm::vec3(dimension == 0 ? 0.0f : -1.0f*rayDirection, 
										dimension == 1 ? 0.0f : -1.0f*rayDirection, 
										dimension == 2 ? 0.0f : -1.0f*rayDirection);
		newNormal = glm::normalize(newNormal);
		i.normal = glm::vec4(newNormal,0.0f);
		minNear = intersection1;
	}

	if(minNear > maxFar || maxFar < 0.0f)
		return false;

	return true;
}

Intersection NonhierBox::checkIntersection(Ray r, bool skipBound)
{
	Intersection i;
	i.distance = -1;
	i.hit = false;

	double near = -10000000.0;
	double far = 10000000.0;

	for(int n=0; n<3; n++)
		 if(!checkIntersectionHelper(r.rayPosition[n], r.rayDirection[n], m_pos[n], i, n, near, far))
		 	return i;

	double positionX = r.rayDirection.x*(near+1e-7) + r.rayPosition.x;
	double positionY = r.rayDirection.y*(near+1e-7) + r.rayPosition.y;
	double positionZ = r.rayDirection.z*(near+1e-7) + r.rayPosition.z;

	i.point = glm::vec4(positionX,positionY,positionZ,1.0f);
	i.distance = near - 1e-10;
	i.hit = true;
	return i;
}

Intersection NonhierSphere::checkIntersection(Ray r, bool skipBound)
{
	Intersection i;

	i.distance = -1;
	i.hit = false;

	double A = pow(r.rayDirection.x,2)+pow(r.rayDirection.y,2)+pow(r.rayDirection.z,2);
	double B = 2*(r.rayDirection.x*(r.rayPosition.x - m_pos.x) + r.rayDirection.y*(r.rayPosition.y-m_pos.y) + r.rayDirection.z*(r.rayPosition.z-m_pos.z));
	double C = pow(r.rayPosition.x-m_pos.x,2)+pow(r.rayPosition.y-m_pos.y,2)+pow(r.rayPosition.z-m_pos.z,2)-pow(m_radius,2);

	double roots[2];
	int numRoots = quadraticRoots(A,B,C, roots);
	double distance = 0;

	if(numRoots == 0)
		return i;

	if(numRoots == 2)
	{
		double temp;
		if(roots[0] < roots[1])
			temp = roots[0];
		else
			temp = roots[1];

		if(temp > 1e-10)
		{
			distance = roots[1];
			i.hit = true;
		}
	}
	else if (roots[0] > 1e-10)
	{
		distance = roots[0];
		i.hit = true;
	}
	else
		i.hit = false;

	glm::vec4 p = r.rayPosition+glm::vec4(r.rayDirection.x*(distance+1e-7),r.rayDirection.y*(distance+1e-7), r.rayDirection.z*(distance+1e-7), 1.0f);
	i.point = p;
	i.normal = p - glm::vec4(m_pos.x,m_pos.y,m_pos.z,0.0f);
	i.distance = distance;
	i.normal = glm::normalize(i.normal);
	return i;
}

Sphere::Sphere()
{
	nhSphere = new NonhierSphere(glm::vec3(0,0,0),1);
}

Intersection Sphere::checkIntersection(Ray r, bool skipBound)
{
	return nhSphere->checkIntersection(r, skipBound);
}

Cube::Cube()
{
	nhBox = new NonhierBox(glm::vec3(0,0,0), 1);
}

Intersection Cube::checkIntersection(Ray r, bool skipBound)
{
	return nhBox->checkIntersection(r, skipBound);
}