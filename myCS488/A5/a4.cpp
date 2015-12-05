#include <glm/ext.hpp>

#include "a4.hpp"
#include "extra.hpp"
#include "PhongMaterial.hpp"
#include "Texture.hpp"
#include "Scheduler.hpp"
#include "ImprovedPerlinNoise.hpp"
#include <math.h>
#include <iomanip>
#include <thread>

const float PI = 3.14159265f;
const int checkerBoardSize = 30;
const int maxDepth = 2;
const float boostEpsilon = 0.0001f;
int antiAliasingMode = 0;
int partitionValue = 2;
static Scheduler schedule;

glm::vec3 generateBackground(size_t x, size_t y)
{
  int value = int(x)/checkerBoardSize + int(y)/checkerBoardSize;
  
  if(value % 2 == 0)
    return glm::vec3(0.0f);
  else
    return glm::vec3(1.0f);
}

float computeFrenselCoefficient(glm::vec3 direction, float environmentFrom, float environmentTo, glm::vec3 outDirection)
{
  float cosThetaS = glm::dot(direction, -1.0f*outDirection);
  float densityRatio = environmentFrom/environmentTo;
  float costThetaTSquared = 1.0f-densityRatio*densityRatio*(1.0f - cosThetaS*cosThetaS); 
  if(costThetaTSquared < 0.0f)
    return 0.0f;
  

  //Calculate the frensel coefficient using Frensel's method
  float cosThetaT = sqrtf(costThetaTSquared);
  float Rs = (environmentFrom*cosThetaS - environmentTo*cosThetaT) / (environmentFrom*cosThetaS + environmentTo*cosThetaT);
  float Rp = (environmentFrom*cosThetaT - environmentTo*cosThetaS) / (environmentFrom*cosThetaT + environmentTo*cosThetaS);

  return 0.5f*(Rs*Rs+Rp*Rp);
}

