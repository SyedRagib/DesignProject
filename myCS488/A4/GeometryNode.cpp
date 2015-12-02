#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

Intersection GeometryNode::checkIntersection(Ray r)
{
	Intersection i;
	i.hit = false;

	//Reconfigure our ray
	r.rayDirection = invtrans*r.rayDirection;
	r.rayPosition = invtrans*r.rayPosition;

	double minDistance = 1000000.0f;
	//Check the intersection with the primitive
	bool skipBound = false;
	if(m_name == "plane")
		skipBound = true;
	i = m_primitive->checkIntersection(r, skipBound);

	
	if(i.distance == -1)
		i.distance = minDistance;

	for(SceneNode * childNode: children)
	{
		Intersection childI = childNode->checkIntersection(r);

		if(childI.hit && childI.distance < i.distance)
			i = childI;
	}

	//set the material
	if(i.hit)
		i.material = m_material;

	i.point = trans*i.point;

	//Calculate the new normal
	glm::mat3 trimmedInvtrans(invtrans);
	glm::vec3 newNormal = glm::transpose(trimmedInvtrans)*glm::vec3(i.normal.x,i.normal.y,i.normal.z);
	i.normal = glm::vec4(newNormal, 0.0f);

	return i;
}
