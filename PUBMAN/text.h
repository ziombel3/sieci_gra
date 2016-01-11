#include "main.h"
#ifndef TEXT_H
#define TEXT_H


struct point {
	GLfloat x,y,s,t;
};
class Text
{
private:
	GLuint program, attribute_coord, uniform_tex, uniform_color;
	GLuint vbo;

	FT_Library ft;
	FT_Face face;
	const char *fontfilename;
public:
	Text(){};
	~Text() {glDeleteProgram(program);};
	void render_text(const char *text, float x, float y, float sx, float sy) ;
	int init_resources();
	void setFont(const char *fontfilename);
	void initDisplayText(int size);
};
#endif
