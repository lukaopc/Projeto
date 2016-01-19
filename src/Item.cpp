#include "Item.h"
#include <iostream>

Item::Item(Coordinate position, ItemType itemType) : Block(position, 1.0f) {
	isInitialized = false;
	type_ = itemType;
}

void Item::init() {
	if(!isInitialized) {
		// create a smart pointer to the model
		std::shared_ptr<C3DObject> model = std::make_shared<C3DObject>();

		if(type() == FASTER_ITEM){
			if (!model->Load("../resources/models/dolphins.obj")) {
				std::cout << "Problem loading faster item model!" << std::endl;
				return;
			}
		} else if(type() == BOMB_ITEM){
			if (!model->Load("../resources/models/dolphins.obj")) {
				std::cout << "Problem loading bomb item model!" << std::endl;
				return;
			}
		} else if(type() == RANGE_ITEM){
			if (!model->Load("../resources/models/dolphins.obj")) {
				std::cout << "Problem loading range item model!" << std::endl;
				return;
			}
		}


		// set the model of the block
		setModelObject(model);

		isInitialized = true;
	}
}

ItemType Item::type(){
	return type_;
}