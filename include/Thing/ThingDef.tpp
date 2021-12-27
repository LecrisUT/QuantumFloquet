//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_THINGDEF_TPP
#define QUANFLOQ_THINGDEF_TPP

#include "ThingDef.hpp"
#include "Registrar/SharedRegistrar.tpp"
#include "Registrar/TypeInfo.tpp"
#include "Registrar/Factory.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class T>
ThingDefBase<T>::ThingDefBase( thingFactory_type& factory ) :
		BaseDef(factory), thingFactory{factory} { }
template<class Traits>
ThingDef_<Traits>::ThingDef_():
		base(thingFactory), thingFactory() {
	static_assert(std::derived_from<def_type, alias>,
	              "Traits::def_type does not derive ThingDef<Traits>");
	static_assert(std::constructible_from<thingFactory_type>,
	              "ThingFactory constructor is not standard");
}
// endregion

// region Static initializers
template<class Traits>
inline const TypeInfo* const ThingDef_<Traits>::typeInfo = &TypeInfo::Create<def_type, thing_type>("Def");
template<class Traits>
inline typename Traits::registrar_type ThingDef_<Traits>::registrar;
template<class Traits>
inline typename Traits::factory_type ThingDef_<Traits>::factory;
// endregion

// region Interfaces
template<class Traits>
void ThingDef_<Traits>::ExposeData( Scriber& base ) {
	BaseDef::ExposeData(base);
	if (base.state == Load) {
		if (!this->GetName().empty()) {
			registrar.RegisterName(this->GetName(), this);
			registrar.ResolvePostRegister(this->GetName());
			if (thingFactory.GetName().empty()) {
				thingFactory.name = "ThingFactory/" + std::string(this->GetName());
				if (!thingFactory.GetName().empty())
					BaseFactory::registrar.RegisterName(thingFactory.GetName(), thingFactory);
			}
		}
	}
}
template<class Traits>
std::unique_ptr<typename Traits::def_type> ThingDefFactory_<Traits>::Make( IExposable* parent ) {
	assert(false);
}
template<class Traits>
std::shared_ptr<typename Traits::def_type> ThingDefFactory_<Traits>::MakeShared( IExposable* parent ) {
	if constexpr (std::constructible_from<def_type>) {
		auto object = std::make_shared<def_type>();
		object->thingFactory.def = object;
		if (!object->GetName().empty() && object->thingFactory.GetName().empty()) {
			object->thingFactory.name = "ThingFactory/" + std::string(object->GetName());
			if (!object->thingFactory.GetName().empty())
				BaseFactory::registrar.RegisterName(object->thingFactory.GetName(), object->thingFactory);
		}
		return object;
	} else
		throw std::runtime_error("Cannot Make");
}
// endregion

#endif //QUANFLOQ_THINGDEF_TPP
