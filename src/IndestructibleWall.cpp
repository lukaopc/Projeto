#include "IndestructibleWall.h"
#include <iostream>

IndestructibleWall::IndestructibleWall(Coordinate position) : Block(position, 1.5f) {
	isInitialized = false;
}

void IndestructibleWall::init() {
	if(!isInitialized) {
		// create a smart pointer to the model
		std::shared_ptr<C3DObject> model = std::make_shared<C3DObject>();
		
		if (!model->Load("../resources/models/box.obj")) {
			std::cout << "Problem loading indestructible wall model!" << std::endl;
			return;
		}

		// set the model of the block
		setModelObject(model);

		isInitialized = true;
	}
}