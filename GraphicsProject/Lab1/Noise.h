#pragma once
#include <vector>

class Noise
{
public:
	static double ConstantPi;
	static double ConstantE;

	static double Sample(double, double);
	static double InterpolateSample(double, double);
	static double Interpolate(double, double, double);
	static std::vector<float> Generate(int, int, int, int, double, double, int);
};
