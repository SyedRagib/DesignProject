#include "Scheduler.hpp"
#include <iostream>

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
	/*
	for(int i=0; i<width; i++)
		delete[] rayCasted[i];
	
	delete[] rayCasted;
	*/
}

void Scheduler::setup(int w, int h)
{
	width = w;
	height = h;

	rayCastedImage = Image(w, h, 3);

	for(int i=0; i<width; i++)
			rayCastedImage(i,0,0) = -2.0f;

	/*
	bool** tempBlob = new bool*[w];
	for(int i=0; i<w; i++)
	{
		tempBlob[i] = new bool[h];
	}

	rayCasted = tempBlob;

	for(int i=0; i<w; i++)
		for(int j=0; j<h; j++)
			rayCasted[i][j] = false;
	*/
}


void Scheduler::updatePixel(int x, int y, glm::vec3 c)
{
	rayCastedImage(x,y, 0) = c.x;
	rayCastedImage(x,y, 1) = c.y;
	rayCastedImage(x,y, 2) = c.z;
}

glm::vec2 Scheduler::getNextScheduleHelper()
{
	for(int i=0; i<width; i++)
			if(rayCastedImage(i,0,0) < -1.0f)
			{
				rayCastedImage(i, 0, 0) = 0.0f;
				//std::cout << "current ray tracing " << i << " , " << j << std::endl;
				return glm::vec2(i, 0);
			}

	return glm::vec2(-1.0f, -1.0f);
}

glm::vec2 Scheduler::getNextSchedule()
{
	mtx.lock();
	glm::vec2 rv = getNextScheduleHelper();
	mtx.unlock();
	return rv;
	

}