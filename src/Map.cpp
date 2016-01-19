#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Map.h"
#include "bitmap.h"

Map::Map() {
	width_ = 0.0f;
	height_ = 0.0f;
}

float Map::width(){
	return width_;
}

float Map::height() {
	return height_;
}

std::shared_ptr<Human> Map::player() {
	return player_;
}

std::vector<std::shared_ptr<Machine>> Map::enemies() {
	return enemies_;
}

std::vector<std::shared_ptr<IndestructibleWall>> Map::indestructibleWalls() {
	return indestructibleWalls_;
}

std::vector<std::shared_ptr<DestructibleWall>> Map::destructibleWalls() {
	return destructibleWalls_;
}

void Map::load() {
	BITMAPINFO	*info;
	GLubyte *bitsOfModelMap = LoadDIBitmap("..\\resources\\images\\map.bmp", &info);
	if (bitsOfModelMap == (GLubyte *)0) {
		std::cout << "Error loading map!" << std::endl;
		return;
	}

	int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
	int linha = -1;
	int coluna = 0;
	GLubyte *rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);
	GLubyte *ptr;
	GLubyte *rgbaptr;
	for(rgbaptr = rgba, ptr = bitsOfModelMap;  i > 0; i--, rgbaptr += 4, ptr += 3) {
		if(coluna % info->bmiHeader.biWidth == 0) {
			coluna = 0;
			linha += 1;
		}

		int red = (int)ptr[2];
		int green = (int)ptr[1];
		int blue = (int)ptr[0];

		// set the width and height
		width_ = Block::EDGE_SIZE * info->bmiHeader.biWidth;
		height_ = Block::EDGE_SIZE * info->bmiHeader.biHeight;

		// convert the pixel location to (x,y,z) coordinate with the center of the image in (0,0,0)
		float x = ((float)coluna)*Block::EDGE_SIZE - (float)width_/2.0f+0.5;
		float y = 1.0f;
		float z = ((float)linha)*Block::EDGE_SIZE - (float)height_/2.0f+0.5;
		Coordinate position = {x, y, z};

		std::cout << "Loading map pixel (" << linha << "," << coluna << ")" << " -> ";
		std::cout << "Created point (" << x << "," << y << "," << z << ")" << std::endl;

		if(red == 0 && blue == 0 && green == 0) {
			// it's an indestructible wall
			std::shared_ptr<IndestructibleWall> wall = std::make_shared<IndestructibleWall>(position);
			wall->init();
			indestructibleWalls_.push_back(wall);

		} else if(red == 0 && blue == 255 && green == 0) {
			// it's a destructible wall
			std::shared_ptr<DestructibleWall> wall = std::make_shared<DestructibleWall>(position);
			wall->init();
			destructibleWalls_.push_back(wall);

		} else if(red == 0 && blue == 0 && green == 255) {
			// it's a machine player (a.k.a. enemy)
			std::shared_ptr<Machine> enemy = std::make_shared<Machine>(position);
			enemy->init();
			enemies_.push_back(enemy);

		} else if(red == 255 && blue == 0 && green == 0) {
			// it's a human player
			if(!player_) {
				player_ = std::make_shared<Human>(position);
				player_->init();
			}
		}

		coluna += 1;
	}

	// hide the items
	int numberOfItens = 12;
	int currentItem = 0;
	std::vector<Coordinate> positions;

	// get the possible positions
	std::vector<std::shared_ptr<DestructibleWall>>::iterator boxesIt;
	for (boxesIt = destructibleWalls_.begin() ; boxesIt != destructibleWalls_.end(); ++boxesIt) {
		positions.push_back((*boxesIt)->position());
	}

	// allocate the itens in the boxes
	for(int i = numberOfItens; i > 0 && positions.size() > 0; i--) {
		currentItem = i % 3;

		int selectedPosition = std::rand() % positions.size();
		Coordinate itemPosition = positions.at(selectedPosition);
		positions.erase(positions.begin() + selectedPosition);

		ItemType itemType = static_cast<ItemType>(currentItem);
		Item item = Item(itemPosition, itemType);
		item.init();
		items_.push_back(item);
	}
}

void Map::reload() {
	player_ = 0;
	enemies_.clear();
	indestructibleWalls_.clear();
	destructibleWalls_.clear();

	load();
}

