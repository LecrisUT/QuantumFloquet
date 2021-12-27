//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_ITHING_TPP
#define QUANFLOQ_ITHING_TPP

#include "IThing.hpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class T>
IThingBase<T>::IThingBase():
		thing{static_cast<thing_type&>(*this)} {
	static_assert(std::derived_from<thing_type, IThingBase<T>>,
	              "Derived class does not implement this interface.");
}
template<class TDriver>
IThingBase2<TDriver>::IThingBase2( std::shared_ptr<driver_type> driver ):
		driver{std::move(driver)} { }
template<class T, class TDriver, class TCache>
IThingBase3<T, TDriver, TCache>::IThingBase3( std::shared_ptr<driver_type> driver ):
		IThingBase2<driver_type>(std::move(driver)) {
}

template<class Traits>
IThing_<Traits>::IThing_( std::shared_ptr<driver_type> driver ) : base(std::move(driver)) {
	static_assert(std::derived_from<interface_type, alias>,
	              "Traits::interface_type does not derive IThing<Traits>");
}
// endregion

#endif //QUANFLOQ_ITHING_TPP
