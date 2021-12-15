//
// Created by lecris on 2021-12-13.
//

#include "libScriber.hpp"
#include "interface/Scriber/Scriber.tpp"

using namespace QuanFloq;


std::string_view A::GetName() const {
	return name;
}
std::string_view B::GetName() const {
	return name;
}
std::string_view C::GetName() const {
	return name;
}
void A::ExposeData( Scriber& scriber ) {
	scriber.Scribe("Name", name);
	scriber.Scribe("CItem", CItem);
	scriber.Scribe("CArray", CArray);
	scriber.Scribe("DItem", DItem);
	scriber.Scribe("DArray", DArray);
	scriber.Scribe("BItem", BItem);
	scriber.Scribe("BArray", BArray);
	scriber.Scribe("intVal", intVal);
	scriber.Scribe("intVec", intVec);
	scriber.Scribe("sizetVal", sizetVal);
	scriber.Scribe("sizetVec", sizetVec);
	scriber.Scribe("floatVal", floatVal);
	scriber.Scribe("floatVec", floatVec);
	scriber.Scribe("doubleVal", doubleVal);
	scriber.Scribe("doubleVec", doubleVec);
	scriber.Scribe("stringVec", stringVec);
}
void B::ExposeData( Scriber& scriber ) {
	scriber.Scribe("Name", name);
	scriber.Scribe("CItem", CItem, false, true);
}
void C::ExposeData( Scriber& scriber ) {
	scriber.Scribe("Name", name);
	if (scriber.state == Load)
		registrar.ResolvePostRegister(name);
}
std::string_view D1::GetName() const {
	return "D1";
}
