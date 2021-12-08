//
// Created by lecris on 2021-12-06.
//

#include "Registrar/Registrars.tpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <fmt/format.h>


using namespace QuanFloq;

namespace QuanFloq {
	struct IExposable {
		virtual ~IExposable() = default;
	};

	class A {
	protected:
		std::string Name;
	public:
		static SharedRegistrarRoot<A> registrar;
		static ObjectRegistrar<A> ObjRegistrar;
		A() = default;
		explicit A( std::string_view name ) :
				Name{name} { }
		std::string_view GetName() const { return Name; }
		void SetName( std::string_view name ) { Name = std::string(name); }
	};
	class B :
			public A {
	public:
		static SharedRegistrar<B, A> registrar;
		explicit B( std::string_view name ) :
				A{name} { }
	};
	class B2 :
			public A {
	public:
		static SharedRegistrar<B2, A> registrar;
		explicit B2( std::string_view name ) :
				A{name} { }
	};
	struct C {
		static RefRegistrarRoot<C> registrar;
		virtual std::string_view GetName() const = 0;
	};
	struct C1 : C {
		std::string_view GetName() const override {
			return "C1";
		}
	};
	struct C2 : C {
		std::string_view GetName() const override {
			return "C2";
		}
	};
	struct CA : C {
		static RefRegistrar<CA, C> registrar;
	};
	struct CB : C {
		static RefRegistrar<CB, C> registrar;
	};
	struct C3A : CA {
		std::string_view GetName() const override {
			return "C3A";
		}
	};
	struct C3B : CB {
		std::string_view GetName() const override {
			return "C3B";
		}
	};
	struct D {
		std::shared_ptr<A> ptr;
	};
	SharedRegistrarRoot<A> A::registrar = SharedRegistrarRoot<A>();
	ObjectRegistrar<A> A::ObjRegistrar = ObjectRegistrar<A>();
	SharedRegistrar<B, A> B::registrar = SharedRegistrar<B, A>();
	SharedRegistrar<B2, A> B2::registrar = SharedRegistrar<B2, A>();
	RefRegistrarRoot<C> C::registrar = RefRegistrarRoot<C>();
	RefRegistrar<CA, C> CA::registrar = RefRegistrar<CA, C>();
	RefRegistrar<CB, C> CB::registrar = RefRegistrar<CB, C>();
}

TEST_CASE("Test Registrars", "[Registrar]") {
	SECTION("Registration and Search") {
		SECTION("Reference Registrar") {
			STATIC_REQUIRE(stdRefRegistrar<C>);
			STATIC_REQUIRE(stdRefRegistrar<CA>);
			STATIC_REQUIRE(stdRegistrar<C>);
			STATIC_REQUIRE_FALSE(stdSharedRegistrar<C>);
			CHECK(C::registrar.Set.empty());
			C1 c1;
			C2 c2;
			C3A c3a;
			C3B c3b;
			REQUIRE_NOTHROW(C1::registrar.Register(c1));
			REQUIRE_NOTHROW(C2::registrar.Register(c2));
			REQUIRE_NOTHROW(C3A::registrar.Register(c3a));
			REQUIRE_NOTHROW(C3B::registrar.Register(c3b));
			CHECK(C::registrar.Contains("C1"));
			CHECK(C::registrar.Contains("C2"));
			CHECK(C::registrar.Contains("C3A"));
			CHECK(C::registrar.Contains("C3B"));
			CHECK(C3A::registrar.Contains("C3A"));
			CHECK(!C3A::registrar.Contains("C3B"));
		} SECTION("SharedPointer Registrar") {
			STATIC_REQUIRE(stdSharedRegistrar<A>);
			STATIC_REQUIRE(stdSharedRegistrar<B>);
			STATIC_REQUIRE(stdRegistrar<A>);
			STATIC_REQUIRE_FALSE(stdRefRegistrar<A>);
			CHECK(A::registrar.Set.empty());
			auto a = std::make_shared<A>("A");
			auto b = std::make_shared<B>("B");
			auto b2 = std::make_shared<B2>("B2");
			REQUIRE_NOTHROW(a->registrar.Register(a));
			REQUIRE_NOTHROW(b->registrar.Register(b));
			REQUIRE_NOTHROW(b2->registrar.Register(b2));
			CHECK(A::registrar.Contains("A"));
			CHECK(A::registrar.Contains("B"));
			CHECK(A::registrar.Contains("B2"));
			CHECK(B::registrar.Contains("B"));
			CHECK(!B::registrar.Contains("B2"));
		} SECTION("Object Registrar") {
			CHECK(A::ObjRegistrar.Set.empty());
			REQUIRE_NOTHROW(A::ObjRegistrar.Register("A1"));
			REQUIRE_NOTHROW(A::ObjRegistrar.Register("A2"));
			CHECK(A::ObjRegistrar.Contains("A1"));
		}
	} SECTION("Co_awaits") {
		auto a = std::make_shared<A>();
		REQUIRE_NOTHROW(A::registrar.Register(a));
		CHECK(A::registrar.Set.contains(a));
		D d;
		REQUIRE_NOTHROW(A::registrar.Get("AA", d.ptr, true));
		CHECK(d.ptr == nullptr);
		CHECK(!A::registrar.Contains("AA"));
		a->SetName("AA");
		CHECK(!A::registrar.Contains("AA"));
		CHECK(d.ptr == nullptr);
		REQUIRE_NOTHROW(A::registrar.ResolvePostRegister("AA"));
		CHECK(A::registrar.Contains("AA"));
		CHECK(d.ptr == a);
	}
}