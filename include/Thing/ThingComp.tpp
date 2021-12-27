//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_THINGCOMP_TPP
#define QUANFLOQ_THINGCOMP_TPP

#include "ThingComp.hpp"
#include "CompDef.hpp"
#include "Registrar/TypeInfo.tpp"
#include "Registrar/Factory.tpp"
#include "interface/Scriber/Scriber.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class T>
CompBase<T>::CompBase( thing_type& parent, std::shared_ptr<BaseCompDef> def ) :
		BaseComp(parent, def), parent{parent} { }
template<class C>
CompBase2<C>::CompBase2( thing_type& parent, std::shared_ptr<BaseCompDef> def ) :
		CompBase<thing_type>(parent, def) { }
template<class Traits>
ThingComp_<Traits>::ThingComp_( thing_type& parent, std::shared_ptr<def_type> def ):
		base(parent, def), def{*def} {
	static_assert(std::derived_from<comp_type, alias>,
	              "Traits::comp_type does not derive ThingComp<Traits>");
}
template<class Traits>
CompFactory_<Traits>::CompFactory_() {
	this->name.clear();
}
// endregion
// region Static initializations
template<class Traits>
inline const TypeInfo* const ThingComp_<Traits>::typeInfo = &TypeInfo::Create<comp_type>();
// endregion
// region Interfaces
template<class Traits>
void ThingComp_<Traits>::ExposeData( Scriber& scriber ) {
	BaseComp::ExposeData(scriber);
	if constexpr(stdSharedRegistrar<comp_type>)
		comp_type::registrar.ResolvePostRegister(this->GetName());
}
template<class Traits>
std::unique_ptr<typename Traits::comp_type> CompFactory_<Traits>::Make( IExposable* parent ) {
	assert(parent != nullptr);
	auto thingParent = dynamic_cast<thing_type*>(parent);
	assert(thingParent != nullptr);
	std::unique_ptr<comp_type> ptr;
	if constexpr(std::same_as<driver_type, void>) {
		static_assert(std::constructible_from<comp_type, thing_type&, std::shared_ptr<def_type>>,
		              "comp_type is not standard constructible\n"
		              "Can simply add using alias::alias to include the default constructor");
		ptr = std::make_unique<comp_type>(*thingParent, def);
	} else {
		static_assert(
				std::constructible_from<comp_type, thing_type&, std::shared_ptr<def_type>, std::shared_ptr<driver_type>>,
				"comp_type is not standard constructible\n"
				"A custom constructor needs to be defined including the interface's constructor");
		auto driver = std::dynamic_pointer_cast<driver_type>(def->driver);
		assert(driver);
		ptr = std::make_unique<comp_type>(*thingParent, def, driver);
	}
	if (def->scriberDriver != nullptr)
		ptr->format = def->scriberDriver;
	return ptr;
}
// endregion

#include "CompDriver.tpp"
#include "IComp.tpp"
#include "CompCache.tpp"
#include "CompDef.tpp"

#endif //QUANFLOQ_THINGCOMP_TPP
