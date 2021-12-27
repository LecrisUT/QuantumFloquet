//
// Created by lecris on 2021-11-10.
//

#include "interface/IExposable.hpp"
#include "Registrar/TypeInfo.tpp"
#include "interface/Scriber/ScribeDriver.hpp"

using namespace QuanFloq;

void IExposable::ExposeData( Scriber& scriber ) { }

const TypeInfo& IExposable::GetType() const {
	return TypeInfo::registrar[typeid(*this)];
}
IExposable::IExposable() {
	assert(ScribeDriver::registrar.Contains("JSON Driver"));
	format = ScribeDriver::registrar["JSON Driver"];
	assert(format != nullptr);
}
IExposable::IExposable( std::string_view f ) {
	format = ScribeDriver::registrar[f];
	assert(format != nullptr);
}
void IExposable::RegisterName( std::string_view name ) {
	std::string_view name2 = name.empty() ? GetName() : name;
	if (!name2.empty()) {
		auto registrar = GetType().iRegistrar;
		if (registrar != nullptr)
			registrar->RegisterName(name2, this);
	}
}
std::string_view IExposable::GetName() const {
	return "";
}
