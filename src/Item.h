#ifndef ITEM_H
#define ITEM_H

#include "Block.h"
#include "Coordinate.h"

enum ItemType {FASTER_ITEM, BOMB_ITEM, RANGE_ITEM};

class Item: public Block {
private:
	bool isInitialized;
	
	ItemType type_;

public:
	// constructs a new item in the position
	Item(Coordinate position, ItemType itemType);

	// initialize the model
	void init();

	ItemType type();
};

#endif