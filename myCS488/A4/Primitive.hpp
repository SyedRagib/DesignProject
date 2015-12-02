#pragma once

#include <glm/glm.hpp>
#include "extra.hpp"

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
