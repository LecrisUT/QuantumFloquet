//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_COMPDEF_TPP
#define QUANFLOQ_COMPDEF_TPP

#include "CompDef.hpp"
#include "Registrar/SharedRegistrar.tpp"
#include "Registrar/TypeInfo.tpp"
#include "Registrar/Factory.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class T>
CompDefBase<T>::CompDefBase( BaseFactory& factory ):
		BaseCompDef(factory) { }
template<class C>
CompDefBase2<C>::CompDefBase2( compFactory_type& factory ) :
		CompDefBase<thing_type>(factory), compFactory{factory} { }
template<class Traits>
CompDef_<Traits>::CompDef_():
		base(compFactory), compFactory() {
	static_assert(std::derived_from<def_type, alias>,
	              "Traits::def_type does not derive CompDef<Traits>");
	static_assert(std::constructible_from<compFactory_type>,
	              "CompFactory constructor is not standard");
}
// endregion

// region Static initializations
template<class Traits>
inline const TypeInfo* const CompDef_<Traits>::typeInfo = &TypeInfo::Create<def_type, comp_type>("Def");
template<class Traits>
inline typename Traits::registrar_type CompDef_<Traits>::registrar;
template<class Traits>
inline typename Traits::factory_type CompDef_<Traits>::factory;
// endregion

// region Interfaces
template<class Traits>
void CompDef_<Traits>::ExposeData( Scriber& base ) {
	BaseCompDef::ExposeData(base);
	if (base.state == Load) {
		if (!this->GetName().empty()) {
			registrar.RegisterName(this->GetName(), this);
			registrar.ResolvePostRegister(this->GetName());
			if (compFactory.GetName().empty()) {
				compFactory.name = "CompFactory/" + std::string(this->GetName());
				if (!compFactory.GetName().empty())
					BaseFactory::registrar.RegisterName(compFactory.GetName(), compFactory);
			}
		}
	}
}
template<class Traits>
std::unique_ptr<typename Traits::def_type> CompDefFactory_<Traits>::Make( IExposable* parent ) {
	assert(false);
}
template<class Traits>
std::shared_ptr<typename Traits::def_type> CompDefFactory_<Traits>::MakeShared( IExposable* parent ) {
	if constexpr (std::constructible_from<def_type>) {
		auto object = std::make_shared<def_type>();
		object->compFactory.def = object;
		if (!object->GetName().empty() && object->compFactory.GetName().empty()) {
			object->compFactory.name = "CompFactory/" + std::string(object->GetName());
			if (!object->compFactory.GetName().empty())
				BaseFactory::registrar.RegisterName(object->compFactory.GetName(), object->compFactory);
		}
		return object;
	} else
		throw std::runtime_error("Cannot Make");
}
// endregion

#endif //QUANFLOQ_COMPDEF_TPP
