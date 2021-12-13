//
// Created by lecris on 2021-12-06.
//

#include "libRegistration.hpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>


TEST_CASE("Test Registration", "[Registrar][Auto-Registration]") {
	STATIC_REQUIRE(stdRegistered<A1>);
	STATIC_REQUIRE(stdRegistered<B1>);
	CHECK(A::registrar.Contains("A1"));
	CHECK(B::registrar.Contains("B1"));
	CHECK(A1::registration.pointer == A::registrar.VoidPtr("A1"));
	CHECK(B1::registration.reference == B::registrar.VoidRef("B1"));
}