Colour computeColour(Ray r, Intersection i, const std::list<Light*>& lights, const glm::vec3& ambient, SceneNode *root)
{
  Material *m = i.material;
  glm::vec3 diffuse = m->getColour(glm::vec3(i.point.x, i.point.y, i.point.z));// pm->m_kd;
  glm::vec3 specular = m->getSpecular();
  Colour colour = glm::vec3(0.0f);
  
  for(Light * lightSource: lights)
  {
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

glm::vec3 computeRefractedRayDirection(float originalRefractionIndex, float newRefractionIndex, glm::vec3 rayDirection, glm::vec3 surfaceNormal)
{
  float n = originalRefractionIndex/newRefractionIndex;
  float c1 = -1.0f * glm::dot(surfaceNormal, rayDirection);
  float c2 = sqrt( 1-n*n*(1-c1*c1) );
  glm::vec3 refractedRay = n*rayDirection + (n*c1-c2)*surfaceNormal;
  refractedRay = glm::normalize(refractedRay);
  return refractedRay;
}

Colour rayTracerHelper(Ray &ray, int recursionDepth, bool primaryRay, float reflection, float environment, Intersection &i, SceneNode *root, float x, float y, const std::list<Light*>& lights, const glm::vec3& ambient)
{
  Colour colour(0.0f);
  //Past the recursion depth
  if(recursionDepth >= maxDepth)
    return colour;

  i = root->checkIntersection(ray);

  //Did the ray hit something?
  if(!i.hit)
  {
    //If this ray is the primary ray (ray coming from the eye) then generate the background colour
    if(primaryRay)
      colour = generateBackground(x,y);

    return colour;
  }

  colour = computeColour(ray, i, lights, ambient, root);
  //Calculate reflection colour
  Material *m = i.material;

  glm::vec3 rayDirection = glm::vec3(ray.rayDirection.x, ray.rayDirection.y, ray.rayDirection.z);
  glm::vec3 surfaceNormal = m->getNormal(glm::vec3(i.point.x, i.point.y, i.point.z) ,glm::vec3(i.normal.x, i.normal.y, i.normal.z));
  
  //Calculate the fresnel coefficient
  double fresnelCoefficient = 1.0f;// computeFrenselCoefficient(rayDirection, environment, m->getRefractivity(), surfaceNormal);
  
  //PhongMaterial *pm =(PhongMaterial *)m;
  double matReflection = m->getReflectivity(); // pm->m_reflectioness;
  float matRI = m->getRefractivity(); // pm->m_refractiveIndex;
  glm::vec3 diffuse = m->getColour(glm::vec3(i.point.x, i.point.y, i.point.z));
  
  
  if(matReflection > 0.0f && fresnelCoefficient > 0.0f)
  {
    glm::vec3 reflectDirection = rayDirection - surfaceNormal*2.0f*glm::dot(rayDirection,surfaceNormal);
    glm::vec3 reflectOrigin = glm::vec3(i.point.x,i.point.y,i.point.z) + reflectDirection*boostEpsilon;  //Give it a little boost
    Ray reflectedRay;
    reflectedRay.rayDirection = glm::vec4(reflectDirection.x,reflectDirection.y, reflectDirection.z, 0.0f);
    reflectedRay.rayPosition = glm::vec4(reflectOrigin.x,reflectOrigin.y,reflectOrigin.z, 1.0f);
    Intersection reflectedIntersection;
    Colour reflectedColour = rayTracerHelper(reflectedRay, recursionDepth+1, false, matReflection, matRI, reflectedIntersection, root, x,y, lights, ambient);
  
    if(reflectedIntersection.hit)
    {
      reflectedColour *= float(fresnelCoefficient)*matReflection;
      colour += glm::vec3(reflectedColour.x*diffuse.x, reflectedColour.y*diffuse.y, reflectedColour.z*diffuse.z);    
    }  
  }

  //This will never get called
  if(false && matRI > 0.0 && checkTotalInternalReflection(environment, matRI, rayDirection, surfaceNormal))
  {
    glm::vec3 refractionDirection = glm::refract(rayDirection, surfaceNormal, environment/matRI);
    glm::vec3 refractOrigin = glm::vec3(i.point.x,i.point.y,i.point.z)+refractionDirection*boostEpsilon;
    Ray refractedRay;
    refractedRay.rayDirection = glm::vec4(refractionDirection, 0.0f);
    refractedRay.rayPosition = glm::vec4(refractOrigin, 1.0f);
    Intersection refractedIntersection;
    Colour refractedColour = rayTracerHelper(refractedRay, recursionDepth+1, false, matReflection, matRI, refractedIntersection, root, x,y, lights, ambient);
    colour += refractedColour*float(1.0-fresnelCoefficient);
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

int maxSamplePoints = 1000;
std::vector<SamplePoissonPoint> v;

float getRandomFloat()
{
  float low = 0.0, high = 1.0;
  float r = low + static_cast<float>(rand()) / static_cast<float>(RAND_MAX/high-low);
  return r;
}

void generateSamplePoints()
{
    int number=0;
    float radius = 0.4;
    while(number < maxSamplePoints)
    {
      bool ext=false;
      SamplePoissonPoint spp;
      glm::vec2 p1 = glm::vec2(getRandomFloat(),getRandomFloat());
      glm::vec2 p2;
      while(!ext)
      {
        p2 = glm::vec2(getRandomFloat(), getRandomFloat());
        if(glm::distance(p1,p2) > radius)
          ext = true;
      }
      glm::vec2 p3;
      ext = false;
      while(!ext)
      {
        p3 = glm::vec2(getRandomFloat(), getRandomFloat());
        if(glm::distance(p1,p3) > radius && glm::distance(p2,p3) > radius)
          ext = true;
      }
      spp.p1 = p1;
      spp.p2 = p2;
      spp.p3 = p3;

      v.push_back(spp);
      number++;
    }
}

glm::vec3 computeRayDirection(float x, float y, int w, int h, 
          float fovRadians, double aspectRatio, 
          glm::vec3 nSide, glm::vec3 nUp, glm::vec3 nView)
{
  glm::vec3 dir = nView + 
                      (x / (double(w)) * 2 - 1) * tan(fovRadians) * aspectRatio * nSide + 
                      (y / (double(h)) * 2 - 1) * tan(fovRadians) * -1 * nUp;
  return glm::normalize(dir);
}

void noAntiAliasing(int w, int h, 
          float fovRadians, double aspectRatio, 
          glm::vec3 nSide, glm::vec3 nUp, glm::vec3 nView, glm::vec3 eye, 
          SceneNode *root, 
          const std::list<Light*>& lights, const glm::vec3& ambient,int x,int y)
{
  glm::vec3 nDir = computeRayDirection(x,y,w,h,fovRadians,aspectRatio,nSide,nUp,nView);  //glm::normalize(dir);
      
  Ray r;
  r.rayDirection = glm::vec4(nDir.x,nDir.y,nDir.z,0);
  r.rayPosition = glm::vec4(eye.x,eye.y,eye.z,1);
        
  Intersection i;
  Colour colour = rayTracerHelper(r, 0, true, 1.0f, 1.0f, i, root, x, y, lights, ambient);
  schedule.updatePixel(x, y , colour);
}

void poissonDiskAntiAliasing(int w, int h, 
          float fovRadians, double aspectRatio, 
          glm::vec3 nSide, glm::vec3 nUp, glm::vec3 nView, glm::vec3 eye, 
          SceneNode *root, 
          const std::list<Light*>& lights, const glm::vec3& ambient,int x,int y)
{

      Colour masterColour = glm::vec3(0.0f);
      int rIndex = rand()%maxSamplePoints;
      SamplePoissonPoint spp = v.at(rIndex);
      int maxAlias = 3;
      for(int index=0; index<maxAlias; index++)
      {
        glm::vec2 randOffset;
        switch(index)
        {
          case 0:
            randOffset = spp.p1;
            break;
          case 1:
            randOffset = spp.p2;
            break;
          case 2:
            randOffset = spp.p3;
            break;
        }
        
        float newX = float(x)+randOffset.x;
        float newY = float(y)+randOffset.y;
        glm::vec3 nDir = computeRayDirection(newX,newY,w,h,fovRadians,aspectRatio,nSide,nUp,nView);  //glm::normalize(dir);
      
        Ray r;
        r.rayDirection = glm::vec4(nDir.x,nDir.y,nDir.z,0);
        r.rayPosition = glm::vec4(eye.x,eye.y,eye.z,1);
        
        Intersection i;
        Colour colour = rayTracerHelper(r, 0, true, 1.0f, 1.0f, i, root, newX, newY, lights, ambient);
        masterColour += colour;
      }

      masterColour = masterColour*float(1.0/3.0);
      schedule.updatePixel(x, y , masterColour);
}

void superSamplingAntiAliasing(int w, int h, 
          float fovRadians, double aspectRatio, 
          glm::vec3 nSide, glm::vec3 nUp, glm::vec3 nView, glm::vec3 eye, 
          SceneNode *root, 
          const std::list<Light*>& lights, const glm::vec3& ambient,int x,int y, int partition)
{
  float epsilon = 0.5f;
  Colour masterColour = glm::vec3(0.0f);
  
  for(int g=0; g<partition; g++)
  {
    for(int s=0; s<partition; s++)
    {
      float newX = float(x)+(g+epsilon)/float(partition);
      float newY = float(y)+(s+epsilon)/float(partition);
      glm::vec3 nDir = computeRayDirection(newX,newY,w,h,fovRadians,aspectRatio,nSide,nUp,nView);
      Ray r;
      r.rayDirection = glm::vec4(nDir.x,nDir.y,nDir.z,0);
      r.rayPosition = glm::vec4(eye.x,eye.y,eye.z,1);
        
      Intersection i;
      Colour colour = rayTracerHelper(r, 0, true, 1.0f, 1.0f, i, root, newX, newY, lights, ambient);
      masterColour += colour;
    }
  } 
  masterColour = masterColour/float(partition*partition);
  
  schedule.updatePixel(x, y , masterColour); 
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
    
    if(current.x != x/w && false)
    {
      current = schedule.getNextSchedule();
      continue;
    }
    
    for(int y=0; y<h; y++)
    {
      
      if(y != y/h && false)
      {
        current = schedule.getNextSchedule();
        continue;
      }
      
      if(antiAliasingMode == 0)
        noAntiAliasing(w,h,fovRadians, aspectRatio, nSide, nUp, nView, eye, root, lights, ambient, x, y);
      else if(antiAliasingMode == 1)
        poissonDiskAntiAliasing(w,h,fovRadians, aspectRatio, nSide, nUp, nView, eye, root, lights, ambient, x, y);
      else if(antiAliasingMode == 2)
        superSamplingAntiAliasing(w,h,fovRadians, aspectRatio, nSide, nUp, nView, eye, root, lights, ambient, x, y,partitionValue);
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
	const std::list<Light*>& lights,
  int aaMode, int partition,  int threadCount
)
{
  srand(time(NULL));

  antiAliasingMode = aaMode;
  partitionValue = partition;
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

  if(antiAliasingMode == 1)
    generateSamplePoints();

  int threadSize = threadCount;
  std::thread threads[threadSize];
  for(int i=0; i<threadSize; i++)
  {
    threads[i] = std::thread(runRayTraceThread, w,h,fovRadians, aspectRatio, nSide, nUp, nView, eye, root, lights, ambient);
  }

  for(int i=0; i<threadSize; i++)
    threads[i].join();
    
  image = schedule.rayCastedImage;
    
  //std::cout << std::fixed << std::setprecision(1) << 100.0f*double(y)/double(h) << " completed ..." << std::endl;
  //std::cout << "finished " << y << " out of " << h << std::endl;
  //std::cout << "finished " << h << " out of " << h << std::endl;
  //std::cout << std::fixed << std::setprecision(1) << 100.0f << " completed ..." << std::endl;
  //std::cout << largest_value << std::endl;
}

