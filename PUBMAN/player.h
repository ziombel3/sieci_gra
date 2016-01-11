#include "main.h"
#ifndef PLAYER_H
#define PLAYER_H
using namespace std;
class Player {
	private:
	
	int flag=5;
	int joy_fd, *axis=NULL, num_of_axis=0, num_of_buttons=0;
	char *button=NULL, name_of_joystick[80];
	struct js_event js;
	int initPad(char * name);
	int obiektyPop(int **objectMap,vector<OBJ> &obiekty, int x, int y, int b);
	public:
	Player(char *name);
	void setPlayerNumber(int pN,int i,int j){pM=pN;xx=i;yy=j;};
	int pad(int **objectMap,vector<OBJ> &obiekty, int &x, int &y, int &b) ;
	int id,pM;
	int beers=0,shots=0,lives =3;
	int xx,yy;
	glm::mat4  matR;//rotacji
};

#endif
