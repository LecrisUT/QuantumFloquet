//
// Created by lecris on 2021-11-11.
//

#include "Registrar/TypeInfo.tpp"
#include "Thing/Thing.hpp"
#include "libThing.hpp"
#include "interface/Scriber/JSONDriver.hpp"
#include <filesystem>

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

using namespace QuanFloq;

TEST_CASE("Test Thing", "[Thing][Scriber][Registrar]") {
	SECTION("Intial loads") {
		std::shared_ptr<DLibrary> lib;
		REQUIRE_NOTHROW(lib = DLibrary::Create("./libtest_ThingMod.so"));
		CHECK(TypeInfo::registrar.Contains("A"));
//	CHECK(TypeInfo::registrar.Contains(typeid(A)));
		CHECK(TypeInfo::registrar.Contains("B"));
//	CHECK(TypeInfo::registrar.Contains(typeid(B)));
		CHECK(TypeInfo::registrar.Contains("C"));
//	CHECK(TypeInfo::registrar.Contains(typeid(C)));
		CHECK(TypeInfo::registrar.Contains("BData"));
//	CHECK(TypeInfo::registrar.Contains(typeid(BData)));
		CHECK(TypeInfo::registrar.Contains("ADef"));
		CHECK(TypeInfo::registrar.Contains("BDef"));
		CHECK(TypeInfo::registrar.Contains("CDef"));
//	CHECK(TypeInfo::registrar.Contains(typeid(CDef)));
		CHECK(TypeInfo::registrar.Contains("BCompA"));
//	CHECK(TypeInfo::registrar.Contains(typeid(BCompA)));
		CHECK(TypeInfo::registrar.Contains("BCompB"));
//	CHECK(TypeInfo::registrar.Contains(typeid(BCompB)));
		CHECK(TypeInfo::registrar.Contains("BCompADef"));
//	CHECK(TypeInfo::registrar.Contains(typeid(BCompB)));
		CHECK(TypeInfo::registrar.Contains("BCompBDef"));
//	CHECK(TypeInfo::registrar.Contains(typeid(BCompBDef)));
		CHECK(BaseDriver::registrar.Contains("BDriver1"));
		CHECK(BaseCompDriver::registrar.Contains("BCompADriver1"));
		CHECK(BaseFactory::registrar.Contains("ADefFactory"));
		CHECK(BaseFactory::registrar.Contains("BDefFactory"));
		CHECK(BaseFactory::registrar.Contains("CDefFactory"));
		CHECK(BaseFactory::registrar.Contains("BCompADefFactory"));
		CHECK(BaseFactory::registrar.Contains("BCompBDefFactory"));
		CHECK(BaseFactory::registrar.Contains("ThingDataFactory/BData"));
	} SECTION("Loading Defs") {
		std::vector<std::shared_ptr<BaseDef>> thingDefs;
		std::vector<std::shared_ptr<BaseCompDef>> compDefs;
		REQUIRE_NOTHROW(thingDefs = BaseDef::LoadDefs("./test_Defs.json"));
		REQUIRE_NOTHROW(compDefs = BaseCompDef::LoadDefs("./test_Defs.json"));
		CHECK(BaseDef::registrar.Contains("A"));
		CHECK(BaseDef::registrar.Contains("B1"));
		CHECK(BaseDef::registrar.Contains("B2"));
		CHECK(BaseDef::registrar.Contains("C"));
		CHECK(BaseFactory::registrar.Contains("ThingFactory/A"));
		CHECK(BaseFactory::registrar.Contains("ThingFactory/B1"));
		CHECK(BaseFactory::registrar.Contains("ThingFactory/B2"));
		CHECK(BaseFactory::registrar.Contains("ThingFactory/C"));
		CHECK(BaseCompDef::registrar.Contains("BCompA"));
		CHECK(BaseCompDef::registrar.Contains("BCompB"));
		CHECK(BaseFactory::registrar.Contains("CompFactory/BCompA"));
		CHECK(BaseFactory::registrar.Contains("CompFactory/BCompB"));
	} SECTION("Loading Things") {
		std::vector<std::unique_ptr<BaseThing>> things;
		auto jsonDriver = std::static_pointer_cast<JSONDriver>(JSONDriver::registration.pointer);
		auto scriber = jsonDriver->GenScriber(Load, "./test_Thing.json");
		REQUIRE_NOTHROW(scriber->Scribe("Things", things, true));
		CHECK(things[0]->GetName() == "ThingA_1");
		CHECK(things[0]->def->GetName() == "A");
		CHECK(things[1]->GetName() == "ThingB1_1");
		CHECK(things[1]->def->GetName() == "B1");
		CHECK(things[2]->GetName() == "ThingB1_2");
		CHECK(things[2]->def->GetName() == "B1");
		CHECK(things[3]->GetName() == "ThingB2_1");
		CHECK(things[3]->def->GetName() == "B2");
		CHECK(things[4]->GetName() == "ThingC_1");
		CHECK(things[4]->def->GetName() == "C");
	}
}