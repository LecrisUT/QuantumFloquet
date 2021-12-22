//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_TYPEINFO_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_TYPEINFO_TPP

#include "TypeInfo.hpp"
#include "SharedRegistrar.tpp"
#include "ObjectRegistrar.tpp"

#if defined(__GNUG__)

#include <cxxabi.h>

#elif defined(__clang__)
#include <Demangle.h>
#endif

using namespace QuanFloq;

// region Class specializations
//template<>
//struct QuanFloq::TypeTraits<void> :
//		BaseTypeTraits {
//	std::string name;
//	BaseFactory* factory;
//	IRegistrar* registrar;
//	std::string_view Name() override;
//	BaseFactory* Factory() override;
//	IRegistrar* Registrar() override;
//	TypeTraits( std::type_index&& type, std::string_view name,
//	            BaseFactory* factory, IRegistrar* registrar );
//};
// endregion

// region Constructor/Destructor
template<class T>
TypeTraits<T>::TypeTraits():
		BaseTypeTraits{typeid(T)} {
	// The automatic name unmangling is compiler dependant and to make cross-compatible define it in a template here.
	// Specialize this or Name() to use custom human-readable names
	static const std::string defaultNamespace = "QuanFloq::";
#if defined(__GNUG__)
	int status;
	size_t length;
	char* result = abi::__cxa_demangle(type.name(), nullptr, &length, &status);
	name = std::string(result);
#elif defined(__clang__)
	auto mangledName = std::string(type.name());
	name = llvm::demangle(mangledName);
#else
	name = std::string(type.name());
#endif
	if (name.substr(0, defaultNamespace.size()) == defaultNamespace)
		name = name.substr(defaultNamespace.size());
}
template<class T>
TypeTraits<T>::TypeTraits( std::string_view name ):
		BaseTypeTraits{typeid(T)}, name{name} { }
// endregion

// region Factory functions
template<class T>
const TypeInfo& TypeInfo::Create() {
	return Create(TypeTraits<T>());
}
template<class T>
const TypeInfo& TypeInfo::Create( std::string_view name ) {
	return Create(TypeTraits<T>(name));
}
template<class T, class U>
const TypeInfo& TypeInfo::Create( std::string_view suffix ) {
	auto& type = Create(TypeTraits<T>());
	AddSuffix(type, typeid(U), suffix);
	return type;
}
// endregion

// region TypeRegistrationData
template<class T>
std::string_view TypeTraits<T>::Name() {
	return name;
}
template<class T>
BaseFactory* TypeTraits<T>::Factory() {
	if constexpr(stdFactory<T>)
		return &T::factory;
	else
		return nullptr;
}
template<class T>
IRegistrar* TypeTraits<T>::Registrar() {
	if constexpr(stdRegistrar<T>)
		return &T::registrar;
	else
		return nullptr;
}
// endregion

#endif //QUANFLOQ_INCLUDE_REGISTRAR_TYPEINFO_TPP
