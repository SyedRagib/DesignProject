#pragma once

#define noiseHeight 128
#define noiseWidth 128

class PerlinNoise
{
public:
	double noise[noiseWidth][noiseHeight];
	PerlinNoise();
	void generateNoise();
	double smoothNoise(double x, double y);
	double turbulence(double x, double y, double size);
	double generateMarble(int x, int y);
};