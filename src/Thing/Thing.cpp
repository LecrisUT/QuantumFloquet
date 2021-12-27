//
// Created by Le Minh Cristian on 2021/11/04.
//

#include "Thing/Thing.hpp"
#include "Thing/ThingDef.hpp"
#include "Thing/ThingData.hpp"
#include "Thing/ThingComp.hpp"
#include "interface/Scriber/Scriber.tpp"
#include "Registrar/Factory.tpp"

using namespace QuanFloq;

BaseThing::BaseThing( std::shared_ptr<BaseDef> def_ ) :
		def(std::move(def_)) {
	assert(def != nullptr);
};

void BaseThing::ExposeData( Scriber& scriber ) {
	scriber.Scribe("Name", name);
	if (scriber.state == Load)
		RegisterName(name);
	if (scriber.state == Save)
		scriber.Scribe("Def", def);
	scriber.Scribe("Data", data);
	scriber.Scribe("Comps", comps);
}
std::string_view BaseThing::GetName() const {
	return name;
}
