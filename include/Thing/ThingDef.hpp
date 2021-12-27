//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_THINGDEF_HPP
#define QUANFLOQ_THINGDEF_HPP

#include "ThingTraits.hpp"
#include "interface/IExposable.hpp"
#include "Registrar/TypeInfo.hpp"
#include "Registrar/SharedRegistrar.hpp"
#include "Registrar/Factory.hpp"
#include <vector>

namespace QuanFloq {
	// region Bases
	/**
	 * Base class of ThingDef.
	 *
	 * Contains registrars of the created ThingDefs, its generators,
	 * and default save/load logic.
	 */
	class BaseDef :
			public IExposable {
	public:
		using registrar_type = SharedRegistrarRoot<BaseDef>;
		/// Registrar containing all created ThingDefs
		static registrar_type registrar;
		static std::vector<std::shared_ptr<BaseDef>> LoadDefs( std::string_view location );
		/// Unique name of the ThingDef retrieved from file
		std::string name;
		/// Necessary library for loading the linked Driver
		std::shared_ptr<DLibrary> driverLibrary;
		/// Type info of defined Thing
		const TypeInfo* thing;
		/// Thing's Driver
		std::shared_ptr<BaseDriver> driver;
		/// Comps being generated
		std::vector<std::shared_ptr<BaseCompDef>> comps;
		std::shared_ptr<ScribeDriver> scriberDriver;
		BaseFactory& thingFactory;

	protected:
		explicit BaseDef( BaseFactory& factory );
	public:
		void ExposeData( Scriber& base ) override;
		std::string_view GetName() const override;
	};
	inline BaseDef::registrar_type BaseDef::registrar;
	template<class T>
	class ThingDefBase :
			public BaseDef {
	public:
		using thing_type = T;
		using thingFactory_type = ThingFactory<thing_type>;
		thingFactory_type& thingFactory;
	protected:
		explicit ThingDefBase( thingFactory_type& factory );
	};
	// endregion

	// region Main templates
	template<class Traits>
	class ThingDef_ :
			public ThingDefBase<typename Traits::thing_type> {
		static_assert(tDefTraits<Traits>,
		              "Traits is not a DefTrait");
	public:
		using alias = ThingDef_<Traits>;
		using base = ThingDefBase<typename Traits::thing_type>;
		using def_type = typename Traits::def_type;
		using thing_type = typename Traits::thing_type;
		using thingFactory_type = typename ThingTraits<thing_type>::factory_type;
		using factory_type = typename Traits::factory_type;
		using registrar_type = typename Traits::registrar_type;

		static const TypeInfo* const typeInfo;
		static_assert(&typeInfo);
		static registrar_type registrar;
		static_assert(&registrar);
		static factory_type factory;
		static_assert(&factory);
		thingFactory_type thingFactory;
		ThingDef_();
	public:
		void ExposeData( Scriber& base ) override;
	};
	// endregion

	// region Helper classes
	template<class Traits>
	struct ThingDefFactory_ :
			FactoryWithBases<typename Traits::def_type,
					ThingDef_<typename Traits::defTraits>,
					ThingDefBase<typename Traits::thing_type>,
					BaseDef> {
		static_assert(tDefTraits<Traits>,
		              "Traits is not a ThingDefTrait");
		using thing_type = typename Traits::thing_type;
		using def_type = typename Traits::def_type;
		std::unique_ptr<def_type> Make( IExposable* parent ) override;
		std::shared_ptr<def_type> MakeShared( IExposable* parent ) override;
	};
	// endregion
}

#endif //QUANFLOQ_THINGDEF_HPP
