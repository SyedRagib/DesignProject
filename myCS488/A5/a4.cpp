#include <glm/ext.hpp>

#include "a4.hpp"
#include "extra.hpp"
#include "PhongMaterial.hpp"
#include "Texture.hpp"
#include "PerlinNoise.hpp"
#include "Scheduler.hpp"
#include "ImprovedPerlinNoise.hpp"
#include <math.h>
#include <iomanip>
#include <thread>

const float PI = 3.14159265f;
const int checkerBoardSize = 30;
const int maxDepth = 4;
const float reflectionEpsilon = 0.0001f;

static Scheduler schedule;

glm::vec3 generateBackground(size_t x, size_t y)
{
  int value = int(x)/checkerBoardSize + int(y)/checkerBoardSize;
  return glm::vec3(1.0f,0.0f,0.0f);

  if(value % 2 == 0)
    return glm::vec3(0.0f);
  else
    return glm::vec3(1.0f);
}

float computeFrenselCoefficient(glm::vec3 direction, float environmentFrom, float environmentTo, glm::vec3 outDirection, bool totalInternalRefraction)
{
  //Gotta change this
  float cosThetaS = glm::dot(direction, -1.0f*outDirection);
  float densityRatio = environmentFrom/environmentTo;
  float costThetaTSquared = 1.0f-densityRatio*densityRatio*(1.0f - cosThetaS*cosThetaS); 
  if(costThetaTSquared < 0.0f)
  {
    totalInternalRefraction = true;
    return 0.0f;
  }

  //Calculate the frensel coefficient using Frensel's method
  float cosThetaT = sqrtf(costThetaTSquared);
  float Rs = (environmentFrom*cosThetaS - environmentTo*cosThetaT) / (environmentFrom*cosThetaS + environmentTo*cosThetaT);
  float Rp = (environmentFrom*cosThetaT - environmentTo*cosThetaS) / (environmentFrom*cosThetaT + environmentTo*cosThetaS);

  return 0.5f*(Rs*Rs+Rp*Rp);
}

Colour computeColour(Ray r, Intersection i, const std::list<Light*>& lights, const glm::vec3& ambient, SceneNode *root)
{
  Material *m = i.material;
  //PhongMaterial *pm =(PhongMaterial *)m;
  glm::vec3 diffuse = m->getColour(glm::vec3(i.point.x, i.point.y, i.point.z));// pm->m_kd;
  glm::vec3 specular = m->getSpecular();
  Colour colour = glm::vec3(0.0f);
  
  for(Light * lightSource: lights)
  {
          
          //i.point += 1e-7*i.direction;

          glm::vec3 lightDirectionVect = lightSource->position - glm::vec3(i.point.x,i.point.y,i.point.z);
          lightDirectionVect = glm::normalize(lightDirectionVect);

          //Configure the ray before shadow test, ray is being fired from the primitive to each light source
          Ray secondaryRay;
          glm::vec3 lightDir = glm::vec3(i.point.x-lightSource->position.x,i.point.y-lightSource->position.y,i.point.z-lightSource->position.z);
          lightDir *= -1.0f;
          glm::vec3 nLightDir = glm::normalize(lightDir);
          secondaryRay.rayDirection = glm::vec4(nLightDir.x,nLightDir.y,nLightDir.z,0.0f);
          secondaryRay.rayPosition = i.point;

          //Shadow test
          double maxDistance = glm::length(lightDir);
          Intersection lightIntersected = root->checkIntersection(secondaryRay);
          glm::vec3 intersectedPoint = glm::vec3(lightIntersected.point.x,lightIntersected.point.y,lightIntersected.point.z);
          if(lightIntersected.hit && glm::length(intersectedPoint-lightSource->position) < maxDistance - 0.001)
            continue;
          
          //Compute the colour
          glm::vec3 intersectionNormal =m->getNormal(glm::vec3(i.point.x, i.point.y, i.point.z),glm::vec3(i.normal.x,i.normal.y,i.normal.z));
          double lambertion = glm::dot(intersectionNormal,lightDirectionVect);
          colour += lambertion*diffuse*lightSource->colour;
          
          //Compute shininess
          if(!false)
          {
            glm::vec3 reflectionVector = glm::vec3(-1.0f*lightDirectionVect + 2.0f*lambertion*intersectionNormal);
            double reflectionValue;
            reflectionValue = glm::dot(reflectionVector,glm::vec3(-1.0*r.rayDirection.x, -1.0*r.rayDirection.y, -1.0*r.rayDirection.z));
            if(reflectionValue < 0.0f)
              continue;

            double reflectionShininess = pow(reflectionValue,m->getShininess())*1.0f;
            colour += specular*reflectionShininess*lightSource->colour;
          }
          else
          {
            //Compute reflection using blinn-phong
            glm::vec3 view = -1.0f*glm::vec3(r.rayDirection.x,r.rayDirection.y,r.rayDirection.z);
            glm::vec3 halfway = 1.0f/(glm::length(view+lightDirectionVect))*(view+lightDirectionVect);
            colour += specular*pow(glm::dot(halfway,intersectionNormal), m->getShininess())*1.0f*lightSource->colour;
          }
  }


  return colour;
}

