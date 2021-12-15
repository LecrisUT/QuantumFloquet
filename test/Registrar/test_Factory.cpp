//
// Created by lecris on 2021-12-06.
//


#include "libFactory.hpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>


TEST_CASE("Test Factory", "[Registrar][Scriber]") {
	STATIC_REQUIRE(stdFactory<A>);
}