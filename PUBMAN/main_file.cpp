#include "main.h"
#define JOY_DEV "/dev/input/js0"
#define spaceX 2.975f 
#define spaceY 2.09f
//postprocesing
GLuint fbo, fbo_texture, rbo_depth;
GLuint vbo_fbo_vertices;
GLuint program_postproc, attribute_v_coord_postproc, uniform_fbo_texture,uniform_offset;;
ShaderProgram *shaderPostProc;
float speedWave=1.0f;

//Macierze
glm::mat4  matP;//rzutowania
glm::mat4  matV;//widoku
//glm::mat4  matM;//modelu

//Ustawienia okna i rzutowania
int windowPositionX=100;
int windowPositionY=100;
int windowWidth=1280;
int windowHeight=720;
float cameraAngle=45.0f;

//Zmienne do animacji
float speed=12; //120 stopni/s
int lastTime=0;
float angle=0;
float swapT=0.0f;
int b=1;
vector<OBJ> obiekty;
vector<OBJ> postacie;
ShaderProgram *shaderText;
vector<OBJ> lives;

Text *text;

int **objectMap;
Player **players;
bool mouse_back=false;
int mouse_count =3;

void assignVBOtoAttribute(int i,char* attributeName, GLuint *bufVBO, int variableSize) {
	GLuint location=obiekty[i].shaderProgram->getAttribLocation(attributeName); //Pobierz numery slotów dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER,*bufVBO);  //Uaktywnij uchwyt VBO 
	glEnableVertexAttribArray(location); //W³¹cz u¿ywanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location,variableSize,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location maj¹ byæ brane z aktywnego VBO
}
//Procedura rysuj¹ca jaki obiekt. Ustawia odpowiednie parametry dla vertex shadera i rysuje.
void drawObject(int i) {
	//W³¹czenie programu cieniuj¹cego, który ma zostaæ u¿yty do rysowania
	//W tym programie wystarczy³oby wywo³aæ to raz, w setupShaders, ale chodzi o pokazanie, 
	//¿e mozna zmieniaæ program cieniuj¹cy podczas rysowania jednej sceny
	obiekty[i].shaderProgram->use();
	//cout<<"1"<<endl;
	//Przeka¿ do shadera macierze P,V i M.
	//W linijkach poni¿ej, polecenie:
	//  shaderProgram->getUniformLocation("P") 
	//pobiera numer slotu odpowiadaj¹cego zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powy¿szym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze, 
	//a matP w glm::value_ptr(matP) odpowiada deklaracji  "glm::mat4 matP;" TYM pliku.
	//Ca³a poni¿sza linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane ze zmiennej matP
	//zadeklarowanej globalnie w tym pliku. 
	//Pozosta³e polecenia dzia³aj¹ podobnie.
	glUniformMatrix4fv(obiekty[i].shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(matP));
	glUniformMatrix4fv(obiekty[i].shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(matV));//cout<<"2"<<endl;
	glUniformMatrix4fv(obiekty[i].shaderProgram->getUniformLocation("M"),1, false,  glm::value_ptr(obiekty[i].matM));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,*(obiekty[i].getTex(0)));
	//Uaktywnienie VAO i tym samym uaktywnienie predefiniowanych w tym VAO powi¹zañ slotów atrybutów z tablicami z danymi
	glBindVertexArray((obiekty[i].vao));
	assignVBOtoAttribute(i,const_cast<char*> ("texCoords"),obiekty[i].bufTexCoords,2); 
	assignVBOtoAttribute(i,const_cast<char*> ("vertex"),obiekty[i].bufVertices,4);
	assignVBOtoAttribute(i,const_cast<char*> ("normal"),obiekty[i].bufNormals,4);
	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES,0,obiekty[i].vertexCount);
	
	//Posprz¹tanie po sobie (niekonieczne w sumie je¿eli korzystamy z VAO dla ka¿dego rysowanego obiektu)
	glBindVertexArray(0);
}
void drawObjectP(int i,int k) {
	//W³¹czenie programu cieniuj¹cego, który ma zostaæ u¿yty do rysowania
	//W tym programie wystarczy³oby wywo³aæ to raz, w setupShaders, ale chodzi o pokazanie, 
	//¿e mozna zmieniaæ program cieniuj¹cy podczas rysowania jednej sceny
	postacie[i].shaderProgram->use();
	//cout<<"1"<<endl;
	//Przeka¿ do shadera macierze P,V i M.
	//W linijkach poni¿ej, polecenie:
	//  shaderProgram->getUniformLocation("P") 
	//pobiera numer slotu odpowiadaj¹cego zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powy¿szym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze, 
	//a matP w glm::value_ptr(matP) odpowiada deklaracji  "glm::mat4 matP;" TYM pliku.
	//Ca³a poni¿sza linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane ze zmiennej matP
	//zadeklarowanej globalnie w tym pliku. 
	//Pozosta³e polecenia dzia³aj¹ podobnie.
	glUniformMatrix4fv(postacie[i].shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(matP));
	glUniformMatrix4fv(postacie[i].shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(matV));//cout<<"2"<<endl;
	glUniformMatrix4fv(postacie[i].shaderProgram->getUniformLocation("M"),1, false,  glm::value_ptr(postacie[i].matM));
	glUniformMatrix4fv(postacie[i].shaderProgram->getUniformLocation("M2"),1, false,  glm::value_ptr(postacie[i].matM2));
	if(k)	glUniformMatrix4fv(postacie[i].shaderProgram->getUniformLocation("MR"),1, false,  glm::value_ptr(players[i]->matR));
	glUniform1f(postacie[i].shaderProgram->getUniformLocation("a"),postacie[i].blend);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,*(postacie[i].getTex(0)));
	//Uaktywnienie VAO i tym samym uaktywnienie predefiniowanych w tym VAO powi¹zañ slotów atrybutów z tablicami z danymi
	glBindVertexArray((obiekty[i].vao));
	assignVBOtoAttribute(i,const_cast<char*> ("texCoords"),postacie[i].bufTexCoords,2); 
	assignVBOtoAttribute(i,const_cast<char*> ("vertex"),postacie[i].bufVertices,4);
	assignVBOtoAttribute(i,const_cast<char*> ("normal"),postacie[i].bufNormals,4);
	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES,0,postacie[i].vertexCount);
	
	//Posprz¹tanie po sobie (niekonieczne w sumie je¿eli korzystamy z VAO dla ka¿dego rysowanego obiektu)
	glBindVertexArray(0);
}
void drawObjectL(int i) {
	//W³¹czenie programu cieniuj¹cego, który ma zostaæ u¿yty do rysowania
	//W tym programie wystarczy³oby wywo³aæ to raz, w setupShaders, ale chodzi o pokazanie, 
	//¿e mozna zmieniaæ program cieniuj¹cy podczas rysowania jednej sceny
	lives[i].shaderProgram->use();
	//cout<<"1"<<endl;
	//Przeka¿ do shadera macierze P,V i M.
	//W linijkach poni¿ej, polecenie:
	//  shaderProgram->getUniformLocation("P") 
	//pobiera numer slotu odpowiadaj¹cego zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powy¿szym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze, 
	//a matP w glm::value_ptr(matP) odpowiada deklaracji  "glm::mat4 matP;" TYM pliku.
	//Ca³a poni¿sza linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane ze zmiennej matP
	//zadeklarowanej globalnie w tym pliku. 
	//Pozosta³e polecenia dzia³aj¹ podobnie.
	glUniformMatrix4fv(lives[i].shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(matP));
	glUniformMatrix4fv(lives[i].shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(matV));//cout<<"2"<<endl;
	glUniformMatrix4fv(lives[i].shaderProgram->getUniformLocation("M"),1, false,  glm::value_ptr(lives[i].matM));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,*(lives[i].getTex(0)));
	//Uaktywnienie VAO i tym samym uaktywnienie predefiniowanych w tym VAO powi¹zañ slotów atrybutów z tablicami z danymi
	glBindVertexArray((obiekty[i].vao));
	assignVBOtoAttribute(i,const_cast<char*> ("texCoords"),lives[i].bufTexCoords,2); 
	assignVBOtoAttribute(i,const_cast<char*> ("vertex"),lives[i].bufVertices,4);
	assignVBOtoAttribute(i,const_cast<char*> ("normal"),lives[i].bufNormals,4);
	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES,0,lives[i].vertexCount);
	
	//Posprz¹tanie po sobie (niekonieczne w sumie je¿eli korzystamy z VAO dla ka¿dego rysowanego obiektu)
	glBindVertexArray(0);
}


