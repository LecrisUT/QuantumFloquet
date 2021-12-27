//
// Created by lecris on 2021-12-16.
//

#ifndef QUANFLOQ_TEST_THING_LIBTHING_HPP
#define QUANFLOQ_TEST_THING_LIBTHING_HPP

#include "Thing/Thing.tpp"
#include "Registrar/Registration.hpp"
#include "libThingTraits.hpp"


namespace QuanFloq {
	struct A : Thing<A> {
		using Thing_::Thing_;
	};
	struct IB : IThing<IB> {
		int func();
		using IThing_::IThing_;
	};
	struct B : Thing<B>, IB {
		B( std::shared_ptr<def_type> def, std::shared_ptr<IB::driver_type> driver );
	};
	struct BData : ThingData<BData> {
		int value;
		void ExposeData( Scriber& scriber ) override;
		using ThingData_::ThingData_;
	};
	struct BDriver : ThingDriver<BDriver> {
		virtual int func( B& ) = 0;
	};
	struct IBCompA : IComp<IBCompA> {
		int func();
		using IComp_::IComp_;
	};
	struct BCompA : ThingComp<BCompA>, IBCompA {
		int value;
		BCompA( B& parent, std::shared_ptr<def_type> def, std::shared_ptr<BCompADriver> driver );
		void ExposeData( Scriber& scriber ) override;
	};
	struct BCompADriver : CompDriver<BCompADriver> {
		virtual int func( BCompA& ) = 0;
	};
	struct C : Thing<C> {
		using Thing_::Thing_;
	};
	struct CDef : ThingDef<CDef> {
	};
	struct BCompB : ThingComp<BCompB> {
		using ThingComp_::ThingComp_;
	};
	struct BCompBDef : CompDef<BCompBDef> {
	};
}

#endif //QUANFLOQ_TEST_THING_LIBTHING_HPP