void Map::draw() {
	// draw the player
	if(player_) {
		player_->draw();
	}

	// draw the enemies
	std::vector<std::shared_ptr<Machine>>::iterator enemiesIt;
	for (enemiesIt = enemies_.begin() ; enemiesIt != enemies_.end(); ++enemiesIt) {
		(*enemiesIt)->draw();
	}

	// draw the walls
	std::vector<std::shared_ptr<IndestructibleWall>>::iterator wallsIt;
	for (wallsIt = indestructibleWalls_.begin() ; wallsIt != indestructibleWalls_.end(); ++wallsIt) {
		(*wallsIt)->draw();
	}

	// draw the items
	std::vector<Item>::iterator itemsIt;
	for (itemsIt = items_.begin() ; itemsIt != items_.end(); ++itemsIt) {
		(*itemsIt).draw();
	}

	// draw the boxes
	std::vector<std::shared_ptr<DestructibleWall>>::iterator boxesIt;
	for (boxesIt = destructibleWalls_.begin() ; boxesIt != destructibleWalls_.end(); ++boxesIt) {
		(*boxesIt)->draw();
	}
}

bool Map::collidesWithSomething(Block &block) {
	return collidesWithWall(block) || collidesWithBox(block)
		|| collidesWithEnemy(block) || collidesWithPlayer(block);
}

bool Map::collidesWithWall(Block &block) {
	std::vector<std::shared_ptr<IndestructibleWall>>::iterator wallsIt;
	for (wallsIt = indestructibleWalls_.begin() ; wallsIt != indestructibleWalls_.end(); ++wallsIt) {
		if((*wallsIt)->collidesWith(block)) {
			return true;
		}
	}

	return false;
}

bool Map::collidesWithBox(Block &block) {
	std::vector<std::shared_ptr<DestructibleWall>>::iterator boxesIt;
	for (boxesIt = destructibleWalls_.begin() ; boxesIt != destructibleWalls_.end(); ++boxesIt) {
		if((*boxesIt)->collidesWith(block)) {
			return true;
		}
	}

	return false;
}

bool Map::collidesWithEnemy(Block &block) {
	std::vector<std::shared_ptr<Machine>>::iterator enemiesIt;
	for (enemiesIt = enemies_.begin() ; enemiesIt != enemies_.end(); ++enemiesIt) {
		if((*enemiesIt)->collidesWith(block)) {
			return true;
		}
	}

	return false;
}

bool Map::collidesWithPlayer(Block &block) {
	if(player_) {
		return player_->collidesWith(block);
	}

	return false;
}

void Map::collectItems() {

	if(player_) {
		// remove the enemies that collides
		std::vector<Item>::iterator itemsIt = items_.begin();
		while(itemsIt != items_.end()) {
			if((*itemsIt).collidesWith(*player_)) {
				player_->addPowerUp((*itemsIt).type());

				switch((*itemsIt).type()) {
				case FASTER_ITEM:
					std::cout << "collected faster item" << std::endl;
					break;
				case BOMB_ITEM:
					std::cout << "collected extra bomb item" << std::endl;
					break;
				case RANGE_ITEM:
					std::cout << "collected explosion range item" << std::endl;
					break;
				}
			
				itemsIt = items_.erase(itemsIt);
			} else {
				++itemsIt;
			}
		}
	}
}

void Map::killWhatCollidesWith(Block &block) {
	if(player_ && collidesWithPlayer(block)) {
		player_ = 0;
	}

	// remove the enemies that collides
	std::vector<std::shared_ptr<Machine>>::iterator enemiesIt = enemies_.begin();
	while(enemiesIt != enemies_.end()) {
		if((*enemiesIt)->collidesWith(block)) {
			enemiesIt = enemies_.erase(enemiesIt);
		} else {
			++enemiesIt;
		}
	}

	// remove the boxes that collides
	std::vector<std::shared_ptr<DestructibleWall>>::iterator boxesIt = destructibleWalls_.begin();
	while(boxesIt != destructibleWalls_.end()) {
		if((*boxesIt)->collidesWith(block)) {
			boxesIt = destructibleWalls_.erase(boxesIt);
		} else {
			++boxesIt;
		}
	}
}

void Map::executeMachine() {
	std::vector<std::shared_ptr<Machine>>::iterator enemiesIt;
	for (enemiesIt = enemies_.begin() ; enemiesIt != enemies_.end(); ++enemiesIt) {
		(*enemiesIt)->move(*this);
	}
}

