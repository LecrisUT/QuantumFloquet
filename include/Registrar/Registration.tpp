//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_REGISTRATION_TPP
#define QUANFLOQ_REGISTRATION_TPP

#include "Registration.hpp"
#include "Registrars.hpp"
#include <cassert>

using namespace QuanFloq;

// region Constructor/Destructor
// endregion

// region RefRegistration
template<stdRegistrar T>
RefRegistration RefRegistration::Create( T& staticItem ) {
	bool res = T::registrar.Register(staticItem);
	assert(res);
	return RefRegistration(&staticItem);
}
template<class Base, std::derived_from<Base> T>
RefRegistration RefRegistration::Create( RefRegistrarRoot<Base>& registrar, T& staticItem ) {
	bool res = registrar.Register(staticItem);
	assert(res);
	return RefRegistration(&staticItem);
}
// endregion

// region PtrRegistration
template<stdSharedRegistrar T, class... Args>
requires ctrArgs<T, Args...>
PtrRegistration PtrRegistration::Create( Args&& ... args ) {
	auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
	bool res = T::registrar.Register(ptr);
	assert(res);
	return PtrRegistration(ptr);
}
template<class T, class Base, class... Args>
requires std::derived_from<T, Base> && ctrArgs<T, Args...>
PtrRegistration PtrRegistration::Create( SharedRegistrarRoot<Base>& registrar, Args&& ... args ) {
	auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
	bool res = registrar.Register(ptr);
	assert(res);
	return PtrRegistration(ptr);
}
// endregion



#endif //QUANFLOQ_REGISTRATION_TPP
