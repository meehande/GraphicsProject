#include "ModelView.h"
#include "Noise.h"
//#include <cmath>

ModelView::ModelView()
{

}

ModelView::ModelView(vec3 p, vec3 r, float v, float s)
{
	this->Position = p;
	this->Rotation = r;
	this->Velocity = v;
	this->Scale = s;
}

ModelView::~ModelView()
{

}

mat4 ModelView::CreateModelMatrix()
{
	return mat4::rotate_deg(mat4::translate(mat4::scale(identity_mat4(), this->Scale), this->Position), this->Rotation);
}

mat4 ModelView::CreateViewMatrix()
{	
	return mat4::rotate_deg(mat4::translate(identity_mat4(), vec3::negate(this->Position)), this->Rotation);
}

void ModelView::Forward()
{
	this->Position.v[0] += this->Velocity * sin(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
	this->Position.v[2] -= this->Velocity * cos(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
}

void ModelView::Backward()
{
	this->Position.v[0] -= this->Velocity * sin(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
	this->Position.v[2] += this->Velocity * cos(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
}

void ModelView::Left()
{
	this->Position.v[0] -= this->Velocity * cos(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
	this->Position.v[2] -= this->Velocity * sin(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
}

void ModelView::Right()
{
	this->Position.v[0] += this->Velocity * cos(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
	this->Position.v[2] += this->Velocity * sin(this->Rotation.v[1] / 360.0f * Noise::ConstantPi * 2);
}