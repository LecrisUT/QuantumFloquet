//
// Created by lecris on 2021-11-11.
//

#include "libScriber.hpp"
#include "interface/Scriber/ScribeDriver.hpp"
#include "interface/Scriber/Scriber.hpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>


TEST_CASE("Test Scriber", "[Interface][Scriber][Registrar]") {
	STATIC_REQUIRE(StaticScribeable<DBase>);
	STATIC_REQUIRE(Exposable<A>);
	STATIC_REQUIRE(Exposable<BBase>);
	STATIC_REQUIRE(Exposable<CBase>);
	STATIC_REQUIRE(stdTypeRegistered<A>);
	STATIC_REQUIRE(stdTypeRegistered<B>);
	STATIC_REQUIRE(stdTypeRegistered<C>);
	CHECK(TypeInfo::registrar.Contains("AType"));
	CHECK(TypeInfo::registrar.Contains("BType"));
	CHECK(TypeInfo::registrar.Contains("CType"));
	CHECK(D1::registrar.Contains("D1"));
	CHECK(ScribeDriver::registrar.Contains("JSON Driver"));
	auto& CType = TypeInfo::registrar[typeid(C)];
	auto CRegistrar = dynamic_cast<SharedRegistrarRoot<CBase>*>(CType.iRegistrar);
	REQUIRE(CRegistrar != nullptr);
	CHECK(CRegistrar->set.empty());
	std::unique_ptr<A> A1;
	CHECK_NOTHROW(A1 = A::factory.MakeUnique());
	REQUIRE(A1 != nullptr);
	std::shared_ptr<ScribeDriver> JSONDriver;
	REQUIRE_NOTHROW(JSONDriver = ScribeDriver::registrar["JSON Driver"]);
	CHECK(JSONDriver == "JSON Driver");
	CHECK(*JSONDriver == "JSON Driver");
	CHECK(A1->format == JSONDriver);
	std::shared_ptr<Scriber> Scriber;
	REQUIRE_NOTHROW(Scriber = A1->format->GenScriber(Load, "./test_Scriber.json"));
	CHECK(Scriber != nullptr);
	REQUIRE_NOTHROW(A1->ExposeData(*Scriber));
	CHECK(CRegistrar->Contains("C1"));
	std::shared_ptr<CBase> C1;
	REQUIRE_NOTHROW(C1 = CRegistrar->operator[]("C1"));
	CHECK(A1->name == "A1");
	CHECK(A1->CItem == "C1");
	CHECK(A1->CArray.size() == 1);
	CHECK(A1->CArray[0] == "C1");
	CHECK(A1->DItem == "D1");
	CHECK(A1->DArray.size() == 1);
	CHECK(A1->DArray[0] == "D1");
	CHECK(A1->BItem == "B1");
	auto B1 = dynamic_cast<B*>(A1->BItem.get());
	CHECK(B1 != nullptr);
	CHECK(B1->CItem == "C1");
	CHECK(A1->BArray.size() == 1);
	CHECK(A1->BArray[0] == "B2");
	auto B2 = dynamic_cast<B*>(A1->BArray[0].get());
	CHECK(B2 != nullptr);
	CHECK(B2->CItem == "");
	CHECK(A1->intVal == 10);
	CHECK(A1->intVec.size() == 1);
	CHECK(A1->intVec[0] == -20);
	CHECK(A1->sizetVal == 30);
	CHECK(A1->sizetVec.size() == 1);
	CHECK(A1->sizetVec[0] == 40);
	// TODO: Write an appropiate check for float upcasted value
//	CHECK(A1->floatVal == 3.40282347E+38);
	CHECK(A1->floatVec.size() == 1);
	CHECK(A1->floatVec[0] == 5.5);
	CHECK(A1->doubleVal == 1.79769313486231570E+308);
	CHECK(A1->doubleVec.size() == 1);
	CHECK(A1->doubleVec[0] == 6.6);
	CHECK(A1->stringVec.size() == 1);
	CHECK(A1->stringVec[0] == "String");
}