void RenderString(float x, float y, void *font, const unsigned char* string)
{  
  

  //glColor3f(0, 1, 0); 
  glRasterPos3f(x, y,1.1f);

  glutBitmapString(font, string);
}
string strAppendInt(string str, int i)
{
	string tmp; // brzydkie rozwiązanie
	sprintf((char*)tmp.c_str(), "%d", i);
	string str2 = tmp.c_str();
	str.append(str2);
	return str;
}
void DrawText_()
{

	shaderText->use();
	glUniform4f(shaderText->getUniformLocation("color"),1,1,0,1);

	//int i=90;
	glUniform2f(shaderText->getUniformLocation("coord"),-0.9f,0.9f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)("PLAYER 1"));
	glUniform2f(shaderText->getUniformLocation("coord"),0.7f,0.9f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)("PLAYER 2"));
	glUniform2f(shaderText->getUniformLocation("coord"),-0.9f,0.8f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)("LIVES"));
	glUniform2f(shaderText->getUniformLocation("coord"),0.7f,0.8f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)("LIVES"));
	glUniform2f(shaderText->getUniformLocation("coord"),-0.9f,0.6f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)(strAppendInt("BEERS ",players[0]->beers).c_str()));
	glUniform2f(shaderText->getUniformLocation("coord"),0.7f,0.6f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)(strAppendInt("BEERS ",players[1]->beers).c_str()));
	glUniform2f(shaderText->getUniformLocation("coord"),-0.9f,0.5f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)(strAppendInt("SHOTS ",players[0]->shots).c_str()));
	glUniform2f(shaderText->getUniformLocation("coord"),0.7f,0.5f);
	RenderString(0,0,GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)(strAppendInt("SHOTS ",players[1]->shots).c_str()));
}
void DrawText()
{
	float sx = 2.0 / glutGet(GLUT_WINDOW_WIDTH);
	float sy = 2.0 / glutGet(GLUT_WINDOW_HEIGHT);
	text->initDisplayText(70);
	text->render_text("PLAYER 1", -1 + 8 * sx, 1-30 * sy, sx * 0.5, sy * 0.5);
	text->render_text("PLAYER 2", 1 - 250 * sx, 1-30 * sy, sx * 0.5, sy * 0.5);
	text->initDisplayText(65);
	text->render_text("LIVES", -1 + 8 * sx, 1-75 * sy, sx * 0.5, sy * 0.5);
	text->render_text("LIVES", 1 - 250 * sx, 1-75 * sy, sx * 0.5, sy * 0.5);
	text->render_text((const char*)(strAppendInt("BEERS ",players[0]->beers).c_str()), -1 + 8 * sx, 1-120 * sy, sx * 0.5, sy * 0.5);
	text->render_text((const char*)(strAppendInt("BEERS ",players[1]->beers).c_str()), 1 - 250 * sx, 1-120 * sy, sx * 0.5, sy * 0.5);
	text->render_text((const char*)(strAppendInt("SHOTS ",players[0]->shots).c_str()), -1 + 8 * sx, 1-165 * sy, sx * 0.5, sy * 0.5);
	text->render_text((const char*)(strAppendInt("SHOTS ",players[1]->shots).c_str()), 1 - 250 * sx, 1-165 * sy, sx * 0.5, sy * 0.5);
}
//Procedura rysuj¹ca
void displayFrame() {

	
	//Wyczyæ bufor kolorów i bufor g³êbokoci
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	//Wylicz macierz rzutowania
	matP=glm::mat4(1.0f);//glm::perspective(cameraAngle, (float)windowWidth/(float)windowHeight, 1.0f, 100.0f);
	
	//Wylicz macierz widoku
	matV=glm::rotate(glm::mat4(1.0f),90.0f,glm::vec3(0,0,1));//glm::lookAt(glm::vec3(0.0f,0.0f,7.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f)); 

	//Wylicz macierz modelu
	//matM=glm::rotate(glm::mat4(1.0f),angle,glm::vec3(0.5,1,0)); 
	//Narysuj obiekt
	glClearColor(0,0,0,1);
	DrawText();
	//glClearColor(0,0,0,1);
		for(int i=0;i<2;i++)
	{
		if(players[i]->lives>0)
		drawObjectP(i,1);
	}
	for(int i=2;i<postacie.size();i++)
	{//cout<<"1"<<endl;

		drawObjectP(i,0);
	}

	for(int i=0;i<obiekty.size();i++)
	{//cout<<"1"<<endl;
		drawObject(i);
	}
	
	for(int i=0;i<players[0]->lives;i++)
	{//cout<<"1"<<endl;
		drawObjectL(i);
	}
	for(int i=3;i<players[1]->lives+3;i++)
	{//cout<<"1"<<endl;
		drawObjectL(i);
	}
	//Tylny bufor na przedni
	//glutSwapBuffers();

}

