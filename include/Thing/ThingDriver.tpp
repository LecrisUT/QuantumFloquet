//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_THINGDRIVER_TPP
#define QUANFLOQ_THINGDRIVER_TPP

#include "ThingDriver.hpp"
#include "Registrar/SharedRegistrar.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class Traits>
ThingDriver_<Traits>::ThingDriver_() {
	static_assert(std::derived_from<driver_type, alias>,
	              "Traits::driver_type does not derive ThingDriver<Traits>");
	static_assert(std::derived_from<driver_type, typename ThingTraits<thing_type>::driver_type>,
	              "Driver types mismatch");
}
// endregion

// region Static instantiations
template<class Traits>
inline typename Traits::registrar_type ThingDriver_<Traits>::registrar;
// endregion

#endif //QUANFLOQ_THINGDRIVER_TPP
