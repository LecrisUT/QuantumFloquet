//
// Created by lecris on 2021-12-06.
//

#include "libRegistration.hpp"
//#include <Registrar/Registrars.tpp>

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>

using namespace QuanFloq;


TEST_CASE("Test Registration", "[Registrar][Auto-Registration]") {
	STATIC_REQUIRE(stdSharedRegistrar<A1>);
	STATIC_REQUIRE(stdRefRegistrar<B1>);
	CHECK(A::registrar.Contains("A1"));
	CHECK(B::registrar.Contains("B1"));
	CHECK(A1::registration.pointer == A::registrar.GetPtr("A1"));
	CHECK(B1::registration.reference == B::registrar.GetRef("B1"));
}