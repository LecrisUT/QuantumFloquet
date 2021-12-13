//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRATION_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRATION_TPP

#include "TypeInfo.hpp"
#include "TypeRegistration.hpp"

#if defined(__GNUG__)

#include <cxxabi.h>

#elif defined(__clang__)
#include <Demangle.h>
#endif

using namespace QuanFloq;


// region Constructor/Destructor
template<class T>
TypeRegistrationData<T>::TypeRegistrationData():
		TypeRegistrationDataBase{typeid(T)} {
	// The automatic name unmangling is compiler dependant and to make cross-compatible define it in a template here.
	// Specialize this or Name() to use custom human-readable names
#if defined(__GNUG__)
	int status;
	size_t length;
	char* result = abi::__cxa_demangle(type.name(), nullptr, &length, &status);
	name = std::string(result, length);
#elif defined(__clang__)
	auto mangledName = std::string(type.name());
	name = llvm::demangle(mangledName);
#else
	name = std::string(type.name());
#endif
}
template<class T>
TypeRegistrationData<T>::TypeRegistrationData( std::string_view name ):
		TypeRegistrationDataBase{typeid(T)}, name{name} { }
// endregion

// region Factory functions
template<class T>
TypeRegistration TypeRegistration::Create() {
	return Create(TypeRegistrationData<T>());
}
template<class T>
TypeRegistration TypeRegistration::Create( std::string_view name ) {
	return Create(TypeRegistrationData<T>(name));
}
template<class T, stdTypeRegistered U>
TypeRegistration TypeRegistration::Create( std::string_view suffix ) {
	TypeRegistration& baseRegistration = U::typeRegistration;
	auto name = baseRegistration.type.name + std::string(suffix);
	return Create(TypeRegistrationData<T>(name));
}
//template<class T>
//TypeRegistration TypeRegistration::Create( std::string_view name, std::string_view factory, IRegistrar* registrar ) {
//	if (!registrar)
//		if constexpr(stdSharedRegistrar<T> && strComparable<T>)
//			registrar = &T::registrar;
//	return Create(TypeRegistrationData<void>(typeid(T), name, FactoryBase::registrar[factory], registrar));
//}
template<class T>
TypeRegistration TypeRegistration::Create( std::string_view name, FactoryBase* factory, IRegistrar* registrar ) {
	if (!registrar)
		if constexpr(stdSharedRegistrar<T> && strComparable<T>)
			registrar = &T::registrar;
	return Create(TypeRegistrationData<void>(typeid(T), name, factory, registrar));
}
//template<class T, stdTypeRegistered U>
//TypeRegistration
//TypeRegistration::Create( std::string_view suffix, std::string_view factory, IRegistrar* registrar ) {
//	TypeRegistration& baseRegistration = U::typeRegistration;
//	auto name = baseRegistration.type.name + std::string(suffix);
//	if (!registrar)
//		if constexpr(stdSharedRegistrar<T> && strComparable<T>)
//			registrar = &T::registrar;
//	return Create(TypeRegistrationData<void>(typeid(T), name, FactoryBase::registrar[Factory], registrar));
//}
template<class T, stdTypeRegistered U>
TypeRegistration TypeRegistration::Create( std::string_view suffix, FactoryBase* factory, IRegistrar* registrar ) {
	TypeRegistration& baseRegistration = U::typeRegistration;
	auto name = baseRegistration.type.name + std::string(suffix);
	if (!registrar)
		if constexpr(stdSharedRegistrar<T> && strComparable<T>)
			registrar = &T::registrar;
	return Create(TypeRegistrationData<void>(typeid(T), name, factory, registrar));
}
// endregion

// region TypeRegistrationData
template<class T>
std::string_view TypeRegistrationData<T>::Name() {
	return name;
}
template<class T>
FactoryBase* TypeRegistrationData<T>::Factory() {
	if constexpr(stdFactory<T>)
		return &T::factory;
	else
		return nullptr;
}
template<class T>
IRegistrar* TypeRegistrationData<T>::Registrar() {
	if constexpr(stdSharedRegistrar<T> && strComparable<T>)
		return &T::registrar;
	else
		return nullptr;
}
// endregion

#endif //QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRATION_TPP
