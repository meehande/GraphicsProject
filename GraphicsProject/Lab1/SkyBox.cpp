#include "SkyBox.h"
#include <vector>
#include <SOIL.h>
GLuint loadCubemap(vector<const GLchar*> faces);

void SkyBox::init(){

	   float f = 0.1f;
    GLfloat skyboxVertices[] = {
        // Positions          
        -f,  f, -f,
        -f, -f, -f,
         f, -f, -f,
         f, -f, -f,
         f,  f, -f,
        -f,  f, -f,
  
        -f, -f,  f,
        -f, -f, -f,
        -f,  f, -f,
        -f,  f, -f,
        -f,  f,  f,
        -f, -f,  f,
  
         f, -f, -f,
         f, -f,  f,
         f,  f,  f,
         f,  f,  f,
         f,  f, -f,
         f, -f, -f,
   
        -f, -f,  f,
        -f,  f,  f,
         f,  f,  f,
         f,  f,  f,
         f, -f,  f,
        -f, -f,  f,
  
        -f,  f, -f,
         f,  f, -f,
         f,  f,  f,
         f,  f,  f,
        -f,  f,  f,
        -f,  f, -f,
  
        -f, -f, -f,
        -f, -f,  f,
         f, -f, -f,
         f, -f, -f,
        -f, -f,  f,
         f, -f,  f
    };

	GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

	vector<const GLchar*> faces;
    faces.push_back("../skybox/right.jpg");
    faces.push_back("../skybox/left.jpg");
    faces.push_back("../skybox/top.jpg");
    faces.push_back("../skybox/bottom.jpg");
    faces.push_back("../skybox/back.jpg");
    faces.push_back("../skybox/front.jpg");
    cubemapTexture = loadCubemap(faces);

    glUniform1i(glGetUniformLocation(shader->Program, "skybox"), 0);


}

SkyBox::SkyBox(mat4* v, mat4* p){
	shader = new Shader("../Shaders/skybox_vertexShader.txt", "../Shaders/skybox_fragShader.txt");
	init();
	view = v;
	persp = p;
}


void  SkyBox::bind(){
	shader->use();
	glBindVertexArray(skyboxVAO);
	glEnableVertexAttribArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
}


void  SkyBox::unbind(){

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	shader->unuse();
}


void  SkyBox::draw(){
	 shader->loadMatrix("view", *view);
	 shader->loadMatrix("persp", *persp);
     
	 glDrawArrays(GL_TRIANGLES, 0, 36);
}

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
