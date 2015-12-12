#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "maths_funcs.h"
#include "Shader.h"
#include <SOIL.h>

// GL Includes
//#include <GL/glew.h> // Contains all the necessery OpenGL includes

// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

using namespace std;


class Mesh {
public:
	int num_attribs; 
	std::vector<int>BufferIds;
	unsigned int mesh_vao;
	GLuint loc1, loc2, loc3;
	unsigned int texture;

	int g_point_count;
	std::vector<float> g_vp, g_vn, g_vt;

	Mesh();

	bool read_mesh (const char* file_name);
	void generateObjectBufferMesh(const char* name, Shader* meshShader);
	void loadTexture(const char* s);

	void bind();
	void draw();
	void unbind();
	void generateBuffers(int n);
	void fillBufferData(int, int, float*, int, int);

};