#include "OBJ.h"
using namespace std;


OBJ::OBJ(const char *  model) {
	texture = new GLuint*[2];
	std::vector<glm::vec3>  out_vertices; 
	std::vector<glm::vec2>  out_uvs;
	std::vector<glm::vec3>  out_normals;

	vertexCount = loadOBJ(model,out_vertices,out_uvs,out_normals);
	float *vertices = new float[vertexCount*4];
	float *normals= new float[vertexCount*4];
	float* texCoords  = new float[vertexCount*2];
	for(int i =0;i<vertexCount;i++)
	{
		
			vertices[i*4] = out_vertices[i].x;
			vertices[i*4+1] = out_vertices[i].y;
			vertices[i*4+2] = out_vertices[i].z;
			vertices[i*4+3] = 1.0f;
			normals[i*4]=out_normals[i].x;
			normals[i*4+1]=out_normals[i].y;
			normals[i*4+2]=out_normals[i].z;
			normals[i*4+3]=0.0f;
			texCoords[i*2]=out_uvs[i].x;
			texCoords[i*2+1]=out_uvs[i].y;
		
	}cout<<"3"<<endl;
	bufVertices=makeBuffer(vertices, vertexCount, sizeof(float)*4); 
	bufNormals=makeBuffer(normals, vertexCount, sizeof(float)*4);
	bufTexCoords=makeBuffer(texCoords,vertexCount,sizeof(float)*2);cout<<"3"<<endl;
	glGenVertexArrays(1,&vao);cout<<"3"<<endl;
	delete[] vertices;
	delete[] texCoords;
	delete[] normals;
}
GLuint *OBJ::makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint *handle = new GLuint;
	
	glGenBuffers(1,handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który bêdzie zawiera³ tablicê danych
	glBindBuffer(GL_ARRAY_BUFFER,*handle);  //Uaktywnij wygenerowany uchwyt VBO 
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablicê do VBO
	
	return handle;
}
int OBJ::readTexture(string filename,int i) {
	texture[i] = new GLuint ;
	TGAImg img;
	glActiveTexture(GL_TEXTURE0);
	
	if (img.Load((char*)filename.c_str())==IMG_OK) {
		glGenTextures(1,texture[i]); //Zainicjuj uchwyt tex
		glBindTexture(GL_TEXTURE_2D,*texture[i]); //Przetwarzaj uchwyt tex
		if (img.GetBPP()==24) //Obrazek 24bit
			glTexImage2D(GL_TEXTURE_2D,0,3,img.GetWidth(),img.GetHeight(),0,
				GL_RGB,GL_UNSIGNED_BYTE,img.GetImg());
		else if (img.GetBPP()==32) //Obrazek 32bit
			glTexImage2D(GL_TEXTURE_2D,0,4,img.GetWidth(),img.GetHeight(),0,
				GL_RGBA,GL_UNSIGNED_BYTE,img.GetImg());      
		else {
			printf("Nieobs³ugiwany format obrazka w pliku: %s \n",filename.c_str());
		}
	} else {
		printf("B³¹d przy wczytywaniu pliku: %s\n",filename.c_str());
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	
	return 0;
}
void OBJ::swapTex()
{
	GLuint *wsk = texture[0];
	texture[0] = texture[1];
	texture[1]= wsk;
}
int OBJ::loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return -1;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return -1;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return vertexIndices.size();
}
void OBJ::setupShaders( char* vshader, char* gshader,  char* fshader) {
	shaderProgram=new ShaderProgram(vshader,gshader,fshader);
}

void OBJ::cleanShaders() {
	delete shaderProgram;
}
	OBJ::~OBJ()
	{
		
	}

void OBJ::setupOBJ(OBJ *wsk)
{
	glGenVertexArrays(1,&vao);
	this->shaderProgram = wsk->shaderProgram;
	this->bufVertices= wsk->bufVertices;
	this->bufNormals = wsk->bufNormals;
	this->bufTexCoords= wsk->bufTexCoords;
	this->vertexCount= wsk->vertexCount;
}
void OBJ::setupTex(OBJ *wsk)
{
	this->texture[0] = wsk->texture[0];
	this->texture[1] = wsk->texture[1];

}
void OBJ::setupShaders(OBJ *wsk)
{
	this->shaderProgram = wsk->shaderProgram;

}
bool readMatrixDataM_Obj(vector<OBJ> *obj, char *name,int b,int e)
{
	

	
	if(abs(e)>(*obj).size()|| e<b||b<0)return false;
		
	std::ifstream plik;
	plik.open(name);
	float f[16];

	for(int i =0;i<e;i++)
	{
		for(int j=0;j<16;j++)
		{
			if( !plik.good() )
         	return false;
			plik>>f[j];
			
		}
		if(i>=b)(*obj)[i].matM = glm::mat4(f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]);
	}
	
	//cout<<&Ob<<endl;
	//obiekty = &Ob;
	return true;
}

