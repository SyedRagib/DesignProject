#include "Scheduler.hpp"
#include <iostream>
#include <iomanip>

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

void Scheduler::setup(int w, int h)
{
	width = w;
	height = h;

	rayCastedImage = Image(w, h, 3);

	for(int i=0; i<width; i++)
			rayCastedImage(i,0,0) = -2.0f;
}


void Scheduler::updatePixel(int x, int y, glm::vec3 c)
{
	rayCastedImage(x,y, 0) = c.x;
	rayCastedImage(x,y, 1) = c.y;
	rayCastedImage(x,y, 2) = c.z;

	mtx.lock();
		if(y == (height -1))
		{
			progress++;
			std::cout << std::fixed << std::setprecision(1) << 100.0f*double(progress)/double(width) << " completed ..." << std::endl;
		}
	mtx.unlock();
}

glm::vec2 Scheduler::getNextScheduleHelper()
{
	for(int i=0; i<width; i++)
			if(rayCastedImage(i,0,0) < -1.0f)
			{
				rayCastedImage(i, 0, 0) = 0.0f;
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