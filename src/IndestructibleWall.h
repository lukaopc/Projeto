#ifndef INDESTRUCTIBLEWALL_H
#define INDESTRUCTIBLEWALL_H

#include "Block.h"
#include "Coordinate.h"

class IndestructibleWall: public Block {
private:
	bool isInitialized;

public:
	// constructs a new indestructible wall in the position
	IndestructibleWall(Coordinate position);

	// initialize the model
	void init();
};

#endif