void onDisplay() {
 // logic();
	
	 glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  displayFrame();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	
  glUseProgram(program_postproc);
  glBindTexture(GL_TEXTURE_2D, fbo_texture);
  glUniform1i(uniform_fbo_texture, /*GL_TEXTURE*/0);
  glEnableVertexAttribArray(attribute_v_coord_postproc);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
  glVertexAttribPointer(
    attribute_v_coord_postproc,  // attribute
    2,                  // number of elements per vertex, here (x,y)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisableVertexAttribArray(attribute_v_coord_postproc);
	
  glutSwapBuffers();
}




//Procedura wywo³ywana przy zmianie rozmiaru okna
void changeSize(int w, int h) {
	//Ustawienie wymiarow przestrzeni okna
	glViewport(0,0,w,h);
	//Zapamiêtanie nowych wymiarów okna dla poprawnego wyliczania macierzy rzutowania
	windowWidth=w;
	windowHeight=h;
	  glBindTexture(GL_TEXTURE_2D, fbo_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, windowWidth, windowHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
void cleanObiectMap()
{
	while(obiekty.size()!=1)
	{
		obiekty.pop_back();
	}
	cout<<obiekty.size()<<endl;
}
void nextLevel()
{
	cout<<"new Level"<<endl;
	initPostProgram("fshaderPostWave.txt");
	
	 cleanObiectMap();
	initObiectMap();
	cout<<obiekty.size()<<endl;
	speedWave +=1;
}
int mod(int i,int j)
{
	return i%j;
}
void calcMat2(int i,int interval)
{
	
	if ((postacie[i].anim==1)and(postacie[i].blend<1)) 
	{
	 postacie[i].blend+=speed*interval/1000.0;
	//cout<<postacie[i].blend<<" ";
	}

	
	int st = players[i]->pM;
	players[i]->pM=players[i]->pad(objectMap,obiekty,postacie[i].x,postacie[i].y, b);
	if(st!=players[i]->pM)
	{
	switch(players[i]->pM )
	{
	case 5:
	
		postacie[i].blend=0;
		postacie[i].anim=0;
		postacie[i].matM = postacie[i].matM2;
		if(obiekty.size()==1)nextLevel();
		break;	
	
	case 0:
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f, 0.f+spaceY, 0.f));
		players[i]->matR = glm::mat4(1.0f);
		postacie[i].anim =1;
		postacie[i].x++;
		break;
	case 1:
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f, 0.f-spaceY, 0.f));
		postacie[i].anim =1;
		players[i]->matR =glm::rotate( glm::mat4(1.0f) ,180.f,glm::vec3(1.f,0.f,0.f));
		postacie[i].x--;
		break;
	case 2:
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f+2.954, 0.f, 0.f));
		postacie[i].anim =1;
		players[i]->matR =glm::rotate( glm::mat4(1.0f) ,270.f,glm::vec3(0.f,0.f,1.f));
		postacie[i].y++;
		break;
	case 3:
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f-2.954, 0.f, 0.f));
		postacie[i].anim =1;
		players[i]->matR =glm::rotate( glm::mat4(1.0f) ,90.f,glm::vec3(0.f,0.f,1.f));
		postacie[i].y--;
		break;

	/*case 0:
		postacie[i].blend=0;
		postacie[i].anim=0;
		postacie[i].matM = postacie[i].matM2;
		if(obiekty.size()==1)nextLevel();
		b_st=b;
		break;*/
	}
	}
}
int checkMouse(int i,int st)
{
	/*cout<<postacie[i].x<<" "<<postacie[i].y<<" ";
	try
	{
		cout<<objectMap[postacie[i].x][postacie[i].y]<<endl;
	}
	catch(int e)
	{
		cout<<" error "<<e<<endl;
	}*/

	while(1){
	int r =rand()%4+1;//cout<<r<<endl;
	if(rand()%5<2) r =st;
	switch(r)
	{
		case 1:
			if((postacie[i].x+1<17)and(objectMap[postacie[i].x+1][postacie[i].y]!= 0)) return r; 
//cout<<postacie[i].x<<" "<<postacie[i].y<<" _ "<<objectMap[postacie[i].x][postacie[i].y+1]<<endl;
		break;
		case 2:
			if((postacie[i].x>0)and(objectMap[postacie[i].x-1][postacie[i].y]!= 0)) return r; 
//cout<<postacie[i].x<<" "<<postacie[i].y<<" _ "<<objectMap[postacie[i].x][postacie[i].y-1]<<endl;
		break;
		case 3:
			if((postacie[i].y+1<19)and(objectMap[postacie[i].x][postacie[i].y+1]!= 0)) return r; 
//cout<<postacie[i].x<<" "<<postacie[i].y<<" _ "<<objectMap[postacie[i].x+1][postacie[i].y]<<endl;
		break;
		case 4:
			if((postacie[i].y>0)and(objectMap[postacie[i].x][postacie[i].y-1]!= 0)) return r; 
//cout<<postacie[i].x<<" "<<postacie[i].y<<" _ "<<objectMap[postacie[i].x-1][postacie[i].y]<<endl;
		break;
	}
	
	} 
}
void onDisplayMenu()
{
}
void gameOver()
{
	cout<<"game over"<<endl;
	//glutDisplayFunc(onDisplayMenu);
}

