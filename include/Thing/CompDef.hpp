//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_COMPDEF_HPP
#define QUANFLOQ_COMPDEF_HPP

#include "ThingTraits.hpp"
#include "interface/IExposable.hpp"
#include "Registrar/SharedRegistrar.hpp"
#include "Registrar/TypeInfo.hpp"
#include "Registrar/Factory.hpp"
#include <vector>
#include <list>

namespace QuanFloq {
	class DLibrary;

	// region Bases
	class BaseCompDef :
			public IExposable {
	public:
		using registrar_type = SharedRegistrarRoot<BaseCompDef>;
		static registrar_type registrar;
		static std::vector<std::shared_ptr<BaseCompDef>> LoadDefs( std::string_view location );
		std::string name;
		std::shared_ptr<DLibrary> driverLibrary;
		/// Type info of defined Comp
		const TypeInfo* comp;
		/// Type info of Thing's Driver
		std::shared_ptr<BaseCompDriver> driver;
		//  TODO: implement searcher for this approach
		/// Optional Def of Parent to be attached to
		std::shared_ptr<BaseDef> parent;
		std::shared_ptr<ScribeDriver> scriberDriver;
		BaseFactory& compFactory;
	protected:
		explicit BaseCompDef( BaseFactory& factory );
	public:
		void ExposeData( Scriber& base ) override;
		std::string_view GetName() const override;
	};
	inline BaseCompDef::registrar_type BaseCompDef::registrar;
	template<class T>
	class CompDefBase :
			public BaseCompDef {
	public:
		using thing_type = T;
	protected:
		explicit CompDefBase( BaseFactory& factory );
	};
	template<class C>
	class CompDefBase2 :
			public CompDefBase<typename CompTraits<C>::thing_type> {
	public:
		using comp_type = C;
		using thing_type = typename CompTraits<C>::thing_type;
		using compFactory_type = CompFactory<comp_type>;
		compFactory_type& compFactory;
	protected:
		explicit CompDefBase2( compFactory_type& factory );
	};
	// endregion

	// region Main templates

	template<class Traits>
	class CompDef_ :
			public CompDefBase2<typename Traits::comp_type> {
		static_assert(tCompDefTraits<Traits>,
		              "Traits is not a CompDefTrait");
	public:
		using alias = CompDef_<Traits>;
		using base = CompDefBase2<typename Traits::comp_type>;
		using comp_type = typename Traits::comp_type;
		using def_type = typename Traits::def_type;
		using compFactory_type = typename CompTraits<comp_type>::factory_type;
		using factory_type = typename Traits::factory_type;
		using registrar_type = typename Traits::registrar_type;

		static const TypeInfo* const typeInfo;
		static_assert(&typeInfo);
		static registrar_type registrar;
		static_assert(&registrar);
		static factory_type factory;
		static_assert(&factory);
		compFactory_type compFactory;
		CompDef_();
		void ExposeData( Scriber& base ) override;
	};
	// endregion

	// region Helper classes
	template<class Traits>
	struct CompDefFactory_ :
			FactoryWithBases<typename Traits::def_type,
					CompDef_<typename Traits::compDefTraits>,
					CompDefBase2<typename Traits::comp_type>,
					CompDefBase<typename Traits::thing_type>,
					BaseCompDef> {
		static_assert(tCompDefTraits<Traits>,
		              "Traits is not a CompDefTrait");
		using comp_type = typename Traits::comp_type;
		using def_type = typename Traits::def_type;
		std::unique_ptr<def_type> Make( IExposable* parent ) override;
		std::shared_ptr<def_type> MakeShared( IExposable* parent ) override;
	};
	// endregion
}


#endif //QUANFLOQ_COMPDEF_HPP
