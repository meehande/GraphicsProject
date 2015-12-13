#pragma once
#include "maths_funcs.h"

class ModelView
{
public:
	vec3 Position;
	vec3 Rotation;
	float Velocity;
	float Scale;

	ModelView();
	ModelView(vec3, vec3, float, float);
	~ModelView();

	virtual mat4 CreateModelMatrix();
	virtual mat4 CreateViewMatrix();
	virtual void Forward();
	virtual void Backward();
	virtual void Left();
	virtual void Right();
};

