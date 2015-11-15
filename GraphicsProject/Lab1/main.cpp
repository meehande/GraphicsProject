
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


#include "Shader.h"

// VARIABLES
Shader* meshShader;

std::vector<float> g_vp, g_vn, g_vt;
int g_point_count = 0;


// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;


unsigned int mesh_vao = 0;
int width = 800;
int height = 600;

GLuint loc1, loc2, loc3;
GLuint texture;
GLfloat rotate_y = 0.0f;

// Light attributes
vec3 lightPos(3.0f, 1.0f, 0.0f);


#pragma region MESH LOADING
/*----------------------------------------------------------------------------
                   MESH LOADING FUNCTION
  ----------------------------------------------------------------------------*/

bool load_mesh (const char* file_name) {
  const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate); // TRIANGLES!
        fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
  if (!scene) {
    fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
    return false;
  }
  printf ("  %i animations\n", scene->mNumAnimations);
  printf ("  %i cameras\n", scene->mNumCameras);
  printf ("  %i lights\n", scene->mNumLights);
  printf ("  %i materials\n", scene->mNumMaterials);
  printf ("  %i meshes\n", scene->mNumMeshes);
  printf ("  %i textures\n", scene->mNumTextures);
  
  for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
    const aiMesh* mesh = scene->mMeshes[m_i];
    printf ("    %i vertices in mesh\n", mesh->mNumVertices);
    g_point_count = mesh->mNumVertices;
	cout << "num vertices" <<  mesh->mNumVertices << endl;
	cout << "HERE\n";
    for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if((v_i % 100 == 0) || (v_i > 2900)) cout << v_i << endl;
		
      if (mesh->HasPositions ()) {
		//cout << "Got here ok!\n";
        const aiVector3D* vp = &(mesh->mVertices[v_i]);
        //printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
        g_vp.push_back (vp->x);
        g_vp.push_back (vp->y);
        g_vp.push_back (vp->z);
      }
      if (mesh->HasNormals ()) {
		//cout << "AND HERE\n";
        const aiVector3D* vn = &(mesh->mNormals[v_i]);
        //printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
        g_vn.push_back (vn->x);
        g_vn.push_back (vn->y);
        g_vn.push_back (vn->z);
      }
      if (mesh->HasTextureCoords (0)) {
	//	cout << "shouldn't be here but am?\n";
        const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
        //printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
        g_vt.push_back (vt->x);
        g_vt.push_back (vt->y);
      }
      if (mesh->HasTangentsAndBitangents ()) {
        // NB: could store/print tangents here
      }
    }
  }  
  aiReleaseImport (scene);
  return true;
}

#pragma endregion MESH LOADING


// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

void generateObjectBufferMesh(const char* name) 
{
/*----------------------------------------------------------------------------
                   LOAD MESH HERE AND COPY INTO BUFFERS
  ----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	load_mesh(name);

	unsigned int vp_vbo = 0;
	unsigned int vn_vbo = 0;
	unsigned int vt_vbo = 0;

	//GenVertexArray
	glGenVertexArrays(1, &mesh_vao);
	glBindVertexArray(mesh_vao);
	
	loc1 = glGetAttribLocation(meshShader->Program, "vPosition");
	loc2 = glGetAttribLocation(meshShader->Program, "vNormal");
	loc3 = glGetAttribLocation(meshShader->Program, "vTextureCoords");

	//GenVBOs
	glGenBuffers (1, &vp_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 3 * sizeof (float), &g_vp[0], GL_STATIC_DRAW);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers (1, &vn_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 3 * sizeof (float), &g_vn[0], GL_STATIC_DRAW);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, g_point_count * 2 * sizeof(float), &g_vt[0], GL_STATIC_DRAW);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	//Unbind
	glDisableVertexAttribArray(loc1);
	glDisableVertexAttribArray(loc2);
	glDisableVertexAttribArray(loc3);
	glBindVertexArray(0);
}

void loadTexture(const char* s)
{
	//load and create texture
	int w, h;
	unsigned char* image = SOIL_load_image(s, &w, &h, 0, SOIL_LOAD_RGB);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_LOD_BIAS, 0);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); //unbind
}
#pragma endregion VBO_FUNCTIONS


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable (GL_DEPTH_TEST); // enable depth-testing	
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	
	//MONKEY HEAD//
	meshShader->use();

	//Bind Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(meshShader->Program, "uTexture"), 0); // Not GL_TEXTURE0, Actually only needs to be called once after Creating Shader.

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation (meshShader->Program, "mModelMatrix");
	int view_mat_location = glGetUniformLocation (meshShader->Program, "mViewMatrix");
	int proj_mat_location = glGetUniformLocation (meshShader->Program, "mProjectionMatrix");
	GLint lightPosLoc = glGetUniformLocation(meshShader->Program, "vLightPosition");
    GLint lightColorLoc  = glGetUniformLocation(meshShader->Program, "vLightColour");
	glUniform3f(lightPosLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);

	// Root of the Hierarchy
	mat4 view = identity_mat4 ();
	mat4 persp_proj = perspective(45.0, (float)width/(float)height, 0.1, 100.0);
	mat4 model = identity_mat4 ();
	model = rotate_x_deg(model, 90.0);
	model = rotate_z_deg(model, 180.0);
	model = rotate_y_deg (model, rotate_y); 
	model = translate (model, vec3(0.0f,0.0f,0.0f)); 
	view = translate (view, vec3 (0.0, 0.0, -5.0f));

	// update uniforms & draw
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);

	//Bind Buffers
	glBindVertexArray(mesh_vao);
	glEnableVertexAttribArray(loc1);
	glEnableVertexAttribArray(loc2);
	glEnableVertexAttribArray(loc3);

	//Draw
	glDrawArrays (GL_TRIANGLES, 0, g_point_count);	

	//Unbind Buffers, Shader and Texture
	glDisableVertexAttribArray(loc1);
	glDisableVertexAttribArray(loc2);
	glDisableVertexAttribArray(loc3);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	//Buffers
	generateObjectBufferMesh("../rockysphere.dae");
	//Textures
	loadTexture("../Rocks.jpg");	
}


// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) 
{
	if(key=='x')
	{
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











