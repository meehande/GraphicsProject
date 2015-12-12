#pragma once
#include "Mesh.h"
#include "maths_funcs.h"
#include "Buffer.h"
class Terrain :
	public Mesh
{
private:
	std::vector<float> fHeights;

	Buffer* IndexBuffer;
	int IndexId, IndexCount;

	int Width, Height, WidthHeight;
	float WidthSpread, HeightSpread;

	void Init();
	void ModifyHeights();
	vec3 CalculateNormal(int, int);
	float GetHeight(int, int);
public:
	Terrain();
	Terrain(int, int, int, int, float, float, float, float, int);
	~Terrain();

	void Render(int);
	float GetHeight(vec3, vec3);
};

