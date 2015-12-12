#include "Noise.h"
#include <cmath>

double Noise::ConstantPi = 3.14159265359;
double Noise::ConstantE = 2.718281828459045;

double Noise::Sample(double x, double y)
{
	int n = (int)x + (int)y * 57;
	n = (n << 13) ^ n;
	int nn = (n*(n*n * 60491 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0 - ((double)nn / 1073741824.0);
}

double Noise::Interpolate(double a, double b, double x)
{
	double ft = x * ConstantPi;
	double f = (1.0 - cos(ft)) * 0.5;
	return a * (1.0 - f) + b * f;
}

double Noise::InterpolateSample(double x, double y)
{
	double floorX = floor(x);
	double floorY = floor(y);
	double s = Sample(floorX, floorY);
	double t = Sample(floorX + 1, floorY);
	double u = Sample(floorX, floorY + 1);
	double v = Sample(floorX + 1, floorY + 1);
	double i1 = Interpolate(s, t, x - floorX);
	double i2 = Interpolate(u, v, x - floorX);
	return Interpolate(i1, i2, y - floorY);
}

std::vector<float> Noise::Generate(int i, int j, int Width, int Height, double zoom, double p, int octaves)
{
	std::vector<float> points = std::vector<float>((Width)* (Height), 0);

	for (int y = j; y < Height + j - 1; y++)
	{
		for (int x = i; x < Width + i - 1; x++)
		{
			double noise = 0;
			for (int a = 0; a < octaves; a++)
			{
				double freq = pow(2, a);
				double ampl = pow(p, a);
				noise += ampl * InterpolateSample((double)x * freq / zoom, (double)y * freq / zoom);
			}
			points[(x - i) + (y - j) * Width] = noise;
		}
	}
	return points;
}
