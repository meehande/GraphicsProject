
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Terrain.h"
#include "ModelView.h"

// VARIABLES
Shader* meshShader;
Shader* monkeyShader;
Shader* skyboxShader;
Shader* terrainShader;

Mesh* mountain_mesh;
Mesh* monkey_mesh;
Mesh* sphere_mesh;
Mesh* moon_mesh;
Mesh* tree;

Terrain* terrain = NULL;

ModelView* camera;
ModelView* terrainPos;

GLuint skyboxVAO;
GLuint cubemapTexture;

vec3 LightDirection = vec3(-0.1f, -1, 0);
vec3 LightColour = vec3(1, 1, 1);
vec3 SkyColour = vec3(178 / 256.0f, 110 / 256.0f, 58 / 256.0f);

bool keys[256];

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

#pragma region SKYBOX 
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
#pragma endregion


void display()
{
	glClearColor(SkyColour.v[0], SkyColour.v[1], SkyColour.v[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable (GL_DEPTH_TEST); // enable depth-testing	


	mat4 view = identity_mat4 ();
	mat4 persp_proj = perspective(60.0, (float)width/(float)height, 0.1, -10.0);
	mat4 model = identity_mat4 ();
 
	
//--- SKYBOX ---------------------------------------------------------------//
    glDepthMask(GL_FALSE);// Remember to turn depth writing off
	skyboxShader->use();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->Program, "view"), 1, GL_FALSE,identity_mat4 ().m);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->Program, "projection"), 1, GL_FALSE, persp_proj.m);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(skyboxShader->Program, "skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
//------------------------------------------------------------------------//
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
	
    
	glUniform1f(ambientStrengthLoc, 0.1f);
	glUniform1f(tileFactorLoc, 64);
	glUniform3f(lightPosLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc,  LightColour.v[0],LightColour.v[1], LightColour.v[2]);

	model = terrainPos->CreateModelMatrix();
	view = translate (view, vec3 (0.0, -20.0f, 183.0f));
	view = translate(view, vec3(translate_view_x, translate_view_y, translate_view_z));
	view = rotate_y_deg(view, rotate_view_y);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, camera->CreateViewMatrix().m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	mountain_mesh->draw();
	mountain_mesh->unbind();


	//tree - child of terrain
	tree->bind();
	mat4 childlocalmat1 = identity_mat4();
	
	childlocalmat1 = translate(childlocalmat1, vec3(0,10,0));
	childlocalmat1 = scale(childlocalmat1, vec3(4,4,4));
	mat4 childglobalmat1 = model*childlocalmat1;
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, camera->CreateViewMatrix().m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, childglobalmat1.m);
	tree->draw();
	tree->unbind();

	glUseProgram(0); //unuse mesh shader

	terrainShader->use();
	terrain->bind();

	int model_loc = glGetUniformLocation (terrainShader->Program, "mModelMatrix");
	int view_loc = glGetUniformLocation (terrainShader->Program, "mViewMatrix");
	int proj_loc = glGetUniformLocation (terrainShader->Program, "mProjectionMatrix");
	int view_pos_loc = glGetUniformLocation(terrainShader->Program, "vViewPosition");
    int sky_col_loc  = glGetUniformLocation(terrainShader->Program, "vSkyColour");
	int light_col_loc = glGetUniformLocation(terrainShader->Program, "vLightColour");
	int light_dir_loc = glGetUniformLocation(terrainShader->Program, "vLightDirection");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, terrainPos->CreateModelMatrix().m);
	glUniformMatrix4fv(view_loc, 1 , GL_FALSE, camera->CreateViewMatrix().m);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, persp_proj.m);
	glUniform3f(view_pos_loc, camera->Position.v[0], camera->Position.v[1],camera->Position.v[2] );
	glUniform3f(sky_col_loc, SkyColour.v[0], SkyColour.v[1],SkyColour.v[2]);
	glUniform3f(light_col_loc, LightColour.v[0], LightColour.v[1],LightColour.v[2]);
	glUniform3f(light_dir_loc, LightDirection.v[0], LightDirection.v[1],LightDirection.v[2]);

	terrain->Render(GL_TRIANGLES);

	terrain->unbind();
	glUseProgram(0);

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
	float terrainHeight = terrain->GetHeight(camera->Position, terrainPos->Position);
	if(camera->Position.v[1] < terrainHeight){
		camera->Position.v[1] = terrainHeight;
		//cout << terrainHeight << endl;
	}
	
}


void init()
{
	//Shaders
	meshShader = new Shader("../Shaders/simpleVertexShader.txt", "../Shaders/simpleFragmentShader.txt");
	glUniform1i(glGetUniformLocation(meshShader->Program, "uTexture"), 0); 

	skyboxShader = new Shader("../Shaders/skybox_vertexShader.txt", "../Shaders/skybox_fragShader.txt");

	terrainShader = new Shader("../Shaders/LightVertexShader.glsl","../Shaders/LightFragmentShader.glsl");

	//Terrain
	terrain = new Terrain(0, 0, 1025, 1025, 8.0f, 64.0f, 22.0f, 0.25f, 4);
	terrain->loadTexture("../Rocks.jpg");
	terrainPos = new ModelView(vec3(500, 10, 500), vec3(0, 0, 0), 29.0f, 1.0f);	
	
	//mountain
	mountain_mesh = new Mesh();
	mountain_mesh->generateObjectBufferMesh("../mesh.obj", meshShader);
	mountain_mesh ->loadTexture("../Rocks.jpg");

	//tree

	tree = new Mesh();
	tree->generateObjectBufferMesh("../untitled.obj", terrainShader);
	
	
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

	camera = new ModelView(vec3(500, 10, 500), vec3(0, 0, 0), 2.0f, 1.0f);	
	

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


void keypress(GLFWwindow* window) 
{
	//press = keydown
	//release = keyup
	if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
	{
		translate_view_x -= move_amount;
		camera->Right();
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
	{
		translate_view_x += move_amount;
		//Translate the base, etc.
		camera->Left();
	}
	if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
	{
		translate_view_y += move_amount;
		camera->Backward();
		//Translate the base, etc.
	}
	if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
	{
		translate_view_y -= move_amount;
		//Translate the base, etc.
		camera->Forward();
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










