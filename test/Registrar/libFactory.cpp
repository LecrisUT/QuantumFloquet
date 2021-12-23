//
// Created by lecris on 2021-12-13.
//

#include "libFactory.hpp"
#include "Registrar/Factory.hpp"


using namespace QuanFloq;

template<>
std::string_view Factory<A>::GetName() const {
	return "AFactory";
}
