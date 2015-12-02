#include <glm/ext.hpp>

#include "a4.hpp"
#include "extra.hpp"
#include "PhongMaterial.hpp"
#include <math.h>
#include <iomanip>

const float PI = 3.14159265f;
const int checkerBoardSize = 30;

glm::vec3 generateBackground(size_t x, size_t y)
{
  int value = int(x)/checkerBoardSize + int(y)/checkerBoardSize;

  if(value % 2 == 0)
    return glm::vec3(0.0f);
  else
    return glm::vec3(1.0f);
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
  /*
  std::cerr << "Stub: a4_render(" << root << ",\n     "
            << "image(" << image.width() << ", " 
						<< image.height() << "),\n     "
            << glm::to_string(eye) << ", " 
			<< glm::to_string(view) << ", " 
			<< glm::to_string(up) << ", " << fov << ",\n     "
            << glm::to_string(ambient) << ",\n     {";

  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << "});" << std::endl;
  */
  size_t h = image.height();
  size_t w = image.width();

  //Normalize the vectors
  glm::vec3 nView, nUp, nSide;  
  nView = glm::normalize(view);
  nUp = glm::normalize(up);
  nSide = glm::normalize(glm::cross(view,up));

  float fovRadians = fov*PI/360.0;
  double aspectRatio = (double(w)/double(h));
  bool blinnPhong = true;

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
      Intersection i = root->checkIntersection(r);

      if(i.hit)
      {
        Material *m = i.material;
        PhongMaterial *pm =(PhongMaterial *)m;
        glm::vec3 diffuse = pm->m_kd;
        glm::vec3 specular = pm->m_ks;
        Colour colour = glm::vec3(0.0f);
        //Since the ray hit a primitive, need to calculate light for each light source
        for(Light * lightSource: lights)
        {
          
          //i.point += 1e-7*i.direction;

          glm::vec3 lightDirectionVect = lightSource->position - glm::vec3(i.point.x,i.point.y,i.point.z);
          lightDirectionVect = glm::normalize(lightDirectionVect);

          //Configure the ray before shadow test, ray is being fired from the primitive to each light source
          if(x == h*3/4 && y==192)
            r.returnOutput = true;
          else
            r.returnOutput = false;
          
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
          glm::vec3 intersectionNormal = glm::vec3(i.normal.x,i.normal.y,i.normal.z);
          double lambertion = glm::dot(intersectionNormal,lightDirectionVect);
          colour += lambertion*diffuse*lightSource->colour;
          
          //Compute reflection
          if(!blinnPhong)
          {
            glm::vec3 reflectionVector = glm::vec3(-1.0f*lightDirectionVect + 2.0f*lambertion*intersectionNormal);
            double reflectionValue;
            reflectionValue = glm::dot(reflectionVector,glm::vec3(-1.0*r.rayDirection.x, -1.0*r.rayDirection.y, -1.0*r.rayDirection.z));
            if(reflectionValue < 0.0f)
              continue;

            double reflectionShininess = pow(reflectionValue,pm->m_shininess)*1.0f;
            colour += specular*reflectionShininess*lightSource->colour;
          }
          else
          {
            //Compute reflection using blinn-phong
            glm::vec3 view = -1.0f*glm::vec3(r.rayDirection.x,r.rayDirection.y,r.rayDirection.z);
            glm::vec3 halfway = 1.0f/(glm::length(view+lightDirectionVect))*(view+lightDirectionVect);
            colour += specular*pow(glm::dot(halfway,intersectionNormal), pm->m_shininess)*1.0f*lightSource->colour;
          }
        }

          //Compute ambitent lighting
          colour.x += diffuse.x*ambient.x;
          colour.y += diffuse.y*ambient.y;
          colour.z += diffuse.z*ambient.z;

          image(x,y,0) = colour.x;
          image(x,y,1) = colour.y;
          image(x,y,2) = colour.z; 

      }
      else
      {
        /*
          // Red: increasing from top to bottom
        image(x, y, 0) = (double)y / h;
        // Green: increasing from left to right
        image(x, y, 1) = (double)x / w;
        // Blue: in lower-left and upper-right corners
        image(x, y, 2) = ((y < h/2 && x < w/2)
                        || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;  
        */
        glm::vec3 bgColour =  generateBackground(x,y);
        image(x, y, 0) = bgColour.x;
        image(x, y, 1) = bgColour.y;
        image(x, y, 2) = bgColour.z;
      }
      
    }
    std::cout << std::fixed << std::setprecision(1) << 100.0f*double(y)/double(h) << " completed ..." << std::endl;
  }

  std::cout << std::fixed << std::setprecision(1) << 100.0f << " completed ..." << std::endl;
}

