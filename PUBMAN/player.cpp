#include "player.h"

Player::Player(char * name)
{
	
	initPad(name);
}
int Player::initPad(char * name)
{
		
        if( ( joy_fd = open( name , O_RDONLY)) == -1 )
        {
                printf( "Couldn't open joystick\n" );
                return -1;
        }

        ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
        ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
        ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

        axis = (int *) calloc( num_of_axis, sizeof( int ) );
        button = (char *) calloc( num_of_buttons, sizeof( char ) );

        printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
                , name_of_joystick
                , num_of_axis
                , num_of_buttons );
}
int Player::obiektyPop(int **objectMap,vector<OBJ> &obiekty, int x, int y, int b)
{
	
	if(objectMap[x][y]<100)
	{
			for(int i=0;i<obiekty.size();i++)
				if((obiekty[i].x==x) and(obiekty[i].y==y)) 
				{//cout<<i<<": "<<obiekty[i].x<<" "<<obiekty[i].y<<endl;
					obiekty[i]= obiekty[obiekty.size()-1];
					//cout<<i<<": "<<obiekty[i].x<<" "<<obiekty[i].y<<endl;
					//if(pM==obiekty.size()-1)pM=i;
					obiekty.pop_back();
					
					break;
				}
	if(objectMap[x][y]==1) beers++;
	else if(objectMap[x][y]==2) {shots++;b=-b;}
	else if((objectMap[x][y]>2)and(objectMap[x][y]<7)) mouseBack(true);
	}
	objectMap[x][y]=id;
	return b;
}
int Player::pad(int **objectMap,vector<OBJ> &obiekty, int &x, int &y, int &b) 
{
	fcntl( joy_fd, F_SETFL, O_NONBLOCK ); 
                        /* read the joystick state */
	read(joy_fd, &js, sizeof(struct js_event));
              /* see what to do with the event */
	switch (js.type & ~JS_EVENT_INIT)
	{
		case JS_EVENT_AXIS:
		axis   [ js.number ] = js.value;
		break;
		case JS_EVENT_BUTTON:
		button [ js.number ] = js.value;
		break;
	}
	if((x<16)and(b*axis[5]>0)and(flag==5)and(objectMap[x+1][y]>0))
	{
		//obiekty[pM].matM = glm::translate(obiekty[pM].matM ,glm::vec3(0.f, 0.f+2.09, 0.f));
		flag =0;
		
		
		b=obiektyPop(objectMap,obiekty, x+1, y,b);
		objectMap[x][y]=100;
		
		//cout<<"1-"<<x<<" "<<y<<endl<<objectMap[x][y]<<" "<<id<<endl;
					//glutPostRedisplay();
	}
	else if((x>0)and(b*axis[5]<0)and(flag==5)and(objectMap[x-1][y]>0))
	{
		//obiekty[pM].matM = glm::translate(obiekty[pM].matM ,glm::vec3(0.f, 0.f-2.09, 0.f));
		flag =1;
		
		b=obiektyPop(objectMap,obiekty, x-1, y,b);
		objectMap[x][y]=100;
		//cout<<"2-"<<x<<" "<<y<<endl<<objectMap[x][y]<<" "<<id<<endl;
		}
	else if((y<18)and(b*axis[6]>0)and(flag==5)and(objectMap[x][y+1]>0))
	{
		//obiekty[pM].matM = glm::translate(obiekty[pM].matM ,glm::vec3(0.f+2.954, 0.f, 0.f));
		flag =2;
		
		
		b=obiektyPop(objectMap,obiekty, x, y+1,b);
		objectMap[x][y]=100;
		//cout<<"3-"<<x<<" "<<y<<endl<<objectMap[x][y]<<" "<<id<<endl;
	}
	else if((y>0)and(b*axis[6]<0)and(flag==5)and(objectMap[x][y-1]>0))
	{
			//obiekty[pM].matM = glm::translate(obiekty[pM].matM ,glm::vec3(0.f-2.954, 0.f, 0.f));
			flag =3;
			
			b=obiektyPop(objectMap,obiekty, x, y-1,b);
			objectMap[x][y]=100;
			//cout<<"4-"<<x<<" "<<y<<endl<<objectMap[x][y]<<" "<<id<<endl;
	}
	else if((axis[5]==0)and(flag!=5)and (axis[6]==0))flag =5;
	return flag;
}
