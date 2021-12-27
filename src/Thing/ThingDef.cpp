//
// Created by Le Minh Cristian on 2021/11/04.
//

#include "Thing/ThingDef.hpp"
#include "Thing/CompDef.hpp"
#include "Thing/ThingDriver.hpp"
#include "Registrar/TypeInfo.tpp"
#include "interface/Scriber/Scriber.tpp"
#include "interface/Scriber/JSONDriver.hpp"

using namespace QuanFloq;

BaseDef::BaseDef( BaseFactory& factory ) :
		thingFactory{factory} { }

void BaseDef::ExposeData( Scriber& base ) {
	base.Scribe("Name", name);
	if (base.state == Load)
		registrar.RegisterName(name, this);
	base.Scribe("Library", driverLibrary);
	base.Scribe("ThingClass", thing);
	base.Scribe("Driver", driver);
	base.Scribe("CompDefs", comps);
	base.Scribe("Scriber", scriberDriver);
}
std::string_view BaseDef::GetName() const {
	return name;
}

std::vector<std::shared_ptr<BaseDef>> BaseDef::LoadDefs( std::string_view location ) {
	auto jsonDriver = std::static_pointer_cast<JSONDriver>(JSONDriver::registration.pointer);
	auto scriber = jsonDriver->GenScriber(Load, location);
	std::vector<std::shared_ptr<BaseDef>> defs;
	scriber->Scribe("ThingDefs", defs, true);
	return defs;
}