bool checkTotalInternalReflection(float originalRefractionIndex, float newRefractionIndex, glm::vec3 rayDirection, glm::vec3 surfaceNormal)
{
  float cl = originalRefractionIndex/newRefractionIndex;
  float theta = -1.0f * glm::dot(surfaceNormal, rayDirection);

  return ( (1.0f - cl * cl * (1.0f - theta * theta)) < 0.0f);
}

//https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
glm::vec3 computeRefractedRayDirection(float originalRefractionIndex, float newRefractionIndex, glm::vec3 rayDirection, glm::vec3 surfaceNormal)
{
  float n = originalRefractionIndex/newRefractionIndex;
  float c1 = -1.0f * glm::dot(surfaceNormal, rayDirection);
  float c2 = sqrt( 1-n*n*(1-c1*c1) );
  glm::vec3 refractedRay = n*rayDirection + (n*c1-c2)*surfaceNormal;
  refractedRay = glm::normalize(refractedRay);
  return refractedRay;
}

Colour rayTracerHelper(Ray &ray, int recursionDepth, bool primaryRay, float reflection, float environment, Intersection &i, SceneNode *root, size_t x, size_t y, const std::list<Light*>& lights, const glm::vec3& ambient)
{
  Colour colour(0.0f);
  //Past the recursion depth
  if(recursionDepth > maxDepth)
    return colour;

  i = root->checkIntersection(ray);

  //Did the ray hit something?
  if(!i.hit)
  {
    //If this ray is the primary ray (ray coming from the eye) then generate the background colour
    //if(primaryRay)
      colour = generateBackground(x,y);

    return colour;
  }

  colour = computeColour(ray, i, lights, ambient, root);
  //Calculate reflection colour
  Material *m = i.material;

  glm::vec3 rayDirection = glm::vec3(ray.rayDirection.x, ray.rayDirection.y, ray.rayDirection.z);
  glm::vec3 surfaceNormal = m->getNormal(glm::vec3(i.point.x, i.point.y, i.point.z) ,glm::vec3(i.normal.x, i.normal.y, i.normal.z));
  glm::vec3 outSurfaceNormal = surfaceNormal;
  float viewP = glm::dot(rayDirection,surfaceNormal); 
  
  //We're inside the object
  if(viewP > 0.0f)
    outSurfaceNormal *= -1.0f;

  //Calculate the fresnel coefficient
  bool totalInternalRefraction = false;
  float fresnelCoefficient = computeFrenselCoefficient(rayDirection, environment, m->getRefractivity(), outSurfaceNormal, totalInternalRefraction);

  //PhongMaterial *pm =(PhongMaterial *)m;
  double matReflection = m->getReflectivity(); // pm->m_reflectioness;
  float matRI = m->getRefractivity(); // pm->m_refractiveIndex;
  glm::vec3 diffuse = m->getColour(glm::vec3(i.point.x, i.point.y, i.point.z));
  if(matReflection > 0.0f && reflection > reflectionEpsilon)
  {
    glm::vec3 reflectDirection = rayDirection - outSurfaceNormal*2.0f*glm::dot(rayDirection,outSurfaceNormal);
    glm::vec3 reflectOrigin = glm::vec3(i.point.x,i.point.y,i.point.z) + reflectDirection*reflectionEpsilon;  //Give it a little boost
    Ray reflectedRay;
    reflectedRay.rayDirection = glm::vec4(reflectDirection.x,reflectDirection.y, reflectDirection.z, 0.0f);
    reflectedRay.rayPosition = glm::vec4(reflectOrigin.x,reflectOrigin.y,reflectOrigin.z, 1.0f);
    Intersection reflectedIntersection;
    Colour reflectedColour = rayTracerHelper(reflectedRay, recursionDepth+1, false, reflection*matReflection, matRI, reflectedIntersection, root, x,y, lights, ambient);
    reflectedColour *= reflection*matReflection*fresnelCoefficient;
    colour += glm::vec3(reflectedColour.x*diffuse.x, reflectedColour.y*diffuse.y, reflectedColour.z*diffuse.z);
  }
  
  if(matRI > 0.0f && !checkTotalInternalReflection( environment, matRI, rayDirection, outSurfaceNormal))
  {
    Ray refractedRay;
    glm::vec3 refractedDirection = computeRefractedRayDirection(environment, matRI, rayDirection, outSurfaceNormal);
    glm::vec3 refractedOrigin = glm::vec3(i.point.x, i.point.y, i.point.z) + refractedDirection*0.00001f;  //Give it a little boost
    Intersection refractedIntersection;

    refractedRay.rayDirection = glm::vec4(refractedDirection, 0.0f);
    refractedRay.rayPosition = glm::vec4(refractedOrigin, 1.0f);
    Colour refractedColour = rayTracerHelper( refractedRay, recursionDepth+1, false, reflection, matRI, refractedIntersection, root, x,y, lights, ambient);
    //colour += refractedColour;
    
    if (refractedIntersection.hit) {
        Material *refractedMaterial = i.material;
        //PhongMaterial *refractedPM =(PhongMaterial *)refractedMaterial;
        glm::vec3 refractedDiffuse = refractedMaterial->getColour(glm::vec3(0.0));
        Colour absorbance   = (refractedDiffuse) * 
                              0.15f * 
                              -1.0 *(glm::distance(glm::vec3(refractedIntersection.point.x, refractedIntersection.point.y, refractedIntersection.point.z), refractedOrigin));
        Colour transparency = Colour(expf(absorbance.x), expf(absorbance.y), expf(absorbance.z));
        colour += glm::vec3(refractedColour.x*transparency.x, refractedColour.y*transparency.y, refractedColour.z*transparency.z) * (1.0 - fresnelCoefficient);
    }
  }
  
  return colour;
}

