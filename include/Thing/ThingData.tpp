//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_THING_T_TPP
#define QUANFLOQ_THING_T_TPP

#include "ThingData.hpp"
#include "Registrar/TypeInfo.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class T>
DataBase<T>::DataBase( T& parent ):
		BaseData(parent), parent{parent} { }
template<class Traits>
ThingData_<Traits>::ThingData_( thing_type& parent ) :
		base(parent) {
	static_assert(std::derived_from<data_type, alias>,
	              "Traits::data_type does not derive ThingData<Traits>");
}
// endregion

// region Static initializations
template<class Traits>
inline const TypeInfo* const ThingData_<Traits>::typeInfo = &TypeInfo::Create<data_type, thing_type>("Data");
template<class Traits>
inline typename ThingData_<Traits>::factory_type ThingData_<Traits>::factory;
// endregion

// region Interfaces
template<class Traits>
std::unique_ptr<typename Traits::data_type> ThingDataFactory_<Traits>::Make( IExposable* parent ) {
	static_assert(std::constructible_from<data_type, thing_type&>,
	              "data_type is not standard constructible\n"
	              "Can simply add using alias::alias to include the default constructor");
	auto thingPtr = dynamic_cast<thing_type*>(parent);
	assert(thingPtr);
	return std::make_unique<data_type>(*thingPtr);
}
template<class Traits>
ThingDataFactory_<Traits>::ThingDataFactory_() {
	this->name = "ThingDataFactory/" + data_type::typeInfo->name;
	BaseFactory::registrar.RegisterName(this->name, *this);
}
// endregion

#endif //QUANFLOQ_THING_T_TPP
