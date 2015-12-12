#include "Terrain.h"
#include "Noise.h"
#include <GL\glew.h>

Terrain::Terrain() : Mesh()
{

}

Terrain::Terrain(int x, int y, int width, int height, float widthSpread, float heightSpread, float zoom, float peristance, int octaves) : Mesh()
{
	this->Width = width;
	this->Height = height;
	this->WidthHeight = this->Width * this->Height;
	this->WidthSpread = widthSpread;
	this->HeightSpread = heightSpread;
	this->fHeights = Noise::Generate(x, y, width, height, zoom, peristance, octaves);
	this->ModifyHeights();
	this->Init();
}

Terrain::~Terrain()
{

}

vec3 Terrain::CalculateNormal(int x, int y)
{
	float left = GetHeight(x - 1, y);
	float right = GetHeight(x + 1, y);
	float down = GetHeight(x, y - 1);
	float up = GetHeight(x, y + 1);

	return vec3::normalise(vec3(left - right, 2.0f, down - up));
}

float Terrain::GetHeight(int x, int y)
{
	if (x < 0)
		return GetHeight(x + 1, y);
	else if (y < 0)
		return GetHeight(x, y + 1);
	else if (x >= this->Width)
		return GetHeight(x - 1, y);
	else if (y >= this->Width)
		return GetHeight(x, y - 1);
	else
		return this->fHeights[x + y * this->Width];
}

void Terrain::ModifyHeights()
{
	for (int i = 0; i < this->fHeights.size(); i++)
	{
		this->fHeights[i] = this->HeightSpread * pow(Noise::ConstantE, this->fHeights[i]);
	}
}

void Terrain::Init()
{
	std::vector<float> fPositions = std::vector<float>(3 * this->WidthHeight);
	std::vector<float> fNormals = std::vector<float>(3 * this->WidthHeight);
	std::vector<float> fTextures = std::vector<float>(2 * this->WidthHeight);

	int iIndicesSize = 6 * (this->Width - 1) * (this->Height - 1);
	std::vector<int> iIndices = std::vector<int>(iIndicesSize);

	for (int j = 0; j < this->Height; j++)
	{
		for (int i = 0; i < this->Width; i++)
		{
			fTextures[2 * (i + j * this->Width)] = (float)i / (float)(this->Width - 1);
			fTextures[2 * (i + j * this->Width) + 1] = (float)j / (float)(this->Height - 1);
			fPositions[3 * (i + j * this->Width)] = i * this->WidthSpread;
			fPositions[3 * (i + j * this->Width) + 1] = this->GetHeight(i, j);
			fPositions[3 * (i + j * this->Width) + 2] = j * this->WidthSpread;
			vec3 normal = this->CalculateNormal(i, j);
			fNormals[3 * (i + j * this->Width) + 1] = normal.v[0];
			fNormals[3 * (i + j * this->Width) + 1] = normal.v[1];
			fNormals[3 * (i + j * this->Width) + 1] = normal.v[2];
		}
	}

	int pointer = 0;
	int topLeft, topRight, bottomLeft, bottomRight;
	for (int j = 0; j < this->Height - 1; j++)
	{
		for (int i = 0; i < this->Width - 1; i++)
		{
			topLeft = j * this->Width + i;
			topRight = topLeft + 1;
			bottomLeft = (j + 1) * this->Width + i;
			bottomRight = bottomLeft + 1;
			iIndices[pointer++] = topLeft;
			iIndices[pointer++] = bottomLeft;
			iIndices[pointer++] = topRight;
			iIndices[pointer++] = topRight;
			iIndices[pointer++] = bottomLeft;
			iIndices[pointer++] = bottomRight;
		}
	}

	this->generateBuffers(3);
	this->fillBufferData(0, 3, fPositions.data(), this->WidthHeight, GL_STATIC_DRAW);
	this->fillBufferData(1, 3, fNormals.data(), this->WidthHeight, GL_STATIC_DRAW);
	this->fillBufferData(2, 2, fTextures.data(), this->WidthHeight, GL_STATIC_DRAW);

	this->IndexBuffer = new Buffer(GL_ELEMENT_ARRAY_BUFFER);
	this->IndexBuffer->BufferData(iIndices.data(), iIndicesSize, 4, GL_STATIC_DRAW);
}

void Terrain::Render(int i)
{
	this->IndexBuffer->Bind();;
	glDrawElements(i, this->IndexBuffer->Count, GL_UNSIGNED_INT, 0);
	this->IndexBuffer->Unbind();
}

float BarryCentric(vec3 p1, vec3 p2, vec3 p3, vec2 pos)
{
	float det = (p2.v[2] - p3.v[2]) * (p1.v[0] - p3.v[0]) + (p3.v[0] - p2.v[0]) * (p1.v[2] - p3.v[2]);
	float l1 = ((p2.v[2] - p3.v[2]) * (pos.v[0] - p3.v[0]) + (p3.v[0] - p2.v[0]) * (pos.v[1] - p3.v[2])) / det;
	float l2 = ((p3.v[2] - p1.v[2]) * (pos.v[0] - p3.v[0]) + (p1.v[0] - p3.v[0]) * (pos.v[1] - p3.v[2])) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.v[1] + l2 * p2.v[1] + l3 * p3.v[1];
}

float Terrain::GetHeight(vec3 pos, vec3 terrainPos)
{
	float tX = pos.v[0] - terrainPos.v[0];
	float tZ = pos.v[2] - terrainPos.v[1];
	int gX = (int)floor(tX / this->WidthSpread);
	int gZ = (int)floor(tZ / this->WidthSpread);

	if (gX < 0 || gZ < 0 || gX >= this->Width - 1 || gZ >= this->Width - 1)
		return 0;

	float xCoord = ((int)tX % (int)this->WidthSpread) / this->WidthSpread;
	float zCoord = ((int)tZ % (int)this->WidthSpread) / this->WidthSpread;
	float result;
	if (xCoord <= (1 - zCoord))
	{
		result = BarryCentric(
			vec3(0,fHeights[gX + Width * gZ], 0),
			vec3(1,fHeights[gX + 1 + Width * gZ], 0),
			vec3(0,fHeights[gX + Width * (gZ + 1)], 1),
			vec2(xCoord, zCoord));
	}
	else
	{
		result = BarryCentric(
			vec3(1,fHeights[gX + 1 + Width * gZ], 0),
			vec3(1,fHeights[gX + 1 + Width * (gZ + 1)], 1),
			vec3(0,fHeights[gX + Width * (gZ + 1)], 1),
			vec2(xCoord, zCoord));
	}
	return result + terrainPos.v[1];
}