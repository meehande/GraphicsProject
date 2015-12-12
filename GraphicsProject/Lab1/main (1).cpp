
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "maths_funcs.h"
#include "Terrain.h"
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
Shader* skyboxShader;

Mesh* mountain_mesh;
Mesh* monkey_mesh;
Mesh* sphere_mesh;
Mesh* moon_mesh;

Terrain* terrainModel = NULL;

GLuint skyboxVAO;
GLuint cubemapTexture;

bool keys[256];

std::vector<float> g_vp, g_vn, g_vt;
int g_point_count = 0;


// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void keypress(GLFWwindow* window);
float move_amount = 0.1;

using namespace std;


int width = 800;
int height = 600;
GLfloat rotate_y = 0.0f;
GLfloat translate_view_x = 0.0;
GLfloat translate_view_y = 0.0;
GLfloat translate_view_z = 0.0;
GLfloat rotate_view_y = 0.0;
// Light attributes
vec3 lightPos(3.0f, 1.0f, 0.0f);

GLuint loadCubemap(vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
	printf("cubemapping\n");
    int width,height;
    unsigned char* image;
	
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
        );
    }
	printf("cubemapping again\n");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}  

    GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };




void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable (GL_DEPTH_TEST); // enable depth-testing	
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

	mat4 view = identity_mat4 ();
	mat4 persp_proj = perspective(60.0, (float)width/(float)height, 0.1, -10.0);
	mat4 model = identity_mat4 ();
 
	
	// Draw skybox first
    glDepthMask(GL_FALSE);// Remember to turn depth writing off

	skyboxShader->use();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->Program, "view"), 1, GL_FALSE,view.m);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->Program, "projection"), 1, GL_FALSE, persp_proj.m);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(skyboxShader->Program, "skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    //TERRAIN
	meshShader->use();
	mountain_mesh->bind();
	
	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation (meshShader->Program, "mModelMatrix");
	int view_mat_location = glGetUniformLocation (meshShader->Program, "mViewMatrix");
	int proj_mat_location = glGetUniformLocation (meshShader->Program, "mProjectionMatrix");
	GLint lightPosLoc = glGetUniformLocation(meshShader->Program, "vLightPosition");
    GLint lightColorLoc  = glGetUniformLocation(meshShader->Program, "vLightColour");
	GLint ambientStrengthLoc = glGetUniformLocation(meshShader->Program, "vAmbientStrength");
	GLint tileFactorLoc = glGetUniformLocation(meshShader->Program, "tiling");
	glUniform3f(lightPosLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
	glUniform1f(ambientStrengthLoc, 0.1f);
	glUniform1f(tileFactorLoc, 64);

	
	view = translate (view, vec3 (0.0, -20.0f, 183.0f));
	view = translate(view, vec3(translate_view_x, translate_view_y, translate_view_z));
	view = rotate_y_deg(view, rotate_view_y);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	mountain_mesh->draw();
	mountain_mesh->unbind();

	terrainModel->bind();
	terrainModel->Render(GL_TRIANGLES);
	terrainModel->unbind();

	/*monkey_mesh->bind();

	model = rotate_x_deg(model, 90.0);
	model = rotate_z_deg(model, 180.0);
	model = rotate_y_deg (model, rotate_y); 
	model = translate (model, vec3(0.0f,25.0f,-200.0f)); 

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	glUniform1f(ambientStrengthLoc, 0.8f);
	glUniform1f(tileFactorLoc, 1);

	monkey_mesh->draw();

	model = identity_mat4();
	model = rotate_x_deg(model, 90.0);
	model = rotate_z_deg(model, 180.0);
	model = rotate_y_deg (model, rotate_y); 
	model = translate (model, vec3(0.0f,25.0f,-500.0f)); 

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	monkey_mesh->draw();
	monkey_mesh->unbind();

	sphere_mesh->bind();
	model = identity_mat4();
	model = scale(model, vec3(4,4,4));

	model = translate (model, vec3(100.0f,25.0f,-200.0f)); 
	
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	sphere_mesh->draw();

	model = identity_mat4();
	model = scale(model, vec3(4,4,4));
	model = translate (model, vec3(-50.0f,25.0f,-350.0f));
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	sphere_mesh->draw();

	model = identity_mat4();
	model = scale(model, vec3(4,4,4));
	model = translate (model, vec3(150.0f,25.0f,-450.0f));
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	sphere_mesh->draw();

	sphere_mesh->unbind();*/

	
	glUseProgram(0);

}



void updateScene(GLFWwindow* window) 
{	

	keypress(window);
	// rotate the model slowly around the y axis
	rotate_y+=0.35f;
}


void init()
{
	//Shader
	meshShader = new Shader("../Shaders/simpleVertexShader.txt", "../Shaders/simpleFragmentShader.txt");
	glUniform1i(glGetUniformLocation(meshShader->Program, "uTexture"), 0); 

	skyboxShader = new Shader("../Shaders/skybox_vertexShader.txt", "../Shaders/skybox_fragShader.txt");

	//mountain
	mountain_mesh = new Mesh();
	mountain_mesh->generateObjectBufferMesh("../mesh.obj", meshShader);
	mountain_mesh ->loadTexture("../Rocks.jpg");
	
	//monkey
	//monkey_mesh = new Mesh();
	//monkey_mesh->generateObjectBufferMesh("../Suzanne.dae", meshShader);
	//monkey_mesh ->loadTexture("../Mystery.png");

	////sphere
	//sphere_mesh = new Mesh();
	//sphere_mesh->generateObjectBufferMesh("../rockysphere.dae", meshShader);
	//sphere_mesh ->loadTexture("../Mystery.png");

	////sphere
	//moon_mesh = new Mesh();
	//moon_mesh->generateObjectBufferMesh("../sphere.obj", meshShader);
	//moon_mesh ->loadTexture("../Moon.jpg");

	terrainModel = new Terrain(0, 0, 1025, 1025, 8.0f, 64.0f, 22.0f, 0.25f, 4);
	terrainModel->loadTexture("../Mystery.jpg");

	 // Setup skybox VAO
    GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

	  // Cubemap (Skybox)
    vector<const GLchar*> faces;
    faces.push_back("../skybox/right.jpg");
    faces.push_back("../skybox/left.jpg");
    faces.push_back("../skybox/top.jpg");
    faces.push_back("../skybox/bottom.jpg");
    faces.push_back("../skybox/back.jpg");
    faces.push_back("../skybox/front.jpg");
    cubemapTexture = loadCubemap(faces);

}


// Placeholder code for the keypress
void keypress(GLFWwindow* window) 
{
	//press = keydown
	//release = keyup
	if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
	{
		translate_view_x -= move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
	{
		translate_view_x += move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
	{
		translate_view_y += move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
	{
		translate_view_y -= move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_Z)==GLFW_PRESS)
	{
		translate_view_z -= move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_X)==GLFW_PRESS)
	{
		translate_view_z += move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_T)==GLFW_PRESS)
	{
		rotate_view_y -= move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_Y)==GLFW_PRESS)
	{
		rotate_view_y += move_amount;
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_R)==GLFW_PRESS)
	{
		translate_view_x = 0.0;
		translate_view_y = 0.0;
		translate_view_z = 0.0;
		rotate_view_y = 0.0;
		//Translate the base, etc.
	}
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if(err != GLEW_OK)
		return -1;

	init();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
		display();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
		updateScene(window);
    }

    glfwTerminate();
    return 0;
}









