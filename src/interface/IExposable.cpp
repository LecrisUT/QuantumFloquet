//
// Created by lecris on 2021-11-10.
//

#include "interface/IExposable.hpp"
#include "Registrar/TypeInfo.hpp"
#include "interface/Scriber/ScribeDriver.hpp"

using namespace QuanFloq;

void IExposable::ExposeData( Scriber& scriber ) { }

const TypeInfo& IExposable::GetType() const {
	return TypeInfo::registrar[typeid(*this)];
}
IExposable::IExposable() {
	assert(ScribeDriver::registrar.Contains("JSON Driver"));
	Format = ScribeDriver::registrar["JSON Driver"];
	assert(Format != nullptr);
}
IExposable::IExposable( std::string_view format ) {
	Format = ScribeDriver::registrar[format];
	assert(Format != nullptr);
}
std::string IExposable::GetBaseName() const {
	return GetType().name;
}
