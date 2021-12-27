//
// Created by lecris on 2021-12-16.
//

#include "libThing.hpp"
#include "Thing/Thing.tpp"

struct BDriver1 : BDriver {
	int func( B& b ) override;
	std::string_view GetName() const override;
	static PtrRegistration registration;
};
struct BCompADriver1 : BCompADriver {
	int func( BCompA& b ) override;
	std::string_view GetName() const override;
	static PtrRegistration registration;
};
inline PtrRegistration BDriver1::registration = PtrRegistration::Create<BDriver1>();
inline PtrRegistration BCompADriver1::registration = PtrRegistration::Create<BCompADriver1>();

B::B( std::shared_ptr<def_type> def, std::shared_ptr<IB::driver_type> driver ) :
		Thing_(def), IB(driver) { }
BCompA::BCompA( B& parent, std::shared_ptr<def_type> def, std::shared_ptr<BCompADriver> driver ) :
		ThingComp_(parent, def), IBCompA(driver) {
}
int IB::func() {
	return driver->func(thing);
}
int IBCompA::func() {
	return driver->func(comp);
}
std::string_view BDriver1::GetName() const {
	return "BDriver1";
}
std::string_view BCompADriver1::GetName() const {
	return "BCompADriver1";
}

void BData::ExposeData( Scriber& scriber ) {
	scriber.Scribe("Value", value);
}
void BCompA::ExposeData( Scriber& scriber ) {
	ThingComp_::ExposeData(scriber);
	scriber.Scribe("Value", value);
}

int BCompADriver1::func( BCompA& b ) {
	return b.value;
}
int BDriver1::func( B& b ) {
	return b.data->value;
}