void kill(int k)
{
	if(players[k]->lives>0)
	{
		players[k]->lives--;
		objectMap[postacie[k].x][postacie[k].y] = 100;
		postacie[k].x = players[k]->xx;
		postacie[k].y = players[k]->yy;
		glm::mat4 M = glm::scale(glm::mat4(1.f), glm::vec3(0.03f, 0.03f, 0.f));
		M = glm::translate(M,glm::vec3(26.4f, 16.9f, 0.f));
		M = glm::translate(M,glm::vec3(0.-postacie[k].y*spaceX, 0.f-postacie[k].x*spaceY, 0.f));
		M = glm::rotate(M ,180.f,glm::vec3(0.f,1.f,0.f));
		M = glm::rotate(M ,180.f,glm::vec3(1.f,0.f,0.f));
		postacie[k].matM =M;
		postacie[k].matM2=M;
		objectMap[postacie[k].x][postacie[k].y] = -k-2;
	}
	if((players[0]->lives==0) and (players[1]->lives ==0))
	{
		gameOver();
		
	}
}
void calcMat2Mouse(int i,int interval)
{
	
	if (postacie[i].blend<1)
	{
	 postacie[i].blend+=speed*interval/10000.0*(1+speedWave*5);
	//cout<<postacie[i].blend<<" ";
	}
	else{	//cout<<"mysz idzie"<<endl;
		for(int j=0;j<2;j++)
		{
			if((postacie[i].x ==postacie[j].x)and(postacie[i].y ==postacie[j].y))
				{
					cout<<"kill "<<j<<" "<<postacie[i].y<<" "<< postacie[i].x<<endl;	
					kill(j);				
				}
		} 
		postacie[i].anim=0;
	}
	//int st = players[i]->pM;
	//players[i]->pM=players[i]->pad(objectMap,obiekty);
	if(postacie[i].anim>-1)
	{
	switch(postacie[i].anim)
	{
	case 1://->
		postacie[i].st_anim=postacie[i].anim;
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f, 0.f+spaceY, 0.f));
		postacie[i].anim =-1;
		postacie[i].x++;
		break;
	case 2://<-
		postacie[i].st_anim=postacie[i].anim;
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f, 0.f-spaceY, 0.f));
		postacie[i].anim =-1;
		postacie[i].x--;
		break;
	case 3://v
		postacie[i].st_anim=postacie[i].anim;
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f+2.954, 0.f, 0.f));
		postacie[i].anim =-1;
		postacie[i].y++;
		break;
	case 4://^
		postacie[i].st_anim=postacie[i].anim;
		postacie[i].matM2 = glm::translate(postacie[i].matM ,glm::vec3(0.f-2.954, 0.f, 0.f));
		postacie[i].anim =-1;
		postacie[i].y--;
		break;
	case 0:
		//cout<<"teraz koncze animacje"<<endl;
		postacie[i].blend=0; 
		postacie[i].anim=checkMouse(i,postacie[i].st_anim);
		//cout<<"w: "<<postacie[i].anim<<endl;
		postacie[i].matM = postacie[i].matM2;
		break;
	}
	}
}
void mouseBack(bool back)
{

	if(mouse_back xor back)
	{
		for( int i=0;i<mouse_count;i++)postacie[i+2].swapTex();
		if(back)
		{
			glm::mat4 M = glm::scale(glm::mat4(1.f), glm::vec3(0.03f, 0.03f, 0.f));
			M = glm::translate(M,glm::vec3(26.4f, 16.9f, 0.f));
			M = glm::translate(M,glm::vec3(0.-8*spaceX, 0.f-8*spaceY, 0.f));
			M = glm::rotate(M ,180.f,glm::vec3(0.f,1.f,0.f));
			M = glm::rotate(M ,180.f,glm::vec3(1.f,0.f,0.f));
			for( int i=0;i<mouse_count;i++)
			{
				postacie[i+2].x = 8;postacie[i+2].y = 8;
				postacie[i+2].matM2=M;
				postacie[i+2].blend=0;
			}
		}
	}
	mouse_back = back;
}
void calcMat2MouseBack(int i,int interval)
{
	if (postacie[i].blend<1)
	{
	 postacie[i].blend+=speed*interval/100000.0*(1+speedWave*5);
	//cout<<postacie[i].blend<<" ";
	}
	else
	{	
		postacie[i].matM=postacie[i].matM2;
		postacie[i].blend = 0;
		mouseBack(false);
	}
}
//Procedura uruchamiana okresowo. Robi animacjê.
void nextFrame(void) {
	int actTime=glutGet(GLUT_ELAPSED_TIME);
	int interval=actTime-lastTime;
	lastTime=actTime;
	swapT+=interval;
	if(swapT>250)
	{
		swapT =0;
		postacie[0].swapTex();
		postacie[1].swapTex();
	}
	  glUseProgram(program_postproc);
  GLfloat move = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 2*3.14159 * .75*speedWave;  // 3/4 of a wave cycle per second
  glUniform1f(uniform_offset, move);
	
	for(int i=0;i<2;i++)
	{//cout<<"1"<<endl;
		if(players[i]->lives>0)calcMat2(i,interval);
	}
	for(int i=0;i<mouse_count;i++)
	{//cout<<"1"<<endl;
		if(mouse_back) calcMat2MouseBack(i+2,interval);
		else calcMat2Mouse(i+2,interval);
	}
	glutPostRedisplay();
}

