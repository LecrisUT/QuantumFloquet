//
// Created by lecris on 2021-12-06.
//


#include "test_TypeInfo.hpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>

#if defined(__GNUG__)

#include <cxxabi.h>

#elif defined(__clang__)
#include <Demangle.h>
#endif

TEST_CASE("Test TypeInfo", "[Registrar][Type][Auto-Registration]") {
	SECTION("Basic TypeInfo interface") {
		STATIC_REQUIRE(stdNamed<TypeInfo>);
		STATIC_REQUIRE(strComparable<TypeInfo>);
		STATIC_REQUIRE(stdRegistrar<A>);
		STATIC_REQUIRE(stdTypeRegistered<A>);
		STATIC_REQUIRE(stdFactory<A>);
		CHECK(!TypeInfo::registrar.set.empty());
		CHECK(A::typeInfo->type == typeid(A));
		CHECK(TypeInfo::registrar.Contains("A"));
		REQUIRE(TypeInfo::registrar.Contains(typeid(A)));
		// Convoluted type in order to have a constructable reference in a function
		const TypeInfo* typeptr;
		REQUIRE_NOTHROW(typeptr = &TypeInfo::registrar[typeid(A)]);
		CHECK(typeptr->type == typeid(A));
		CHECK(typeptr->name == "A");
		CHECK(typeptr->mangledName == typeid(A).name());
		CHECK(typeptr->factory == &A::factory);
		CHECK(typeptr->iRegistrar == &A::registrar);
		CHECK(A1::typeInfo.type == typeid(A1));
		CHECK(A2::typeInfo.type == typeid(A2));
		CHECK(A1::typeInfo.name == "ASuffix1");
		CHECK(A2::typeInfo.name == "ASuffix2");
		CHECK(B::typeInfo.name == "BNamed");
		CHECK(C<int>::typeInfo.name == "C<int>");
		CHECK(C<A>::typeInfo.name == "C<QuanFloq::A>");
	} SECTION("Dynamic library and automatic registration") {
		CHECK(!TypeInfo::registrar.Contains("D1"));
		CHECK(!TypeInfo::registrar.Contains("D2"));
		CHECK(!TypeInfo::registrar.Contains("D3"));
		CHECK(!TypeInfo::registrar.Contains("E1"));
		CHECK(!TypeInfo::registrar.Contains("E2"));
		CHECK(!TypeInfo::registrar.Contains("E3"));
		CHECK(!TypeInfo::registrar.Contains("F"));
		REQUIRE_NOTHROW(DLibrary::Create("./libtest_TypeInfoMod.so"));
		CHECK(DLibrary::registrar.Contains("./libtest_TypeInfoMod.so"));
		CHECK(TypeInfo::registrar.Contains("D1"));
		CHECK(TypeInfo::registrar.Contains("D2"));
		CHECK(TypeInfo::registrar.Contains("D3"));
		CHECK_NOFAIL(!TypeInfo::registrar.Contains("E1"));
		CHECK_NOFAIL(!TypeInfo::registrar.Contains("E2"));
		CHECK_NOFAIL(TypeInfo::registrar.Contains("E3"));
		CHECK(TypeInfo::registrar.Contains("F"));
	}
}