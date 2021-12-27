//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_COMPDRIVER_TPP
#define QUANFLOQ_COMPDRIVER_TPP

#include "CompDriver.hpp"
#include "Registrar/SharedRegistrar.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class Traits>
CompDriver_<Traits>::CompDriver_() {
	static_assert(std::derived_from<driver_type, alias>,
	              "Traits::driver_type does not derive CompDriver<Traits>");
}
// endregion

// region Static initializations
template<class Traits>
inline typename Traits::registrar_type CompDriver_<Traits>::registrar;
// endregion


#endif //QUANFLOQ_COMPDRIVER_TPP
