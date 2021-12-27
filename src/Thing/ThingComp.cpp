//
// Created by Le Minh Cristian on 2021/11/04.
//

#include "Thing/ThingComp.hpp"
#include "Thing/CompDef.hpp"
#include "interface/Scriber/Scriber.tpp"
#include "Registrar/Factory.tpp"
#include "Thing/Thing.hpp"

using namespace QuanFloq;

BaseComp::BaseComp( BaseThing& parent_, std::shared_ptr<BaseCompDef> def_ ) :
		def{std::move(def_)}, parent{parent_} {
	if (!parent.GetName().empty())
		name = std::string(parent.GetName()) + "/" + std::string(def->GetName());
}

void BaseComp::ExposeData( Scriber& scriber ) {
	scriber.Scribe("Name", name);
	if (scriber.state == Load) {
		if (name.empty()) {
			if (!parent.GetName().empty()) {
				name = std::string(parent.GetName()) + "/" + std::string(def->GetName());
				RegisterName(name);
			}
		} else
			RegisterName(name);
	}
	if (scriber.state == Save)
		scriber.Scribe("Def", def);
}
std::string_view BaseComp::GetName() const {
	return name;
}
