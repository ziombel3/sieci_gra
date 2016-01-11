#ifndef MAIN_H
#define MAIN_H
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdio.h>
#include "tga.h"
#include "shaderprogram.h"
#include "OBJ.h"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <stdlib.h> 
#include <cstdlib>
#include "player.h"
#include "mouse.h"
#include <time.h>   
#include <ft2build.h>
#include FT_FREETYPE_H
#include "common/shader_utils.h"
#include "text.h"
void initObiectMap();
int initPostProgram(const char* fFilename);
void mouseBack(bool back); 
#endif
