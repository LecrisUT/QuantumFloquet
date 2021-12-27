//
// Created by lecris on 2021-12-20.
//

#ifndef QUANFLOQ_TEST_THING_LIBTHINGTRAITS_HPP
#define QUANFLOQ_TEST_THING_LIBTHINGTRAITS_HPP

#include "Thing/ThingTraits.hpp"

namespace QuanFloq {
	struct B;
	struct BData;
	class BDriver;
	class IB;
	template<>
	struct ThingDataTraits<BData> : DefaultThingDataTraits<BData, B> {
	};
	template<>
	struct ThingDriverTraits<BDriver> : DefaultThingDriverTraits<BDriver, B, IB> {
	};
	template<>
	struct IThingTraits<IB> : DefaultIThingTraits<IB, B, BDriver> {
	};
	template<>
	struct ThingTraits<B> : DefaultThingTraits<B, BDriver, BData> {
	};
	struct BCompA;
	struct BCompADriver;
	struct IBCompA;
	template<>
	struct CompTraits<BCompA>;
	template<>
	struct CompDriverTraits<BCompADriver> : DefaultCompDriverTraits<BCompADriver, BCompA, IBCompA> {

	};
	template<>
	struct ICompTraits<IBCompA> : DefaultICompTraits<IBCompA, BCompA, BCompADriver> {

	};
	template<>
	struct CompTraits<BCompA> : DefaultCompTraits<BCompA, B, BCompADriver> {

	};
	struct C;
	struct CDef;
	template<>
	struct ThingTraits<C> : DefaultThingTraits<C> {
		using def_type = CDef;
	};
	template<>
	struct ThingDefTraits<CDef> : DefaultThingDefTraits<CDef, C> {
	};
	struct BCompB;
	struct BCompBDef;
	template<>
	struct CompTraits<BCompB> : DefaultCompTraits<BCompB, B> {
		using def_type = BCompBDef;
	};
	template<>
	struct CompDefTraits<BCompBDef> : DefaultCompDefTraits<BCompBDef, BCompB> {
	};
}

#endif //QUANFLOQ_TEST_THING_LIBTHINGTRAITS_HPP
