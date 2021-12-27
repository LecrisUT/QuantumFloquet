//
// Created by Le Minh Cristian on 2021/11/04.
//

#include "Thing/CompDef.hpp"
#include "Thing/ThingDef.hpp"
#include "Thing/CompDriver.hpp"
#include "interface/Scriber/Scriber.tpp"
#include "Registrar/TypeInfo.tpp"
#include "interface/Scriber/JSONDriver.hpp"

using namespace QuanFloq;

BaseCompDef::BaseCompDef( BaseFactory& factory ) :
		compFactory{factory} { }

void BaseCompDef::ExposeData( Scriber& base ) {
	base.Scribe("Name", name);
	if (base.state == Load)
		registrar.RegisterName(name, this);
	base.Scribe("Library", driverLibrary);
	base.Scribe("CompClass", comp);
	base.Scribe("Driver", driver);
	base.Scribe("Parent", parent);
	base.Scribe("Scriber", scriberDriver);
}
std::string_view BaseCompDef::GetName() const {
	return name;
}
std::vector<std::shared_ptr<BaseCompDef>> BaseCompDef::LoadDefs( std::string_view location ) {
	auto jsonDriver = std::static_pointer_cast<JSONDriver>(JSONDriver::registration.pointer);
	auto scriber = jsonDriver->GenScriber(Load, location);
	std::vector<std::shared_ptr<BaseCompDef>> defs;
	scriber->Scribe("CompDefs", defs, true);
	return defs;
}
