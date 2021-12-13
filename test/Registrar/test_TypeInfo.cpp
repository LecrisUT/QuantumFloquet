//
// Created by lecris on 2021-12-06.
//


#include "libTypeInfo.hpp"
#include "Registrar/TypeInfo.hpp"
#include "Registrar/Registrars.tpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>

#if defined(__GNUG__)

#include <cxxabi.h>

#elif defined(__clang__)
#include <Demangle.h>
#endif


TEST_CASE("Test TypeInfo", "[Registrar][Type][Auto-Registration]") {
	STATIC_REQUIRE(stdNamed<TypeInfo>);
	STATIC_REQUIRE(strComparable<TypeInfo>);
	STATIC_REQUIRE(stdRegistrar<A>);
	STATIC_REQUIRE(stdTypeRegistered<A>);
	STATIC_REQUIRE(stdFactory<A>);
	CHECK(!TypeInfo::registrar.Set.empty());
	CHECK(A::typeRegistration.type == typeid(A));
	std::string name;
#if defined(__GNUG__)
	int status;
	size_t length;
	char* result = abi::__cxa_demangle(typeid(A).name(), nullptr, &length, &status);
	name = std::string(result, length);
#elif defined(__clang__)
	auto mangledName = std::string(typeid(A).name());
	name = llvm::demangle(mangledName);
#else
	name = std::string(typeid(A).name());
#endif
	INFO(fmt::format("Unmangled name = {}", name))
	CHECK(TypeInfo::registrar.Contains(name));
	REQUIRE(TypeInfo::registrar.Contains(typeid(A)));
	// Convoluted type in order to have a constructable reference in a function
	std::unique_ptr<std::reference_wrapper<const TypeInfo>> typeptr;
	REQUIRE_NOTHROW(typeptr = std::make_unique<std::reference_wrapper<const TypeInfo>>(
			std::ref<const TypeInfo>(TypeInfo::registrar[typeid(A)])));
	const TypeInfo& type = typeptr->get();
	CHECK(type.type == typeid(A));
	CHECK(type.name == name);
	CHECK(type.mangledName == typeid(A).name());
	CHECK(type.factory == &A::factory);
	CHECK(type.iRegistrar == &A::registrar);
}