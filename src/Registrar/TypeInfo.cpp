//
// Created by lecris on 11/9/21.
//

#include "Registrar/TypeInfo.hpp"
#include "Registrar/TypeRegistrar.hpp"
#include "Registrar/TypeRegistration.hpp"

#include <dlfcn.h>
#include <cassert>

using namespace QuanFloq;

// region Constructors/Destructors
dynamic_library::dynamic_library( std::string_view library ) :
		DLL(dlopen(std::string(library).c_str(), RTLD_NOW), CloseDL), Location(library) {
	// TODO: Check if Register in constructor works.
	registrar.Register(shared_from_this());
	// At library loading static members are initialized and registered
	// There can be dangling registration because ResolvePostRegister cannot be called from constructor.q
	IRegistrar::ResolveDanglingRegisters();
}
TypeInfo::TypeInfo( TypeRegistrationDataBase&& data ) :
		type{data.type}, name{data.Name()}, mangledName{data.MangledName()},
		iRegistrar{data.Registrar()}, factory{data.Factory()} { }
// endregion

// region Factory functions
std::shared_ptr<dynamic_library> dynamic_library::Create( std::string_view libray ) {
	return std::shared_ptr<dynamic_library>(new dynamic_library(libray));
}
TypeInfo TypeInfo::Create( TypeRegistrationDataBase&& data ) {
	return TypeInfo(std::forward<TypeRegistrationDataBase>(data));
}
// endregion

// region Comparators
bool dynamic_library::operator==( const dynamic_library& t2 ) const { return DLL == t2.DLL; }
bool TypeInfo::operator==( const TypeInfo& t2 ) const { return type == t2.type; }
bool TypeInfo::operator==( const std::type_index& t2 ) const { return type == t2; }
// endregion

// region Interface functions
void dynamic_library::CloseDL( void* ptr ) {
	if (ptr == nullptr)
		dlclose(ptr);
}
std::string_view dynamic_library::GetName() const {
	return Location;
}
std::string_view TypeInfo::GetName() const {
	return name;
}
// endregion
