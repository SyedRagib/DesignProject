#pragma once

class ImprovedPerlinNoise
{
public:
  ImprovedPerlinNoise();
  double noise(double x, double y, double z);
  double perlin(double x, double y, double z);
private:
  int p[512];
  
  double fade(double t);
  double lerp(double t, double a, double b);
  double grad(int hash, double x, double y, double z);
};