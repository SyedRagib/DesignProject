#include "PerlinNoise.hpp"

#include <stdlib.h>
#include <time.h>     
#include <math.h>

PerlinNoise::PerlinNoise()
{
}

void PerlinNoise::generateNoise()
{
	srand(time(NULL));
	for(int x=0; x<noiseWidth; x++)
		for(int y=0; y<noiseHeight; y++)
			noise[x][y] = (rand() % 32768) / 32768.0;
}

double PerlinNoise::smoothNoise(double x, double y)
{
	double fractY = y - int(y);
	double fractX = x - int(x);
	
   int x1 = (int(x) + noiseWidth) % noiseWidth;
   int y1 = (int(y) + noiseHeight) % noiseHeight;

   int x2 = (x1 + noiseWidth - 1) % noiseWidth;
   int y2 = (y1 + noiseHeight - 1) % noiseHeight;

   double value = 0.0;
   value += fractX       * fractY       * noise[x1][y1];
   value += fractX       * (1 - fractY) * noise[x1][y2];
   value += (1 - fractX) * fractY       * noise[x2][y1];
   value += (1 - fractX) * (1 - fractY) * noise[x2][y2];

   return value;
}

double PerlinNoise::turbulence(double x, double y, double size)
{
	double value = 0.0, initialSize = size;
    
    while(size >= 1)
    {
        value += smoothNoise(x / size, y / size) * size;
        size /= 2.0;
    }
    
    //return value;
    return(value / (2*initialSize));
}

double PerlinNoise::generateMarble(int x, int y)
{
	double xPeriod = 5.0; //defines repetition of marble lines in x direction
    double yPeriod = 10.0; //defines repetition of marble lines in y direction
    //turbPower = 0 ==> it becomes a normal sine pattern
    double turbPower = 5.0; //makes twists
    double turbSize = 32.0; //initial size of the turbulence
	double xyValue = x * xPeriod / noiseHeight + y * yPeriod / noiseWidth + turbPower * turbulence(x, y, turbSize);
    double sineValue = fabs(sin(xyValue * 3.14159));

    return sineValue;
}
