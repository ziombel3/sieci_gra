#ifndef OBJ_H
#define OBJ_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>     
#include <string>  
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shaderprogram.h"
#include "tga.h"
using namespace std;
class OBJ {
private:
	int loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
);
	GLuint **texture;
public:
	OBJ(const char * model); 
	OBJ(){texture = new GLuint*[2];}
	int readTexture(string filename,int i);
	GLuint *makeBuffer(void *data, int vertexCount, int vertexSize) ;
	void cleanShaders();
	void setupShaders( char* vshader, char* gshader,  char* fshader);
	void setupOBJ(OBJ *wsk);
	void setupTex(OBJ *wsk);
	void setupShaders(OBJ *wsk);
	GLuint *getTex(int i){ return texture[i];}
	void setTex(int i,GLuint *texture){ texture[i]= *texture;};
	void swapTex();
	~OBJ();
	glm::mat4  matM;//modelu
	glm::mat4  matM2;//modelu
	ShaderProgram *shaderProgram;
	GLuint *bufVertices;
	GLuint *bufNormals; 
	GLuint *bufTexCoords;  //Uchwyt na bufor VBO przechowuj¹cy tablicê wspó³rzêdnych teksturowania
	int vertexCount;
	GLuint vao;
	int x,y;
	float blend=0;
	int anim=0,st_anim;//st do myszy
	
};
bool readMatrixDataM_Obj(vector<OBJ> *obiekty, char *name,int b,int e);

#endif
