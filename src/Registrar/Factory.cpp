//
// Created by lecris on 2021-12-23.
//

#include "Registrar/Factory.hpp"

using namespace QuanFloq;

std::string_view BaseFactory::GetName() const {
	return name;
}