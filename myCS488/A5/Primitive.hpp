#pragma once

#include <glm/glm.hpp>
#include "extra.hpp"

#define MAX_DISTANCE_TO_INTERSECTON FLT_MAX
#define FLOAT_ZERO 0.000001f

class Primitive {
public:
  virtual ~Primitive();
  //Declare a pure virtual function, which will be defined by the inherited class
  virtual Intersection checkIntersection(Ray r, bool skipBound) = 0;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual Intersection checkIntersection(Ray r, bool skipBound);
  double m_radius;
private:
  glm::vec3 m_pos;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
  virtual Intersection checkIntersection(Ray r, bool skipBound);
private:
  glm::vec3 m_pos;
  double m_size;
  bool checkIntersectionHelper(double rayPosition, double rayDirection, double cubePosition, Intersection &i, int dimension, double &minNear, double &maxFar);
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();

  virtual Intersection checkIntersection(Ray r, bool skipBound);
  NonhierSphere* nhSphere;
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();

  virtual Intersection checkIntersection(Ray r, bool skipBound);
  NonhierBox* nhBox;
};

//New primitives added
class Torus : public Primitive {
public:
  Torus(const glm::vec3& pos, const glm::vec3& axis, double centreTocentreHole, double tubeRadius)
    : m_pos(pos), m_axis(axis), m_radius(centreTocentreHole), m_tubeRadius(tubeRadius)
  {
  }

  virtual ~Torus();
  void stuff(Ray r, bool skipBound);
  virtual Intersection checkIntersection(Ray r, bool skipBound);

  //A is the distance from the middle of the torus to the middle of the tube
  double m_radius;
  //B is raidus of the tube
  double m_tubeRadius;

  double m_innerRadius;
  double m_outerRadius;
private:
  glm::vec3 m_pos;
  glm::vec3 m_axis;
};

class Cylinder : public Primitive {
public:
  Cylinder(const glm::vec3& bottomCenter, double height, double radius)
    :m_bottomCenter(bottomCenter), m_height(height), m_radius(radius)
  {
    m_topCenter = glm::vec3(bottomCenter.x,bottomCenter.y+float(m_height), bottomCenter.z);
    m_upAxis = glm::normalize(m_topCenter-m_bottomCenter);
  }

  virtual ~Cylinder();
  virtual Intersection checkIntersection(Ray r, bool skipBound);

  glm::vec3 m_pos;
  glm::vec3 m_topCenter;
  glm::vec3 m_bottomCenter;
  glm::vec3 m_upAxis;
  double m_radius;
  double m_height;
};

class Cone : public Primitive
{
public:
  Cone(const glm::vec3& bot, const glm::vec3& upAxis, double alpha, double height)
    : m_botCenter(bot), m_Axis(glm::normalize(upAxis)), m_alpha(alpha), m_height(height), m_radius(tan(alpha))
    {
    }

    virtual ~Cone();
    virtual Intersection checkIntersection(Ray r, bool skipBound);

private:
  glm::vec3 m_topCenter;
  glm::vec3 m_botCenter;
  double m_radius;
  double m_alpha;
  double m_height;
  glm::vec3 m_Axis;
};