glm::vec3 rayCast(
  // What to render
  SceneNode *root,

  // Image to write to, set to a given width and height
  Image& image,

  // Viewing parameters
  const glm::vec3& eye, const glm::vec3& view,
  const glm::vec3& up, double fov,

  // Lighting parameters
  const glm::vec3& ambient,
  const std::list<Light*>& lights,
  int recursion)
{
  return glm::vec3();
}


void runRayTraceThread(int w, int h, 
          float fovRadians, double aspectRatio, 
          glm::vec3 nSide, glm::vec3 nUp, glm::vec3 nView, glm::vec3 eye, 
          SceneNode *root, 
          const std::list<Light*>& lights, const glm::vec3& ambient)
{
  glm::vec2 current = schedule.getNextSchedule();
  
  while(current.x >= 0 && current.y >= 0)
  {
    int x = current.x;
    for(int y=0; y<h; y++)
    {
      glm::vec3 dir = nView + 
                      (x / (double(w)) * 2 - 1) * tan(fovRadians) * aspectRatio * nSide + 
                      (y / (double(h)) * 2 - 1) * tan(fovRadians) * -1 * nUp;
      glm::vec3 nDir = glm::normalize(dir);
      Ray r;
      r.rayDirection = glm::vec4(nDir.x,nDir.y,nDir.z,0);
      r.rayPosition = glm::vec4(eye.x,eye.y,eye.z,1);
        
      Intersection i;
      Colour colour = rayTracerHelper(r, 0, true, 1.0f, 1.0f, i, root, x, y, lights, ambient);
      schedule.updatePixel(x, y , colour);
    }

    current = schedule.getNextSchedule();
  }
}

