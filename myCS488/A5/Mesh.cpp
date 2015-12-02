#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include <glm/glm.hpp>

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	double xMin, xMax, yMin, yMax, zMin, zMax;
	xMin = yMin = zMin = 10000000.0f;
	xMax = yMax = zMax = -10000000.0f;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );

			if(vx < xMin)
				xMin = vx;
			if(vx > xMax)
				xMax = vx;

			if(vy < yMin)
				yMin = vy;
			if(vy > yMax)
				yMax = vy;

			if(vz < zMin)
				zMin = vz;
			if(vz > zMax)
				zMax = vz;

		} 
		else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

	boundingPoint = glm::vec3((xMax+xMin)/2.0f,(yMax+yMin)/2.0f,(zMax+zMin)/2.0f);

	//Calculate the largest radius
	std::vector<double> radiusCandidates;
	boundingRadius = 0.0f;
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMax,yMax,zMax))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMax,yMax,zMin))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMax,yMin,zMax))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMin,yMax,zMax))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMax,yMin,zMin))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMin,yMin,zMin))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMin,yMin,zMax))));
	radiusCandidates.push_back(double(glm::distance(boundingPoint, glm::vec3(xMin,yMax,zMin))));

	for(int k=0; k<radiusCandidates.size(); k++)
		if(radiusCandidates[k] > boundingRadius)
			boundingRadius = radiusCandidates[k];
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

Intersection Mesh::checkIntersection(Ray r, bool skipBound)
{

	Intersection i;
	i.hit = false;

	//if(r.returnOutput)
	//	std::cout << skipBound << std::endl;

	//Bounds check first
	
	if(!skipBound)
	{
		NonhierSphere boundingSphere = NonhierSphere(boundingPoint, boundingRadius);
		Intersection boundingIntersection = boundingSphere.checkIntersection(r, true);

		//If the ray fails to intersect with the bounded sphere then just avoid this primitive	
		if(!boundingIntersection.hit)
			return i;
	}
	
	
	glm::vec3 IntersectionPoint;
	glm::vec3 IntersectionNormal;

	double intersectionMin, intersectionMax, currentIntersection;
	intersectionMin = intersectionMax = 10000000.0f;

	glm::vec3 rayDirection = glm::vec3(r.rayDirection.x,r.rayDirection.y,r.rayDirection.z);
	glm::vec3 rayPosition  = glm::vec3(r.rayPosition.x,r.rayPosition.y, r.rayPosition.z);

	for(int n=0; n < m_faces.size(); n++)
	{
		//Get the vertices of the triangle
		glm::vec3 v1,v2,v3;
		v1 = glm::vec3(m_vertices[m_faces[n].v1]);
		v2 = glm::vec3(m_vertices[m_faces[n].v2]); 
		v3 = glm::vec3(m_vertices[m_faces[n].v3]);

		//Calculate the normal and normalize it
		IntersectionNormal = glm::cross(v3-v2, v1-v2);
		IntersectionNormal = glm::normalize(IntersectionNormal);


		glm::mat3 tempMatrix = glm::mat3();
		tempMatrix[0] = IntersectionNormal;
		/*
		if(r.returnOutput)
		{
			std::cout << IntersectionNormal.x << std::endl;
			std::cout << IntersectionNormal.y << std::endl;
			std::cout << IntersectionNormal.z << std::endl;
			bool stuff = glm::dot(IntersectionNormal, rayDirection) < 1e-3;
			std::cout << stuff << std::endl;
		}
		*/	
			//std::cout << IntersectionNormal << std::endl;


		//if(glm::dot(IntersectionNormal,rayDirection) < 1e-3)
		//	continue;
		currentIntersection = glm::dot(IntersectionNormal, v1 - rayPosition) / glm::dot(IntersectionNormal, rayDirection);

		//Check the t value
		if(currentIntersection > intersectionMin)
			continue;
		
		//Calculate the intersection point
		IntersectionPoint = rayPosition + (currentIntersection+1e-7) * rayDirection;

		//Check intersection of halfspace
		double halfspaceIntersection1 = glm::dot(glm::cross(v2-v1, IntersectionPoint-v1), IntersectionNormal);
		double halfspaceIntersection2 = glm::dot(glm::cross(v3-v2, IntersectionPoint-v2), IntersectionNormal);
		double halfspaceIntersection3 = glm::dot(glm::cross(v1-v3, IntersectionPoint-v3), IntersectionNormal);

		if( (halfspaceIntersection1 < 0) || (halfspaceIntersection2 < 0) || (halfspaceIntersection3 < 0) )
			continue;
		
			
		i.hit = true;
		i.point = glm::vec4(IntersectionPoint, 1.0f);
		i.normal = 1.0f*glm::vec4(IntersectionNormal, 0.0f);
		i.distance = currentIntersection;
		intersectionMin = currentIntersection;

	}

	if(intersectionMin == intersectionMax)
		i.hit = false;

	return i;
}
