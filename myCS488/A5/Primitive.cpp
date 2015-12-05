#include "Primitive.hpp"
#include "polyroots.hpp"
#include <iostream>
#include <vector>	

bool oncer = false;

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

Torus::~Torus()
{}

Cylinder::~Cylinder()
{}

Cone::~Cone()
{}

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
		//newNormal = glm::normalize(newNormal);
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
		/*
		double temp;
		if(roots[0] > roots[1])
		{
			temp = roots[0];
			roots[0] = roots[1];
			roots[1] = temp;
		}

		if(roots[0] > 1e-2)
		{
			distance = roots[0];
			i.hit = true;
		}
		else if (roots[1] > 1e-2)
		{
			distance = roots[1];
			i.hit = true;
		}
		*/
		
		
		double temp;
		if(roots[0] < roots[1])
			temp = roots[0];
		else
			temp = roots[1];

		if(temp > 1e-10)
		{
			distance = temp;//roots[1];
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
	//i.normal = glm::normalize(i.normal);
	return i;
}

void Torus::stuff(Ray r, bool skipBound)
{}


Intersection Cylinder::checkIntersection(Ray r, bool skipBound)
{
	Intersection i;
	i.hit = false;

	double minValue = 0.000001f;
	double t = -1.0f;

	glm::vec3 rayOrigin = glm::vec3(r.rayPosition.x, r.rayPosition.y, r.rayPosition.z);
	glm::vec3 rayDirection = glm::vec3(r.rayDirection.x, r.rayDirection.y, r.rayDirection.z);
	glm::vec3 bottomToOrigin = rayOrigin - m_bottomCenter;
	glm::vec3 topToOrigin = rayOrigin - m_topCenter;
	glm::vec3 u = rayDirection - m_upAxis*glm::dot(rayDirection, m_upAxis);
	glm::vec3 v = bottomToOrigin - m_upAxis*glm::dot(bottomToOrigin, m_upAxis);


	double a = glm::dot(u,u);
	double b = 2*glm::dot(u,v);
	double c = glm::dot(v,v)  - m_radius*m_radius;
	double roots[2];
	int numRoots = quadraticRoots(a,b,c,roots);
	for(int index=0; index<numRoots; index++)
	{
		glm::vec3 cylinderPoint = rayOrigin + float(roots[index])*rayDirection;
		glm::vec3 bottomCenterToPoint = cylinderPoint - m_bottomCenter;
		glm::vec3 topCenterToPoint = cylinderPoint - m_topCenter;

		if(glm::dot(m_upAxis, bottomCenterToPoint) > 0.0f && glm::dot(m_upAxis, topCenterToPoint) < 0.0f)
			if(t < 0.0f || t > roots[index])
				t = roots[index];
	}

	double upAxisDotDirection = glm::dot(m_upAxis, rayDirection);
	
	if(fabs(upAxisDotDirection) < minValue)
		return i;
	
	//check t3 is valid root
	double upAxisDotOrigin = glm::dot(m_upAxis, rayOrigin);
	double bottomToOriginDotUpAxis = glm::dot(bottomToOrigin, m_upAxis);
	double possibleRoot = -1.0f*bottomToOriginDotUpAxis/upAxisDotDirection;
	if(possibleRoot > 0.0f)
	{
		glm::vec3 cylinderPoint = rayOrigin + float(possibleRoot)*rayDirection;
		glm::vec3 bottomCenterToPoint = cylinderPoint - m_bottomCenter;
		if(glm::dot(bottomCenterToPoint, bottomCenterToPoint) < m_radius*m_radius)
			if(t < 0.0f || t > possibleRoot)
				t = possibleRoot;
	}

	//check if t4 is valid root
	double topToOriginDotUpAxis = glm::dot(topToOrigin, -1.0f*m_upAxis);
	possibleRoot = topToOriginDotUpAxis/upAxisDotDirection;
	if(possibleRoot > 0.0f)
	{
		glm::vec3 cylinderPoint = rayOrigin + float(possibleRoot)*rayDirection;
		glm::vec3 topCenterToPoint = cylinderPoint - m_topCenter;
		if(glm::dot(topCenterToPoint, topCenterToPoint) < m_radius*m_radius)
			if(t < 0.0f || t > possibleRoot)
				t = possibleRoot;	
	}
	
	if(t > 0.0f)
	{
		//Finalize the intersection class
		glm::vec3 cylinderPoint = rayOrigin + float(t)*rayDirection;

		i.hit = true;
		i.point = glm::vec4(cylinderPoint, 1.0f);
		i.distance = t;

		//Calculate normal
		double squaredR = m_radius*m_radius;
		glm::vec3 topCenterToPoint = cylinderPoint - m_topCenter;
		if(glm::dot(topCenterToPoint, m_upAxis) < minValue && glm::dot(topCenterToPoint,topCenterToPoint) < squaredR)
		{
			i.normal = glm::vec4(m_upAxis, 0.0f);	//hit the top
			return i;
		}
		
		glm::vec3 bottomCenterToPoint = cylinderPoint - m_bottomCenter;
		if(glm::dot(bottomCenterToPoint, m_upAxis) < minValue && glm::dot(bottomCenterToPoint, bottomCenterToPoint) < squaredR)
		{
			i.normal = glm::vec4(-1.0f*m_upAxis, 0.0f); //hit the bottom
			return i;
		}

		//hit the sides
		glm::vec3 pPrime = glm::vec3(m_topCenter.x, cylinderPoint.y,m_topCenter.z);
		glm::vec3 normal = cylinderPoint - pPrime;
		i.normal = glm::vec4(normal, 0.0f);
	}

	return i;
}