void a4_render(
	// What to render
	SceneNode *root,

	// Image to write to, set to a given width and height
	Image& image,

	// Viewing parameters
	const glm::vec3& eye, const glm::vec3& view,
	const glm::vec3& up, double fov,

	// Lighting parameters
	const glm::vec3& ambient,
	const std::list<Light*>& lights
)
{
  // Fill in raytracing code here.
  size_t h = image.height();
  size_t w = image.width();

  // Setup scheduler
  schedule.setup(w, h);

  //Normalize the vectors
  glm::vec3 nView, nUp, nSide;  
  nView = glm::normalize(view);
  nUp = glm::normalize(up);
  nSide = glm::normalize(glm::cross(view,up));

  float fovRadians = fov*PI/360.0;
  double aspectRatio = (double(w)/double(h));
  bool blinnPhong = true;
  /*
  ImprovedPerlinNoise imp;
  for (size_t y = 0; y < h; y++) {
      for (size_t x = 0; x < w; x++) {
          double m_scale = 0.5;
          double i = x * m_scale;
          double j = y * m_scale;
         double noiseCoef = 0;

         double alpha = 0.05;
         for (int level = 1; level < 10; level++) {
            noiseCoef +=  (1.0f / level) * fabsf(imp.noise(
               level * alpha * i,
               level *  (alpha+0.10) * j,
               0
            ));
         }

        image(x,y,0) = image(x,y,1)= image(x,y,2) = noiseCoef;
      }
    }
    */
  bool multiThread = true;
  
  if(multiThread)
  {
    int threadSize = 10;
    std::thread threads[threadSize];
    for(int i=0; i<threadSize; i++)
    {
      threads[i] = std::thread(runRayTraceThread, w,h,fovRadians, aspectRatio, nSide, nUp, nView, eye, root, lights, ambient);
    }

    for(int i=0; i<threadSize; i++)
      threads[i].join();
    
    image = schedule.rayCastedImage;
  }
  else
  {
    for (size_t y = 0; y < h; ++y) {
      for (size_t x = 0; x < w; ++x) {
        //Calculate the direction that the ray is firing towards
        glm::vec3 dir = nView + 
                        (x / (double(w)) * 2 - 1) * tan(fovRadians) * aspectRatio * nSide + 
                        (y / (double(h)) * 2 - 1) * tan(fovRadians) * -1 * nUp;
        glm::vec3 nDir = glm::normalize(dir);
        Ray r;
        r.rayDirection = glm::vec4(nDir.x,nDir.y,nDir.z,0);
        r.rayPosition = glm::vec4(eye.x,eye.y,eye.z,1);
        

        Intersection i;
        Colour colour = rayTracerHelper(r, 0, true, 1.0f, 1.0f, i, root, x, y, lights, ambient);
        
        image(x,y,0) = colour.x;
        image(x,y,1) = colour.y;
        image(x,y,2) = colour.z;
      }
    }
  }
    
  //std::cout << std::fixed << std::setprecision(1) << 100.0f*double(y)/double(h) << " completed ..." << std::endl;
  //std::cout << "finished " << y << " out of " << h << std::endl;
  //std::cout << "finished " << h << " out of " << h << std::endl;
  //std::cout << std::fixed << std::setprecision(1) << 100.0f << " completed ..." << std::endl;
  //std::cout << largest_value << std::endl;
}

