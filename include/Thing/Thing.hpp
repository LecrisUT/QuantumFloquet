//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_THINGTHING_HPP
#define QUANFLOQ_THINGTHING_HPP

#include "ThingTraits.hpp"
#include "interface/IExposable.hpp"
#include "Registrar/Factory.hpp"
#include "Registrar/TypeInfo.hpp"

namespace QuanFloq {
	// region Bases
	class BaseThing :
			// Allows to be saved and loaded
			public IExposable {
	public:
		/// Searchable name
		std::string name;
		/// Object's class definition. Contains driver, components, constructor, static variables, etc.
		std::shared_ptr<BaseDef> def;
		/// Variable data structure.
		std::unique_ptr<BaseData> data;
		/// Component expansions.
		std::vector<std::unique_ptr<BaseComp>> comps;
	protected:
		explicit BaseThing( std::shared_ptr<BaseDef> def );
	public:
		// Specify how the object is saved and loaded
		void ExposeData( Scriber& scriber ) override;
		// Unique name construction and other comparators for quick indexing
		std::string_view GetName() const override;
	};
	template<class T>
	class ThingBase :
			public BaseThing {
	public:
		using thing_type = T;
		explicit ThingBase( std::shared_ptr<BaseDef> def );
	};
	// endregion

	// region Main Template
	template<class Traits>
	class Thing_ :
			public ThingBase<typename Traits::thing_type> {
		// Using static assert instead of template requires in order to use with incomplete types
		static_assert(tThingTraits<Traits>,
		              "Traits is not a ThingTrait");
	public:
		using alias = Thing_<Traits>;
		using base = ThingBase<typename Traits::thing_type>;
		using thing_type = typename Traits::thing_type;
		using comp_type = typename Traits::comp_type;
		using driver_type = typename Traits::driver_type;
		using interface_type = typename Traits::interface_type;
		using def_type = typename Traits::def_type;
		using factory_type = typename Traits::factory_type;
		using data_type = typename Traits::data_type;

		static const TypeInfo* const typeInfo;
		static_assert(&typeInfo);
		static_assert(&ThingDef<typename ThingDefTraits<def_type>::defTraits>::typeInfo);
		data_type* data;
		def_type& def;
		std::vector<comp_type*> comps;
		explicit Thing_( std::shared_ptr<def_type> def );
		void ExposeData( Scriber& scriber ) override;
	};
	// endregion

	// region Helper classes
	template<class Traits>
	struct ThingFactory_ :
			FactoryWithBases<typename Traits::thing_type,
					ThingBase<typename Traits::thing_type>,
					BaseThing> {
		static_assert(tThingTraits<Traits>,
		              "Traits is not a ThingTrait");
		using thing_type = typename Traits::thing_type;
		using def_type = typename Traits::def_type;
		using driver_type = typename Traits::driver_type;
		std::shared_ptr<def_type> def;
		ThingFactory_();
		std::unique_ptr<thing_type> Make( IExposable* parent ) override;
	};
	// endregion
}

#include "ThingDriver.hpp"
#include "IThing.hpp"
#include "ThingCache.hpp"
#include "ThingData.hpp"
#include "ThingDef.hpp"
#include "ThingComp.hpp"

#endif //QUANFLOQ_THINGTHING_HPP
