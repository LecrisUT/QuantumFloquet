//
// Created by lecris on 2021-12-16.
//

#ifndef QUANFLOQ_INCLUDE_THING_THINGTRAITS_HPP
#define QUANFLOQ_INCLUDE_THING_THINGTRAITS_HPP

#include "ThingConcepts.hpp"
#include "UtilityConcepts.hpp"
//#include "Registrar/Registrars.hpp"
//#include "Registrar/Factory.hpp"

namespace QuanFloq {
	// region Forward declarations
	template<class T, class U>
	struct SharedRegistrar;
	template<class T, class ...Args>
	struct FactoryWithBases;
	// endregion

	// region Main traits
	template<class TModule>
	struct ModuleTraits {
		using thingTraits = void;
		using dataTraits = void;
		using defTraits = void;
		using compDefTraits = void;
		using compTraits = void;
	};
	template<class T>
	struct ThingTraits {
		using thing_type = T;
		using thingTraits = ThingTraits<T>;
		using driver_type = void;
		using interface_type = void;
		using data_type = void;
		using def_type = ThingDef_<ThingDefTraits<thingTraits>>;
		using factory_type = ThingFactory_<thingTraits>;
		using comp_type = CompBase<thing_type>;
	};
	template<class TDriver>
	struct ThingDriverTraits {
		static_assert(!std::same_as<TDriver, TDriver>,
		              "DriverTraits has to be specialized in order to determine thing_type and interface_type");
		using driver_type = TDriver;
		using driverTraits = ThingDriverTraits<TDriver>;
		using thing_type = void;
		using cache_type = void;
		using interface_type = void;
		using registrar_type = SharedRegistrar<driver_type, BaseDriver>;
	};
	template<class IT>
	struct IThingTraits {
		static_assert(!std::same_as<IT, IT>,
		              "IThingTraits has to be specialized in order to determine thing_type and driver_type");
		using interface_type = IT;
		using iThingTraits = IThingTraits<IT>;
		using thing_type = void;
		using driver_type = void;
		using cache_type = void;
	};
	template<class TDef>
	struct ThingDefTraits {
		static_assert(!std::same_as<TDef, TDef>,
		              "DefTraits has to be specialized in order to determine thing_type");
		using def_type = TDef;
		using defTraits = ThingDefTraits<TDef>;
		using thing_type = void;
		using registrar_type = SharedRegistrar<def_type, BaseDef>;
		using factory_type = ThingDefFactory_<defTraits>;
	};
	template<class TData>
	struct ThingDataTraits {
		static_assert(!std::same_as<TData, TData>,
		              "DataTraits has to be specialized in order to determine thing_type");
		using data_type = TData;
		using dataTraits = ThingDataTraits<TData>;
		using thing_type = void;
		using factory_type = ThingDataFactory<TData>;
	};
	template<class C>
	struct CompTraits {
		static_assert(!std::same_as<C, C>,
		              "CompTraits has to be specialized in order to determine thing_type");
		using comp_type = C;
		using compTraits = CompTraits<C>;
		using thing_type = void;
		using driver_type = void;
		using interface_type = void;;
		using def_type = CompDef_<CompDefTraits<compTraits>>;
		using factory_type = CompFactory_<compTraits>;
	};
	template<class CDriver>
	struct CompDriverTraits {
		static_assert(!std::same_as<CDriver, CDriver>,
		              "CompDriverTraits has to be specialized in order to determine comp_type and interface_type");
		using driver_type = CDriver;
		using compDriverTraits = CompDriverTraits<CDriver>;
		using comp_type = void;
		using cache_type = void;
		using interface_type = void;
		using registrar_type = SharedRegistrar<driver_type, BaseCompDriver>;
	};
	template<class IC>
	struct ICompTraits {
		static_assert(!std::same_as<IC, IC>,
		              "ICompTraits has to be specialized in order to determine comp_type and driver_type");
		using interface_type = IC;
		using iCompTraits = ICompTraits<IC>;
		using comp_type = void;
		using driver_type = void;
		using cache_type = void;
	};
	template<class CDef>
	struct CompDefTraits {
		static_assert(!std::same_as<CDef, CDef>,
		              "CompDef has to be specialized in order to determine at least comp_type");
		using def_type = CDef;
		using compDefTraits = CompDefTraits<CDef>;
		using comp_type = void;
//		using thing_type = typename CompTraits<comp_type>::thing_type;
		using thing_type = void;
		using registrar_type = SharedRegistrar<def_type, BaseCompDef>;
		using factory_type = CompDefFactory_<compDefTraits>;
	};
	// endregion
	// region Some specializations
	template<>
	struct ThingDriverTraits<void> {
		using driver_type = void;
		using thing_type = void;
		using cache_type = void;
		using interface_type = void;
		using registrar_type = void;
		using driverTraits = void;
	};
	template<>
	struct CompDriverTraits<void> {
		using driver_type = void;
		using comp_type = void;
		using thing_type = void;
		using cache_type = void;
		using interface_type = void;
		using registrar_type = void;
		using compDriverTraits = void;
	};
	// endregion
	// region Recursive traits overloads
	// Thing
	template<class T>
	struct ThingTraits<ThingTraits<T>> :
			ThingTraits<T>::thingTraits {
	};
	template<class This>
	concept HasThingTraits = requires{
		typename This::thingTraits;
	} && SpecializationOf<typename This::thingTraits, ThingTraits>;
	template<HasThingTraits T>
	struct ThingTraits<T> :
			T::thingTraits {

	};
	// ThingDriver
	template<class T>
	struct ThingDriverTraits<ThingDriverTraits<T>> :
			ThingDriverTraits<T>::driverTraits {
	};
	template<class T>
	struct IThingTraits<IThingTraits<T>> :
			IThingTraits<T>::iThingTraits {
	};
	template<class T>
	struct ThingDefTraits<ThingDefTraits<T>> :
			ThingDefTraits<T>::defTraits {
	};
	template<class T>
	struct ThingDataTraits<ThingDataTraits<T>> :
			ThingDataTraits<T>::dataTraits {
	};
	template<class T>
	struct CompTraits<CompTraits<T>> :
			CompTraits<T>::compTraits {
	};
	template<class T>
	struct CompDriverTraits<CompDriverTraits<T>> :
			CompDriverTraits<T>::compDriverTraits {
	};
	template<class T>
	struct ICompTraits<ICompTraits<T>> :
			ICompTraits<T>::iCompTraits {
	};
	template<class T>
	struct CompDefTraits<CompDefTraits<T>> :
			CompDefTraits<T>::compDefTraits {
	};
	// endregion
	// region Default bases
	template<class T, class TDriver = void, class TData = void>
	struct DefaultThingTraits {
		using thing_type = T;
		using thingTraits = ThingTraits<T>;
		using driver_type = TDriver;
		using interface_type = typename ThingDriverTraits<TDriver>::interface_type;
		using data_type = TData;
		using def_type = ThingDef_<ThingDefTraits<thingTraits>>;
		using factory_type = ThingFactory_<thingTraits>;
		using comp_type = CompBase<thing_type>;
	};
	template<class TDriver, class T, class IT, class TCache = void>
	struct DefaultThingDriverTraits {
		using driver_type = TDriver;
		using driverTraits = ThingDriverTraits<TDriver>;
		using thing_type = T;
		using cache_type = TCache;
		using interface_type = IT;
		using registrar_type = SharedRegistrar<driver_type, BaseDriver>;
	};
	template<class IT, class T, class TDriver, class TCache = void>
	struct DefaultIThingTraits {
		using interface_type = IT;
		using iThingTraits = IThingTraits<IT>;
		using thing_type = T;
		using driver_type = TDriver;
		using cache_type = TCache;
	};
	template<class TDef, class T>
	struct DefaultThingDefTraits {
		using def_type = TDef;
		using defTraits = ThingDefTraits<TDef>;
		using thing_type = T;
		using registrar_type = SharedRegistrar<def_type, BaseDef>;
		using factory_type = ThingDefFactory_<defTraits>;
	};
	template<class TData, class T>
	struct DefaultThingDataTraits {
		using data_type = TData;
		using dataTraits = ThingDataTraits<TData>;
		using thing_type = T;
		using factory_type = ThingDataFactory_<dataTraits>;
	};
	template<class C, class T, class CDriver = void>
	struct DefaultCompTraits {
		using comp_type = C;
		using compTraits = CompTraits<C>;
		using thing_type = T;
		using driver_type = CDriver;
		using interface_type = typename CompDriverTraits<CDriver>::interface_type;
		using def_type = CompDef_<CompDefTraits<compTraits>>;
		using factory_type = CompFactory_<compTraits>;
	};
	template<class CDriver, class C, class IC, class CCache = void>
	struct DefaultCompDriverTraits {
		using driver_type = CDriver;
		using compDriverTraits = CompDriverTraits<CDriver>;
		using comp_type = C;
		using cache_type = CCache;
		using interface_type = IC;
		using registrar_type = SharedRegistrar<driver_type, BaseCompDriver>;
	};
	template<class IC, class C, class CDriver, class CCache = void>
	struct DefaultICompTraits {
		using interface_type = IC;
		using iCompTraits = ICompTraits<IC>;
		using comp_type = C;
		using driver_type = CDriver;
		using cache_type = CCache;
	};
	template<class CDef, class C>
	struct DefaultCompDefTraits {
		using def_type = CDef;
		using compDefTraits = CompDefTraits<CDef>;
		using comp_type = C;
		using thing_type = typename CompTraits<C>::thing_type;
		using registrar_type = SharedRegistrar<def_type, BaseCompDef>;
		using factory_type = CompDefFactory_<compDefTraits>;
	};
	// endregion
	// region Overloads for Automatically created types
	// Thing, Driver, IThing, Data, Comp, CompDriver and IComp are manually created by design
	template<class T>
	struct ThingDefTraits<ThingTraits<T>> {
		using thingTraits = typename ThingTraits<T>::thingTraits;
		using def_type = ThingDef_<ThingDefTraits<thingTraits>>;
		using defTraits = ThingDefTraits<thingTraits>;
		using thing_type = typename thingTraits::thing_type;
		using registrar_type = SharedRegistrar<def_type, BaseDef>;
		using factory_type = ThingDefFactory_<defTraits>;
	};
	template<class T>
	struct ThingDefTraits<ThingDef_<ThingDefTraits<ThingTraits<T>>>> {
		using thingTraits = typename ThingTraits<T>::thingTraits;
		using defTraits = ThingDefTraits<thingTraits>;
		using def_type = typename defTraits::def_type;
		using thing_type = typename defTraits::thing_type;
		using registrar_type = typename defTraits::registrar_type;
		using factory_type = typename defTraits::factory_type;
	};
	template<class T>
	struct CompDefTraits<CompTraits<T>> {
		using compTraits = typename CompTraits<T>::compTraits;
		using def_type = CompDef_<CompDefTraits<compTraits>>;
		using compDefTraits = CompDefTraits<compTraits>;
		using comp_type = typename compTraits::comp_type;
		using thing_type = typename compTraits::thing_type;
		using registrar_type = SharedRegistrar<def_type, BaseCompDef>;
		using factory_type = CompDefFactory_<compDefTraits>;
	};
	template<class T>
	struct CompDefTraits<CompDef_<CompDefTraits<CompTraits<T>>>> {
		using compTraits = typename CompTraits<T>::compTraits;
		using compDefTraits = CompDefTraits<compTraits>;
		using def_type = typename compDefTraits::def_type;
		using comp_type = typename compDefTraits::comp_type;
		using thing_type = typename compDefTraits::thing_type;
		using registrar_type = typename compDefTraits::registrar_type;
		using factory_type = typename compDefTraits::factory_type;
	};
	// endregion
	// region Helper concepts
	template<class This>
	concept HasDefTraits = requires{
		typename ThingDefTraits<This>;
	};
	template<class This>
	concept HasDataTraits = requires{
		typename ThingDataTraits<This>;
	};
	template<class This>
	concept HasCompTraits = requires{
		typename CompTraits<This>;
	};
	template<class This>
	concept HasCompDefTraits = requires{
		typename CompDefTraits<This>;
	};
	template<class This>
	concept HasThingDriver = !std::same_as<typename ThingTraits<This>::driver_type, void>;
	template<class This>
	concept HasThingData = !std::same_as<typename ThingTraits<This>::data_type, void>;
	// endregion
}

#endif //QUANFLOQ_INCLUDE_THING_THINGTRAITS_HPP
