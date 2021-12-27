//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_ICOMP_TPP
#define QUANFLOQ_ICOMP_TPP

#include "IComp.hpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class C>
ICompBase<C>::ICompBase():
		comp{static_cast<comp_type&>(*this)} {
	static_assert(std::derived_from<comp_type, ICompBase<C>>,
	              "Derived class does not implement this interface.");
}
template<class CDriver>
ICompBase2<CDriver>::ICompBase2( std::shared_ptr<driver_type> driver ):
		driver{std::move(driver)} { }
template<class C, class CDriver, class CCache>
ICompBase3<C, CDriver, CCache>::ICompBase3( std::shared_ptr<driver_type> driver ):
		ICompBase2<driver_type>(std::move(driver)) {
}
template<class Traits>
IComp_<Traits>::IComp_( std::shared_ptr<driver_type> driver ) : base(std::move(driver)) {
	static_assert(std::derived_from<interface_type, alias>,
	              "Traits::interface_type does not derive IComp<Traits>");
}
// endregion

#endif //QUANFLOQ_ICOMP_TPP
