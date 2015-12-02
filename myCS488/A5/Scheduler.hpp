#pragma once

#include "Image.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <mutex>

struct Pixel
{
	int x;
	int y;
	glm::vec3 colour;
};

typedef struct Pixel Pixel;

class Scheduler
{
private:
	std::mutex mtx; 
	
	int width;
	int height;
public:
	Image rayCastedImage;
	void setup(int width, int height);
	Scheduler();
	~Scheduler();
	void updatePixel(int x, int y,glm::vec3 c);
	glm::vec2 getNextScheduleHelper();
	glm::vec2 getNextSchedule();
};