Intersection Torus::checkIntersection(Ray r, bool skipBound)
{
	Intersection i;
	i.hit = false;

	double Asquared = m_radius*m_radius;
	double Bsquared = m_tubeRadius*m_tubeRadius;
	//This needs to be swapped
	glm::vec3 d = glm::vec3(r.rayPosition.x, r.rayPosition.y, r.rayPosition.z);
	glm::vec3 e = glm::vec3(r.rayDirection.x, r.rayDirection.y, r.rayDirection.z);

	double G = 4*Asquared*(e.x*e.x + e.y*e.y);
	double H = 8*Asquared*(d.x*e.x + d.y*e.y);
	double I = 4*Asquared*(d.x*d.x + d.y*d.y);
	double J = e.x*e.x + e.y*e.y + e.z*e.z;
	double K = 2*glm::dot(e,d);
	double L = d.x*d.x + d.y*d.y + d.z*d.z + (Asquared - Bsquared);

	double A = J*J;
	double B = 2*J*K;
	double C = 2*J*L+K*K-G;
	double D = 2*K*L-H;
	double E = L*L-I;
	
	double roots[4];
	int numRoots = quarticRoots(B, C, D, E, roots);
	if(numRoots == 0)
		return i;
	
	//Find minimum of the solution
	double t = 1000000.0f;
	for(int x=0; x<numRoots; x++)
		if(t > roots[x])
			t = roots[x];
	
	//Calculate the normal
	glm::vec3 p = d + float(t+1e-7)*e;
	glm::vec3 pPrime = d + float(t+1e-7)*e;
	pPrime.z = m_pos.y;
	glm::vec3 q = float(m_radius/glm::length(pPrime))*pPrime;

	i.hit = true;
	i.point = glm::vec4(p, 1.0f);
	i.distance = t;
	i.normal = glm::vec4(glm::normalize(p-q), 0.0f);
	return i;
}

Intersection Cone::checkIntersection(Ray r, bool skipBound)
{
	
	Intersection i;
	i.hit = false;

	glm::vec3 rayDirection = glm::vec3(r.rayDirection.x, r.rayDirection.y, r.rayDirection.z);
	glm::vec3 rayOrigin = glm::vec3(r.rayPosition.x, r.rayPosition.y, r.rayPosition.z);
	glm::vec3 downVector = -1.0f*m_Axis;
	glm::vec3 top = m_botCenter + m_Axis*float(m_height);

	double cosAlphaSqr = cos(m_alpha)*cos(m_alpha);
	double sineAlphaSqr = 1 - cosAlphaSqr; //using trig identity where sin2(u)+cos2(u)=1
	glm::vec3 deltaP = rayOrigin - top;
	

	//Setup the A coefficient
	glm::vec3 temp1 = rayDirection-downVector*(glm::dot(downVector, rayDirection));
	double temp2 = glm::dot(rayDirection,downVector);
	double aCoefficient1 = glm::dot(temp1,temp1);
	double aCoefficient2 = temp2*temp2;
	double A = cosAlphaSqr*aCoefficient1-sineAlphaSqr*aCoefficient2;

	//Setup the B coefficient
	double bCoefficient1 = glm::dot(temp1, deltaP-glm::dot(deltaP, downVector)*downVector);
	double bCoefficient2 = glm::dot(rayDirection, downVector)*glm::dot(deltaP, downVector);
	double B = 2.0*cosAlphaSqr*bCoefficient1-2.0*sineAlphaSqr*bCoefficient2;

	//Setup the C coefficient
	glm::vec3 temp3 = deltaP - downVector*glm::dot(deltaP, downVector);
	double cCoefficient1 = glm::dot(temp3, temp3);
	double cCoefficient2 = glm::dot(deltaP, downVector)*glm::dot(deltaP, downVector);
	double C = cosAlphaSqr*cCoefficient1 - sineAlphaSqr*cCoefficient2;

	double roots[2];
	int numRoots = quadraticRoots(A,B,C, roots);

	double t=-1.0;
	for(int i=0; i<numRoots; i++)
	{
		if( glm::dot(m_Axis, rayOrigin+rayDirection*float(roots[i]) - m_botCenter) > 0.0f &&
			glm::dot(m_Axis, rayOrigin+rayDirection*float(roots[i]) - top) < 0.0f &&
			(t < 0.0 || roots[i] < t))
			t = roots[i];
	}

	//no solution found
	if(numRoots == 0)
		return i;

	float beta = glm::dot(rayDirection, m_Axis);
	if(beta > 1e-6)
	{
		double rootCandidate = (glm::dot(m_botCenter-rayOrigin, m_Axis)) / beta;
		glm::vec3 temp4 = rayDirection*float(rootCandidate) - (m_botCenter-rayOrigin);
		if(rootCandidate > 0.0 &&
			(t < 0.0 || rootCandidate < t) &&
			glm::dot(temp4, temp4) <= m_radius*m_radius)
			t = rootCandidate;
	}

	if(t > 0.0)
	{
		i.hit = true;
		glm::vec3 point = rayOrigin+rayDirection*float(t);
		i.point = glm::vec4(point, 1.0f);
		
		glm::vec3 normal;
		glm::vec3 botCenterToPoint = point - m_botCenter;
		//is it lying on the bottom
		if(abs(glm::dot(downVector, botCenterToPoint)) < 0.000001f && glm::dot(botCenterToPoint, botCenterToPoint) < m_radius*m_radius)
			normal = downVector;
		else
		{
			//It's lygin on the side
			normal = point - (downVector*glm::dot(botCenterToPoint, downVector) + m_botCenter);
			float ratio = -1.0f*m_radius/m_height;
			normal = normal + downVector*(ratio*glm::length(normal)); 
		}
		i.normal =glm::vec4(normal, 0.0f);
		i.distance = t;
	}

	return i;
}

//Hierarchical constructors
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
