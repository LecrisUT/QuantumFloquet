//
// Created by lecris on 11/9/21.
//

#ifndef QUANFLOQ_THINGCONCEPTS_HPP
#define QUANFLOQ_THINGCONCEPTS_HPP

#include <concepts>
#include <memory>
#include "ThingTraits.hpp"

namespace QuanFloq {
	// region Traits forward declarations
	template<class>
	struct ThingTraits;
	template<class>
	struct ThingDriverTraits;
	template<class>
	struct IThingTraits;
	template<class>
	struct ThingDefTraits;
	template<class>
	struct ThingDataTraits;
	template<class>
	struct CompTraits;
	template<class>
	struct CompDriverTraits;
	template<class>
	struct ICompTraits;
	template<class>
	struct CompDefTraits;
	// endregion

	// region Traits concepts
	template<class This>
	concept tThingTraits = requires {
		typename This::thing_type;
		typename This::driver_type;
		typename This::data_type;
		typename This::def_type;
		typename This::factory_type;
		typename This::comp_type;
		typename This::thingTraits;
		typename ThingTraits<typename This::thing_type>;
	} && std::same_as<This, ThingTraits<typename This::thing_type>>;
//	template<class This>
//	concept tThingTraits = true;
	template<class This>
	concept tDriverTraits = requires {
		typename This::driver_type;
		typename This::thing_type;
		typename This::cache_type;
		typename This::interface_type;
		typename This::registrar_type;
		typename This::driverTraits;
		typename ThingDriverTraits<typename This::driver_type>;
	} && std::same_as<This, ThingDriverTraits<typename This::driver_type>>;
	template<class This>
	concept tIThingTraits = requires {
		typename This::driver_type;
		typename This::thing_type;
		typename This::cache_type;
		typename This::interface_type;
		typename This::iThingTraits;
		typename IThingTraits<typename This::interface_type>;
	} && std::same_as<This, IThingTraits<typename This::interface_type>>;
	template<class This>
	concept tDefTraits = requires {
		typename This::def_type;
		typename This::thing_type;
		typename This::registrar_type;
		typename This::factory_type;
		typename This::defTraits;
		typename ThingDefTraits<typename This::def_type>;
	} && std::same_as<This, typename ThingDefTraits<typename This::def_type>::defTraits>;
	template<class This>
	concept tDataTraits = requires {
		typename This::data_type;
		typename This::thing_type;
		typename This::factory_type;
		typename This::dataTraits;
		typename ThingDataTraits<typename This::data_type>;
	} && std::same_as<This, ThingDataTraits<typename This::data_type>>;
	template<class This>
	concept tCompTraits = requires {
		typename This::comp_type;
		typename This::thing_type;
		typename This::driver_type;
		typename This::def_type;
		typename This::factory_type;
		typename This::compTraits;
		typename CompTraits<typename This::comp_type>;
	} && std::same_as<This, CompTraits<typename This::comp_type>>;
	template<class This>
	concept tCompDriverTraits = requires {
		typename This::driver_type;
		typename This::comp_type;
		typename This::cache_type;
		typename This::interface_type;
		typename This::registrar_type;
		typename This::compDriverTraits;
		typename CompDriverTraits<typename This::driver_type>;
	} && std::same_as<This, CompDriverTraits<typename This::driver_type>>;
	template<class This>
	concept tICompTraits = requires {
		typename This::driver_type;
		typename This::comp_type;
		typename This::cache_type;
		typename This::interface_type;
		typename This::iCompTraits;
		typename ICompTraits<typename This::interface_type>;
	} && std::same_as<This, ICompTraits<typename This::interface_type>>;
	template<class This>
	concept tCompDefTraits = requires {
		typename This::def_type;
		typename This::comp_type;
		typename This::thing_type;
		typename This::registrar_type;
		typename This::factory_type;
		typename This::compDefTraits;
		typename CompDefTraits<typename This::def_type>;
	} && std::same_as<This, typename CompDefTraits<typename This::def_type>::compDefTraits>;
	// endregion
	// region Internal Things
	template<class Traits>
	class Thing_;
	template<class Traits>
	class ThingCache_;
	template<class Traits>
	struct ThingFactory_;
	template<class Traits>
	class ThingDriver_;
	template<class Traits>
	class IThing_;
	template<class Traits>
	class ThingDef_;
	template<class Traits>
	struct ThingDefFactory_;
	template<class Traits>
	class ThingData_;
	template<class Traits>
	struct ThingDataFactory_;
	template<class Traits>
	class ThingComp_;
	template<class Traits>
	class CompCache_;
	template<class Traits>
	struct CompFactory_;
	template<class Traits>
	class CompDriver_;
	template<class Traits>
	class IComp_;
	template<class Traits>
	class CompDef_;
	template<class Traits>
	struct CompDefFactory_;
	// endregion
	// region Aliases and Other mains
	// Thing
	template<class T>
	using Thing = Thing_<typename ThingTraits<T>::thingTraits>;
	template<class T>
	using ThingCache = ThingCache_<typename ThingTraits<T>::thingTraits>;
	template<class T>
	using ThingFactory = ThingFactory_<typename ThingTraits<T>::thingTraits>;
	// ThingDriver
	template<class TDriver>
	using ThingDriver = ThingDriver_<typename ThingDriverTraits<TDriver>::driverTraits>;
	// IThing
	template<class IT>
	using IThing = IThing_<typename IThingTraits<IT>::iThingTraits>;
	// ThingDef
	template<class TDef>
	using ThingDef = ThingDef_<typename ThingDefTraits<TDef>::defTraits>;
	template<class TDef>
	using ThingDefFactory = ThingDefFactory_<typename ThingDefTraits<TDef>::defTraits>;
	// ThingData
	template<class TData>
	using ThingData = ThingData_<typename ThingDataTraits<TData>::dataTraits>;
	template<class TData>
	using ThingDataFactory = ThingDataFactory_<typename ThingDataTraits<TData>::dataTraits>;
	// ThingComp
	template<class C>
	using ThingComp = ThingComp_<typename CompTraits<C>::compTraits>;
	template<class C>
	using CompCache = CompCache_<typename CompTraits<C>::compTraits>;
	template<class C>
	using CompFactory = CompFactory_<typename CompTraits<C>::compTraits>;
	// CompDriver
	template<class CDriver>
	using CompDriver = CompDriver_<typename CompDriverTraits<CDriver>::compDriverTraits>;
	// IComp
	template<class IC>
	using IComp = IComp_<typename ICompTraits<IC>::iCompTraits>;
	// CompDef
	template<class CDef>
	using CompDef = CompDef_<typename CompDefTraits<CDef>::compDefTraits>;
	template<class CDef>
	using CompDefFactory = CompDefFactory_<typename CompDefTraits<CDef>::compDefTraits>;
	// endregion
	// region Thing Concepts
	// Thing
	template<class This>
	concept tThing = std::derived_from<This, Thing<This>>;
	template<class This, class T>
	concept tCache = std::derived_from<This, ThingCache<T>>;
	// ThingDriver
	template<class This>
	concept tDriver = std::derived_from<This, ThingDriver<This>>;
	// IThing
	template<class This>
	concept tIThing = std::derived_from<This, IThing<This>>;
	// ThingDef
	template<class This>
	concept tDef = std::derived_from<This, ThingDef<This>>;
	// ThingData
	template<class This>
	concept tThingData = std::derived_from<This, ThingData<This>>;
	// ThingComp
	template<class This>
	concept tComp = std::derived_from<This, ThingComp<This>>;
	template<class This, class C>
	concept tCompCache = std::derived_from<This, CompCache<C>>;
	// CompDriver
	template<class This>
	concept tCompDriver = std::derived_from<This, CompDriver<This>>;
	// IComp
	template<class This>
	concept tIComp = std::derived_from<This, IComp<This>>;
	// CompDef
	template<class This>
	concept tCompDef = std::derived_from<This, CompDef<This>>;
	// endregion
	// region Bases
	// Thing
	class BaseThing;
	template<class>
	class ThingBase;
	template<class T>
	struct ThingFactoryBase;
	template<class T, class TDef>
	struct ThingFactoryBase1;
	// ThingDriver
	class BaseDriver;
	template<class T>
	class ThingDriverBase;
	template<class T, class TCache>
	class ThingDriverBase2;
	// IThing
	template<class T>
	class IThingBase;
	template<class TDriver>
	class IThingBase2;
	template<class T, class TDriver, class TCache>
	class IThingBase3;
	// ThingDef
	class BaseDef;
	template<class T>
	class ThingDefBase;
	// ThingData
	class BaseData;
	template<class T>
	class DataBase;
	// ThingComp
	class BaseComp;
	template<class T>
	class CompBase;
	template<class T, class C>
	class CompBase1;
	template<class C>
	struct CompFactoryBase;
	template<class T, class C, class CDef>
	struct CompFactoryBase1;
	// CompDriver
	class BaseCompDriver;
	// IComp
	// CompDef
	class BaseCompDef;
	template<class T>
	class CompDefBase;
	template<class C>
	class CompDefBase2;
	// endregion
}

#endif //QUANFLOQ_THINGCONCEPTS_HPP
