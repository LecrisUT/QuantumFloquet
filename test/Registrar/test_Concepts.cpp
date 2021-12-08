//
// Created by lecris on 2021-12-06.
//

//
// Created by lecris on 2021-12-06.
//

#include "Registrar/RegistrarConcepts.hpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>


using namespace QuanFloq;

namespace QuanFloq {
	struct A {
		std::string Name;
		A( std::string_view name ) : Name(name) { }
		auto operator<=>( std::string_view str ) const {
			return Name <=> str;
		}
		auto operator==( std::string_view str ) const {
			return Name == str;
		}
		auto operator<=>( const A& a2 ) const {
			return Name <=> a2.Name;
		}
		auto operator==( const A& a2 ) const {
			return Name == a2.Name;
		}
	};
	struct B {
		std::string Name;
		B( std::string_view name ) : Name(name) { }
		std::string_view GetName() const { return Name; }
	};
	struct C {
		std::string Name;
		C( std::string_view name ) : Name(name) { }
	};
	template<class T>
	struct SPtr : std::shared_ptr<T> {
	};
	template<class T>
	struct UPtr : std::shared_ptr<T> {
	};
	template<class T>
	struct SPtrVec : std::vector<std::shared_ptr<T>> {
	};
	template<class T>
	struct UPtrVec : std::vector<std::shared_ptr<T>> {
	};
}

TEST_CASE("Test Registrar Concepts", "[Concepts][Registrar]") {
	SECTION("Smart Pointer") {
		STATIC_REQUIRE(sptr<std::shared_ptr<A>, A>);
		STATIC_REQUIRE_FALSE(sptr<std::shared_ptr<A>, B>);
		STATIC_REQUIRE(sptr<std::unique_ptr<A>, A>);
		STATIC_REQUIRE_FALSE(sptr<std::unique_ptr<A>, B>);
		STATIC_REQUIRE(sptrVec<std::vector<std::shared_ptr<A>>, A>);
		STATIC_REQUIRE_FALSE(sptrVec<std::vector<std::shared_ptr<A>>, B>);
		STATIC_REQUIRE(sptrVec<std::vector<std::unique_ptr<A>>, A>);
		STATIC_REQUIRE_FALSE(sptrVec<std::vector<std::unique_ptr<A>>, B>);

		STATIC_REQUIRE(sptr<SPtr < A>, A >);
		STATIC_REQUIRE_FALSE(sptr<SPtr < A>, B >);
		STATIC_REQUIRE(sptr<UPtr < A>, A >);
		STATIC_REQUIRE_FALSE(sptr<UPtr < A>, B >);
		STATIC_REQUIRE(sptrVec<SPtrVec < A>, A >);
		STATIC_REQUIRE_FALSE(sptr<SPtrVec < A>, B >);
		STATIC_REQUIRE(sptrVec<UPtrVec < A>, A >);
		STATIC_REQUIRE_FALSE(sptrVec<UPtrVec < A>, B >);
		STATIC_REQUIRE_FALSE(sptr<A*, A>);
		STATIC_REQUIRE_FALSE(sptrVec<std::vector<A*>, A>);
	} SECTION("String Comparator") {
		STATIC_REQUIRE_FALSE(stdNamed<A>);
		STATIC_REQUIRE(stdNamed<B>);
		STATIC_REQUIRE(strComparable<A>);
		STATIC_REQUIRE(strComparable<B>);
		STATIC_REQUIRE_FALSE(strComparable<C>);
		A a{"Name1"}, a2{"Name2"};
		B b{"Name3"}, b2{"Name4"};
		A& ra = a, ra2 = a2;
		std::reference_wrapper<A> rwa = a, rwa2 = a2;
		std::reference_wrapper<B> rwb = b, rwb2 = b2;
		std::shared_ptr<A> sa = std::make_shared<A>("Name5"), sa2;
		std::unique_ptr<A> ua = std::make_unique<A>("Name6"), ua2;
		CHECK(a > "Name0");
		CHECK(a != "Name0");
		CHECK(a == "Name1");
		CHECK(a < "Name2");
		CHECK(a < a2);
		CHECK(b > "Name0");
		CHECK(b != "Name0");
		CHECK(b == "Name3");
		CHECK(b < "Name4");
		CHECK(b < b2);
		CHECK(ra > "Name0");
		CHECK(ra == "Name1");
		CHECK(ra < ra2);
		CHECK(ra < a2);
		CHECK(rwa > "Name0");
		CHECK(rwa == "Name1");
		CHECK(rwa < rwa2);
		CHECK(rwa < ra2);
		CHECK(rwa < a2);
		CHECK(rwb < rwb2);
		CHECK(sa > "Name0");
		CHECK(sa == "Name5");
		CHECK(sa2 < "Name0");
		CHECK(sa2 == "");
		CHECK(ua > "Name0");
		CHECK(ua == "Name6");
		CHECK(ua2 < "Name0");
		CHECK(ua2 == "");
	}
}