//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_THINGCOMP_HPP
#define QUANFLOQ_THINGCOMP_HPP

#include "ThingTraits.hpp"
#include "interface/IExposable.hpp"
#include "Registrar/TypeInfo.hpp"
#include "Registrar/Factory.hpp"

namespace QuanFloq {
	// region Bases
	class BaseComp :
			public IExposable {
	public:
		/// Comp's static Definition. Also contains constructor.
		std::shared_ptr<BaseCompDef> def;
		/// Linked base Thing for keeping the object alive.
		BaseThing& parent;

		std::string name;
	protected:
		BaseComp( BaseThing& parent, std::shared_ptr<BaseCompDef> def );
	public:
		void ExposeData( Scriber& scriber ) override;
		std::string_view GetName() const override;
	};


	template<class T>
	class CompBase :
			public BaseComp {
	public:
		using thing_type = T;
		thing_type& parent;

	protected:
		CompBase( thing_type& parent, std::shared_ptr<BaseCompDef> def );
	};

	/**
	 * Base Thing Component for adding extensions to Thing avoiding polymorphism.
	 * Can contain relevant data sturcture for interface.
	 */
	template<class C>
	class CompBase2 :
			public CompBase<typename CompTraits<C>::thing_type> {
	public:
		using comp_type = C;
		using thing_type = typename CompTraits<C>::thing_type;
	protected:
		CompBase2( thing_type& parent, std::shared_ptr<BaseCompDef> def );
	};
	// endregion
	// region Main Template
	template<class Traits>
	class ThingComp_ :
			public CompBase2<typename Traits::comp_type> {
		static_assert(tCompTraits<Traits>,
		              "Traits is not a CompTrait");
	public:
		using alias = ThingComp_<Traits>;
		using base = CompBase2<typename Traits::comp_type>;
		using thing_type = typename Traits::thing_type;
		using comp_type = typename Traits::comp_type;
		using driver_type = typename Traits::driver_type;
		using def_type = typename Traits::def_type;
		using factory_type = typename Traits::factory_type;

		static const TypeInfo* const typeInfo;
		static_assert(&typeInfo);
		static_assert(&CompDef<typename CompDefTraits<def_type>::compDefTraits>::typeInfo);

		def_type& def;
		ThingComp_( thing_type& parent, std::shared_ptr<def_type> def );
		void ExposeData( Scriber& scriber ) override;
	};
	// endregion
	// region Helper classes
	template<class Traits>
	struct CompFactory_ :
			public FactoryWithBases<typename Traits::comp_type,
					CompBase2<typename Traits::comp_type>,
					CompBase<typename Traits::thing_type>,
					BaseComp> {
		static_assert(tCompTraits<Traits>,
		              "Traits is not a CompTrait");
		using thing_type = typename Traits::thing_type;
		using comp_type = typename Traits::comp_type;
		using def_type = typename Traits::def_type;
		using driver_type = typename Traits::driver_type;
		std::shared_ptr<def_type> def;
		explicit CompFactory_();
		std::unique_ptr<comp_type> Make( IExposable* parent ) override;
	};
	// endregion
}

#include "CompDriver.hpp"
#include "IComp.hpp"
#include "CompCache.hpp"
#include "CompDef.hpp"

#endif //QUANFLOQ_THINGCOMP_HPP
