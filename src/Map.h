#ifndef MAP_H
#define MAP_H

#include <memory>
#include <vector>
#include "Block.h"
#include "Machine.h"
#include "Human.h"
#include "IndestructibleWall.h"
#include "DestructibleWall.h"
#include "Item.h"

enum SquareType {INDESTRUCTIBLE_WALL, DESTRUCTIBLE_WALL};

typedef struct {
	std::shared_ptr<Block> block;
	Coordinate position;
	SquareType type;
} Square;

class Map {
private:
	std::vector<std::shared_ptr<Machine>> enemies_;
	std::shared_ptr<Human> player_;
	std::vector<std::shared_ptr<IndestructibleWall>> indestructibleWalls_;
	std::vector<std::shared_ptr<DestructibleWall>> destructibleWalls_;
	std::vector<Item> items_;
	float width_;
	float height_;

public:
	Map();
	float width();
	float height();
	std::shared_ptr<Human> player();
	std::vector<std::shared_ptr<Machine>> enemies();
	std::vector<std::shared_ptr<IndestructibleWall>> indestructibleWalls();
	std::vector<std::shared_ptr<DestructibleWall>> destructibleWalls();
	void load();
	void reload();
	void draw();
	bool collidesWithSomething(Block &block);
	bool collidesWithWall(Block &block);
	bool collidesWithBox(Block &block);
	bool collidesWithEnemy(Block &block);
	bool collidesWithPlayer(Block &block);
	void killWhatCollidesWith(Block &block);
	void collectItems();
	void executeMachine();
};

#endif