bool loadMap(char *name)
{		
	std::ifstream plik;
	plik.open(name);
	for(int i =0;i<17;i++)
	{
		for(int j=0;j<19;j++)
		{
			if( !plik.good() )
         	return false;
			plik>>objectMap[i][j];
		}
	}
	return true;
}
void addPlayer(OBJ *o,string texName)
{
	OBJ *o2= new OBJ();
	o2->readTexture(texName,0);
	o2->readTexture("./texture/pakman2.tga",1);
	o2->setupOBJ(o);
	o2->setupShaders("vshaderPR.txt",NULL,"fshader.txt");
	postacie.push_back(*o2);
}
void addMouse(OBJ *o,OBJ *o3)
{
	OBJ *o2= new OBJ();
	o2->setupTex(o3);
	o2->setupOBJ(o);
	o2->setupShaders("vshaderP.txt",NULL,"fshader.txt");
	
	postacie.push_back(*o2);
}
void addLivesMap(glm::mat4 *M3,float i,float j,int nr)
{
	glm::mat4 M2= glm::translate(*M3,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
	M2  = glm::rotate(M2 ,180.f,glm::vec3(0.f,1.f,0.f));
	M2  = glm::rotate(M2 ,180.f,glm::vec3(1.f,0.f,0.f));
	lives[nr].matM =M2;
}
void loadLives(OBJ *o,OBJ *o3)
{
	OBJ *o2 = new OBJ();
	o2->setupOBJ(o);
	o2->setupTex(o);
	o2->setupShaders(o3);
	lives.push_back(*o2);
}
void addPlayerMap(glm::mat4 *M3,int i,int j,int nr)
{

	//glm::mat4 M2;
	//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
	glm::mat4 M2= glm::translate(*M3,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
	M2  = glm::rotate(M2 ,180.f,glm::vec3(0.f,1.f,0.f));
	M2  = glm::rotate(M2 ,180.f,glm::vec3(1.f,0.f,0.f));
	postacie[nr].x = i;postacie[nr].y = j;
	postacie[nr].matM =M2;
	postacie[nr].matM2=M2;
	
}
void initObiectMap()
{
	loadMap("./mapa");
	glm::mat4 M = glm::scale(glm::mat4(1.f), glm::vec3(0.03f, 0.03f, 0.f));
	M = glm::translate(M,glm::vec3(26.4f, 16.9f, 0.f));
	OBJ *o = &obiekty[0];
	OBJ *beer = new OBJ();
	beer->readTexture("./texture/beer2.tga",0);
	OBJ *shot = new OBJ();
	shot->readTexture("./texture/shot.tga",0);
	
	for(int i =0;i <17;i++)
	{//cout<<endl;
		for(int j =0;j <19;j++)
		{
			//cout<<objectMap[i][j]<<" ";
			if(objectMap[i][j]==1)
			{ 	
				OBJ *o2= new OBJ();

				o2->setupTex(beer);
				o2->setupOBJ(o);
	
				//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
				o2->matM = glm::translate(M,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
				o2->matM  = glm::rotate(o2->matM ,180.f,glm::vec3(0.f,1.f,0.f));
				o2->x = i;o2->y = j;
				obiekty.push_back(*o2);
			}
			else if(objectMap[i][j]==2)
			{
				OBJ *o2= new OBJ();
				o2->setupTex(shot);
				o2->setupOBJ(o);
	
				//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
				o2->matM = glm::translate(M,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
				o2->matM  = glm::rotate(o2->matM ,180.f,glm::vec3(0.f,1.f,0.f));
				o2->x = i;o2->y = j;
				obiekty.push_back(*o2);
			}
			else if(objectMap[i][j]==3)
			{

				OBJ *o2= new OBJ();
				o2->readTexture("./texture/food1.tga",0);
				o2->setupOBJ(o);
	
				//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
				o2->matM = glm::translate(M,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
				o2->matM  = glm::rotate(o2->matM ,180.f,glm::vec3(0.f,1.f,0.f));
				o2->x = i;o2->y = j;
				obiekty.push_back(*o2);
			}
			else if(objectMap[i][j]==4)
			{

				OBJ *o2= new OBJ();
				o2->readTexture("./texture/food2.tga",0);
				o2->setupOBJ(o);
	
				//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
				o2->matM = glm::translate(M,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
				o2->matM  = glm::rotate(o2->matM ,180.f,glm::vec3(0.f,1.f,0.f));
				o2->x = i;o2->y = j;
				obiekty.push_back(*o2);
			}
			else if(objectMap[i][j]==5)
			{

				OBJ *o2= new OBJ();
				o2->readTexture("./texture/food3.tga",0);
				o2->setupOBJ(o);
	
				//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
				o2->matM = glm::translate(M,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
				o2->matM  = glm::rotate(o2->matM ,180.f,glm::vec3(0.f,1.f,0.f));
				o2->x = i;o2->y = j;
				obiekty.push_back(*o2);
			}
			else if(objectMap[i][j]==6)
			{

				OBJ *o2= new OBJ();
				o2->readTexture("./texture/food4.tga",0);
				o2->setupOBJ(o);
	
				//o2->matM = glm::translate(o2->matM,glm::vec3(0.48f, 0.83f, 0.f));
				o2->matM = glm::translate(M,glm::vec3(0.-j*spaceX, 0.f-i*spaceY, 0.f));
				o2->matM  = glm::rotate(o2->matM ,180.f,glm::vec3(0.f,1.f,0.f));
				o2->x = i;o2->y = j;
				obiekty.push_back(*o2);
			}
			else if(objectMap[i][j]==-2)
			{
			
				addPlayerMap(&M,i,j,0);
				players[0]->setPlayerNumber(0,i,j);
			}
			else if(objectMap[i][j]==-3)
			{
			
				addPlayerMap(&M,i,j,1);
				players[1]->setPlayerNumber(0,i,j);
			}
		}
	}
	
}
void loadObject()
{
		objectMap = new int*[17];
	for(int i=0;i<17;i++)objectMap[i] = new int[19];
	

	OBJ *o= new OBJ("./plane.obj");
	o->setupShaders("vshader.txt",NULL,"fshader.txt");
	o->readTexture("./texture/tlo2.tga",0);
	o->matM = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.1f));//środek 256
	o->x=-100;
	obiekty.push_back(*o);
	glm::mat4 M = glm::scale(glm::mat4(1.f), glm::vec3(0.03f, 0.03f, 0.f));
	M = glm::translate(M,glm::vec3(26.4f, 16.9f, 0.f));
	
	addPlayer(o,"./texture/pakman.tga");
	addPlayer(o,"./texture/pakmanG.tga");
	for(int i=0;i<mouse_count;i++)
	{
		OBJ *o2= new OBJ();
		o2->readTexture("./texture/Mouse.tga",0);
		o2->readTexture("./texture/Mouse2.tga",1);
		addMouse(o,o2);
		addPlayerMap(&M,8,8,i+2);
		postacie[i+2].blend=2;
		postacie[i+2].anim =1;
	}
	for(int i=0;i<players[0]->lives;i++)
	{
		loadLives(&postacie[0],&obiekty[0]);
		addLivesMap(&M,-4+i,-0.4f,i);
	}

	for(int i=3;i<players[1]->lives+players[0]->lives;i++)
	{
		loadLives(&postacie[1],&obiekty[0]);
		addLivesMap(&M,18.5+i,-0.4f,i);
	}
	cout<<"player add"<<endl;
	initObiectMap();
	
}
void initText()
{
	shaderText = new ShaderProgram("vshaderText.txt",NULL,"fshaderText.txt");
}
//Procedura inicjuj¹ca biblotekê glut
void initGLUT(int *argc, char** argv) {
	glutInit(argc,argv); //Zainicjuj bibliotekê GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //Alokuj bufory kolorów (podwójne buforowanie) i bufor kolorów
	
	glutInitWindowPosition(windowPositionX,windowPositionY); //Wska¿ pocz¹tkow¹ pozycjê okna
	glutInitWindowSize(windowWidth,windowHeight); //Wska¿ pocz¹tkowy rozmiar okna
	glutCreateWindow("OpenGL 3.3"); //Utwórz okno i nadaj mu tytu³
//	glutIdleFunc(pad);
	glutReshapeFunc(changeSize); //Zarejestruj procedurê changeSize jako procedurê obs³uguj¹ca zmianê rozmiaru okna
	glutDisplayFunc(onDisplay); //Zarejestruj procedurê displayFrame jako procedurê obs³uguj¹ca odwierzanie okna
	glutIdleFunc(nextFrame); //Zarejestruj procedurê nextFrame jako procedurê wywo³ywan¹ najczêciêj jak siê da (animacja)
}


//Procedura inicjuj¹ca bibliotekê glew
void initGLEW() {
	GLenum err=glewInit();
	if (GLEW_OK!=err) {
		fprintf(stderr,"%s\n",glewGetErrorString(err));
		exit(1);
	}
}

int initPostProces()
{
	/* init_resources */
  /* Create back-buffer, used for post-processing */

  /* Texture */
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &fbo_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Depth buffer */
  glGenRenderbuffers(1, &rbo_depth);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, windowWidth, windowHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  /* Framebuffer to link everything together */
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
  GLenum status;
  if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
    return 0;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void freePostProces()
{
  glDeleteRenderbuffers(1, &rbo_depth);
  glDeleteTextures(1, &fbo_texture);
  glDeleteFramebuffers(1, &fbo);
}
void initPostVertices()
{
	 GLfloat fbo_vertices[] = {
    -1, -1,
     1, -1,
    -1,  1,
     1,  1,
  };
  glGenBuffers(1, &vbo_fbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void freePostVertices()
{
	 glDeleteBuffers(1, &vbo_fbo_vertices);
}
int initPostProgram(const char* fFilename)
{
	GLuint vs, fs;
	  GLint link_ok = GL_FALSE;
  GLint validate_ok = GL_FALSE;
	 const char* uniform_name;
	 const char* attribute_name;
	/* init_resources */
  /* Post-processing */
	//shaderPostProces = new ShaderProgram("vshaderPost.txt",NULL,"fshaderPost.txt");
	  if ((vs = create_shader("vshaderPost.txt", GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs = create_shader(fFilename, GL_FRAGMENT_SHADER)) == 0) return 0;

  program_postproc = glCreateProgram();
  glAttachShader(program_postproc, vs);
  glAttachShader(program_postproc, fs);
  glLinkProgram(program_postproc);
  glGetProgramiv(program_postproc, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    print_log(program_postproc);
    return 0;
  }
  glValidateProgram(program_postproc);
  glGetProgramiv(program_postproc, GL_VALIDATE_STATUS, &validate_ok); 
  if (!validate_ok) {
    fprintf(stderr, "glValidateProgram:");
    print_log(program_postproc);
  }

  attribute_name = "v_coord";
  attribute_v_coord_postproc = glGetAttribLocation(program_postproc, attribute_name);
  if (attribute_v_coord_postproc == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  uniform_name = "fbo_texture";
  uniform_fbo_texture = glGetUniformLocation(program_postproc, uniform_name);
  if (uniform_fbo_texture == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }
   uniform_name = "offset";
  uniform_offset = glGetUniformLocation(program_postproc, uniform_name);
  if (uniform_offset == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }
}
void free_all()
{
	freePostProces();
	freePostVertices();
	delete(text);
}
int main(int argc, char** argv) {
	
	srand(time(NULL));
	initGLUT(&argc,argv);
	initGLEW();
	glEnable(GL_DEPTH_TEST);//cout<<"1"<<endl;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
	initText();

	players= new Player*[2];
	players[0] = new Player(JOY_DEV);
	players[1] = new Player("/dev/input/js1");
	players[0]->id=-2;
	players[1]->id=-3;
	loadObject();cout<<"oblect load"<<endl;
	
	//players = new Player(JOY_DEV);
	initPostProces();cout<<"ipp"<<endl;
	initPostVertices();cout<<"ipv"<<endl;
	initPostProgram("fshaderPost.txt");cout<<"ipp"<<endl;
	text = new Text();
	text->setFont("FreeSans.ttf");
	text->init_resources();

	glutMainLoop();
	  //close( joy_fd ); 
	free_all();
	return 0;
}
