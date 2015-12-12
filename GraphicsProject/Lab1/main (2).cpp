
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "maths_funcs.h"

#include <SOIL.h>

// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

#include "Mesh.h"
#include "Shader.h"

// VARIABLES
Shader* meshShader;
Shader* monkeyShader;

Mesh* mountain_mesh;
Mesh* monkey_mesh;

std::vector<float> g_vp, g_vn, g_vt;
int g_point_count = 0;


// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;


int width = 800;
int height = 600;
GLfloat rotate_y = 0.0f;
GLfloat translate_view_x = 0.0;
GLfloat translate_view_y = 0.0;
GLfloat translate_view_z = 0.0;
// Light attributes
vec3 lightPos(3.0f, 1.0f, 0.0f);


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable (GL_DEPTH_TEST); // enable depth-testing	
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	
	//MONKEY HEAD//
	meshShader->use();
	mountain_mesh->bind();
	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation (meshShader->Program, "mModelMatrix");
	int view_mat_location = glGetUniformLocation (meshShader->Program, "mViewMatrix");
	int proj_mat_location = glGetUniformLocation (meshShader->Program, "mProjectionMatrix");
	GLint lightPosLoc = glGetUniformLocation(meshShader->Program, "vLightPosition");
    GLint lightColorLoc  = glGetUniformLocation(meshShader->Program, "vLightColour");
	glUniform3f(lightPosLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
	// Root of the Hierarchy
	//view_trans = vec4(0.0, -30.0f, 0.0f,0.0f),
	mat4 view = identity_mat4 ();
	//view = translate(view,vec3(0.0, -30.0f, 0.0f) );
	mat4 persp_proj = perspective(60.0, (float)width/(float)height, 0.1, -10.0);
	mat4 model = identity_mat4 ();
	view = translate (view, vec3 (0.0, -20.0f, 183.0f));
	view = translate(view, vec3(translate_view_x, translate_view_y, translate_view_z));
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	mountain_mesh->draw();
	mountain_mesh->unbind();

	monkey_mesh->bind();
	model = rotate_x_deg(model, 90.0);
	model = rotate_z_deg(model, 180.0);
	model = rotate_y_deg (model, rotate_y); 
	model = translate (model, vec3(0.0f,25.0f,-200.0f)); 
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	monkey_mesh->draw();
	monkey_mesh->unbind();

	glUseProgram(0);

    glutSwapBuffers();
}



void updateScene() 
{	
	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	// rotate the model slowly around the y axis
	rotate_y+=0.35f;
	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	//Shader
	meshShader = new Shader("../Shaders/simpleVertexShader.txt", "../Shaders/simpleFragmentShader.txt");
	glUniform1i(glGetUniformLocation(meshShader->Program, "uTexture"), 0); 

	//mountain
	mountain_mesh = new Mesh();
	mountain_mesh->generateObjectBufferMesh("../mesh.obj", meshShader);
	mountain_mesh ->loadTexture("../Rocks.jpg");

	//monkey
	monkey_mesh = new Mesh();
	monkey_mesh->generateObjectBufferMesh("../Suzanne.dae", meshShader);
	monkey_mesh ->loadTexture("../Mystery.png");

}


// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) 
{
	if(key=='d')
	{
		translate_view_x -= 0.5;
		//Translate the base, etc.
	}
	if(key=='a')
	{
		translate_view_x += 0.5;
		//Translate the base, etc.
	}
	if(key=='s')
	{
		translate_view_y += 0.5;
		//Translate the base, etc.
	}
	if(key=='w')
	{
		translate_view_y -= 0.5;
		//Translate the base, etc.
	}
	if(key=='z')
	{
		translate_view_z -= 0.5;
		//Translate the base, etc.
	}
	if(key=='r')
	{
		translate_view_x = 0.0;
		translate_view_y = 0.0;
		translate_view_z = 0.0;
		//Translate the base, etc.
	}
}

int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}











