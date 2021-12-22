//
// Created by lecris on 11/9/21.
//

#include "Registrar/TypeInfo.hpp"
#include "Registrar/ObjectRegistrar.tpp"
#include "interface/IExposable.hpp"
#include "Registrar/SharedRegistrar.tpp"

#include <dlfcn.h>
#include <cassert>
#include <algorithm>

using namespace QuanFloq;

// region Constructors/Destructors
dynamic_library::dynamic_library( std::string_view library ) :
		Location(library), DLL{OpenDL(Location)} { }
dynamic_library::~dynamic_library() {
	CloseDL(DLL);
}
TypeInfo::TypeInfo( BaseTypeTraits&& data ) :
		type{data.type}, name{data.Name()}, mangledName{data.MangledName()},
		iRegistrar{data.Registrar()}, factory{data.Factory()} { }
BaseTypeTraits::BaseTypeTraits( const std::type_index& type ) : type{type} { }
// endregion

// region Factory functions
std::shared_ptr<dynamic_library> dynamic_library::Create( std::string_view libray ) {
	auto ptr = std::shared_ptr<dynamic_library>(new dynamic_library(libray));
	registrar.Register(ptr);
	// At library loading static members are initialized and registered
	// There can be dangling registration because ResolvePostRegister cannot be called from constructor.q
	IRegistrar::ResolveDanglingRegisters();
	return ptr;
}
const TypeInfo& TypeInfo::Create( BaseTypeTraits&& data ) {
	auto result = registrar.Register(std::move(data));
	if (!result.second)
		throw NotRegistered(registrar, "Name not generated");
	return *result.first;
}
// endregion

// region Comparators
bool std::less<TypeInfo>::operator()( const TypeInfo& lhs, const TypeInfo& rhs ) const {
	return std::less<std::type_index>::operator()(lhs.type, rhs.type);
}
bool std::less<TypeInfo>::operator()( const TypeInfo& lhs, const std::type_index& rhs ) const {
	return std::less<std::type_index>::operator()(lhs.type, rhs);
}
bool std::less<TypeInfo>::operator()( const std::type_index& lhs, const TypeInfo& rhs ) const {
	return std::less<std::type_index>::operator()(lhs, rhs.type);
}
bool std::equal_to<TypeInfo>::operator()( const TypeInfo& lhs, const TypeInfo& rhs ) const {
	return std::equal_to<std::type_index>::operator()(lhs.type, rhs.type);
}
bool std::equal_to<TypeInfo>::operator()( const TypeInfo& lhs, const std::type_index& rhs ) const {
	return std::equal_to<std::type_index>::operator()(lhs.type, rhs);
}
bool std::equal_to<TypeInfo>::operator()( const std::type_index& lhs, const TypeInfo& rhs ) const {
	return std::equal_to<std::type_index>::operator()(lhs, rhs.type);
}
bool std::equal_to<TypeInfo>::operator()( const TypeInfo& lhs, std::string_view rhs ) const {
	return std::equal_to<std::string_view>::operator()(lhs.name, rhs);
}
bool std::equal_to<TypeInfo>::operator()( std::string_view lhs, const TypeInfo& rhs ) const {
	return std::equal_to<std::string_view>::operator()(lhs, rhs.name);
}
size_t std::hash<TypeInfo>::operator()( const TypeInfo& val ) const {
	return std::hash<std::type_index>::operator()(val.type);
}
bool dynamic_library::operator==( const dynamic_library& t2 ) const { return DLL == t2.DLL; }
bool TypeInfo::operator==( const TypeInfo& t2 ) const { return type == t2.type; }
bool TypeInfo::operator==( const std::type_index& t2 ) const { return type == t2; }
// endregion

// region Interface functions
void* dynamic_library::OpenDL( const std::filesystem::path& Location ) {
	void* dl = dlopen(Location.c_str(), RTLD_NOLOAD);
	assert(!dl);
	dl = dlopen(Location.c_str(), RTLD_NOW | RTLD_GLOBAL);
	return dl;
}
void dynamic_library::CloseDL( void* ptr ) {
	if (ptr == nullptr)
		dlclose(ptr);
}
std::string_view dynamic_library::GetName() const {
	return Location.c_str();
}
std::string_view TypeInfo::GetName() const {
	return name;
}
TypeInfo::task_type TypeInfo::AddSuffix( const TypeInfo& type, const std::type_index& base, std::string_view suffix ) {
	std::string suffixCopy{suffix};
	auto& baseType = co_await awaiter_type(registrar, base);
	type.name = baseType.name + suffixCopy;
	registrar.RegisterName(type.name, type);
}
std::string_view BaseTypeTraits::MangledName() const {
	return type.name();
}
// endregion
