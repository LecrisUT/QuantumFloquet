//
// Created by lecris on 2021-12-13.
//

#include "Registrar/TypeInfo.hpp"
#include "Registrar/TypeRegistrar.hpp"
#include<algorithm>

using namespace QuanFloq;


bool TypeRegistrar::Contains( const std::type_index& type ) const {
	// TypeInfo is ordered by the human-readable name so cannot use contains
	return std::ranges::any_of(Set, [type]( auto& val ) { return val.type == type; });
}
const TypeInfo& TypeRegistrar::operator[]( const std::type_index& type ) const {
	auto iter = std::ranges::find_if(Set, [type]( auto& val ) { return val.type == type; });
	assert(iter != Set.end());
	return *iter;
}