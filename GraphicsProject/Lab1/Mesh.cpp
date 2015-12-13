#include "Mesh.h"

Mesh::Mesh()
{
	GLuint id;
	glGenVertexArrays(1, &id);
	this->mesh_vao = id;
	this->BufferIds = std::vector<int>();
	loc1 = 0;//positions
	loc2 = 2;//normals
	loc3 = 1;//textures
}


bool Mesh::read_mesh (const char* file_name) {
		const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate); // TRIANGLES!
		//fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		printf(" mesh loading... \n");
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
		g_vp.clear();
		g_vt.clear();
		g_vn.clear();


		for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
			const aiMesh* mesh = scene->mMeshes[m_i];
			// printf ("    %i vertices in mesh\n", mesh->mNumVertices);
			g_point_count = mesh->mNumVertices;
			for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
				//	if((v_i % 100 == 0) || (v_i > 2900)) cout << v_i << endl;

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

void Mesh::generateObjectBufferMesh(const char* name, Shader* meshShader) 
{
	read_mesh(name); //read in file with positions, normals, textures

	unsigned int n_vbos = 3;
	//GenVertexArray
	//glGenVertexArrays(1, &mesh_vao);
	//glBindVertexArray(mesh_vao);
	
	loc1 = glGetAttribLocation(meshShader->Program, "vPosition");
	loc2 = glGetAttribLocation(meshShader->Program, "vNormal");
	loc3 = glGetAttribLocation(meshShader->Program, "vTextureCoords");


	//GenVBOs & fill data
	generateBuffers(n_vbos);
	glBindVertexArray(mesh_vao);
	
	fillBufferData(loc1,3, &g_vp[0], g_point_count, GL_STATIC_DRAW);
	fillBufferData(loc2,3, &g_vn[0], g_point_count, GL_STATIC_DRAW);
	fillBufferData(loc3,2, &g_vt[0], g_point_count, GL_STATIC_DRAW);

	cout << "BufferID0: " << BufferIds.data() <<endl;

	//Unbind
	glDisableVertexAttribArray(loc1);
	glDisableVertexAttribArray(loc2);
	glDisableVertexAttribArray(loc3);
	glBindVertexArray(0);
}

void Mesh::loadTexture(const char* s)
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

void Mesh::bind(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(this->mesh_vao);
	for(int i=0; i<BufferIds.size(); i++)
		glEnableVertexAttribArray(i);
	glEnableVertexAttribArray(loc1);
	glEnableVertexAttribArray(loc2);
	glEnableVertexAttribArray(loc3);
		
}
void Mesh::draw(){
	//Draw
	glDrawArrays (GL_TRIANGLES, 0, g_point_count);	
}
void Mesh::unbind(){
	//Unbind Buffers, Shader and Texture
	for(int i=0; i<BufferIds.size(); i++)
		glDisableVertexAttribArray(i);
	glDisableVertexAttribArray(loc1);
	glDisableVertexAttribArray(loc2);
	glDisableVertexAttribArray(loc3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Mesh::generateBuffers(int n){
//	glGenVertexArrays(1, &mesh_vao);
	GLuint* vbo = new GLuint[n];
	glBindVertexArray(mesh_vao);
	glGenBuffers(n, vbo);
	for (int i = 0; i<n; i++)
		this->BufferIds.push_back(vbo[i]);
	delete[] vbo;
}

void Mesh::fillBufferData(int attrib, int dim, float* data, int n, int hint){
	num_attribs = n;
	glBindVertexArray(mesh_vao);
	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[attrib]);
	glBufferData(GL_ARRAY_BUFFER, n*dim*sizeof(float), data, hint);
	glVertexAttribPointer(attrib, dim, GL_FLOAT, GL_FALSE, 0,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
