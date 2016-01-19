#ifndef BOMB_H
#define BOMB_H

#include "Block.h"
#include "Coordinate.h"
#include "bitmap.h"

class Bomb: public Block {
private:
	bool isInitialized;

	// texture stuff
	GLubyte	    *bits_;
	BITMAPINFO	*info_;
	GLenum      type_;
	GLuint      *texture_;
	GLubyte	    *rgba_;
	GLubyte	    *rgbaptr_;
	GLubyte     *ptr_;

public:
	// constructs a new bomb in the position
	Bomb(Coordinate position);

	// constructs a bomb without position
	Bomb();

	// initialize the model
	void init();
	void loadTexture(GLuint *texture);
	void draw();
};

#endif