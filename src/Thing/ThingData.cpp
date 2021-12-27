//
// Created by lecris on 2021-11-10.
//

#include "Thing/ThingData.hpp"
#include "Thing/Thing.hpp"

using namespace QuanFloq;

BaseData::BaseData( BaseThing& parent ) :
		parent(parent) {
	name = parent.name + "Data";
}
std::string_view BaseData::GetName() const {
	return name;
}
