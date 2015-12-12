#pragma once
#include"Shader.h"

class SkyBox{
private: 
	Shader* shader;
	void init();
	GLuint skyboxVAO;
	GLuint cubemapTexture;

public:
	mat4* view;
	mat4* persp;
	SkyBox(mat4* v, mat4* p);
	void bind();
	void unbind();
	void draw();


};