//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_THING_T_HPP
#define QUANFLOQ_THING_T_HPP

#include "ThingTraits.hpp"
#include "Registrar/Factory.hpp"
#include "Registrar/TypeInfo.hpp"
#include "interface/IExposable.hpp"

namespace QuanFloq {
	// region Bases
	/**
	 * Base Thing variable form data structure. Avoid polymorphism (virtual)
	 * to be able to static_cast when/if necessary. Alternatively use virtual
	 * getters for relevant data in various forms. Assume that data is cacheable.
	 * Generator needs to be defined in TypeInfo. should implement a static GenRequester<Thing_t>
	 */
	struct BaseData :
			IExposable {
		BaseThing& parent;
		std::string name;
	protected:
		explicit BaseData( BaseThing& parent );
	public:
		std::string_view GetName() const override;
	};
	template<class T>
	struct DataBase :
			BaseData {
		T& parent;
	protected:
		explicit DataBase( T& parent );
	};
	// endregion

	// region Main template
	template<class Traits>
	struct ThingData_ :
			DataBase<typename Traits::thing_type> {
		static_assert(tDataTraits<Traits>,
		              "Traits is not a DataTrait");
		using alias = ThingData<Traits>;
		using base = DataBase<typename Traits::thing_type>;
		using thing_type = typename Traits::thing_type;
		using data_type = typename Traits::data_type;
		using factory_type = typename Traits::factory_type;

		static const TypeInfo* const typeInfo;
		static_assert(&typeInfo);
		static factory_type factory;
		static_assert(&factory);
		explicit ThingData_( thing_type& parent );
	};
	// endregion

	// region Helper classes
	template<class Traits>
	class ThingDataFactory_ :
			public FactoryWithBases<typename Traits::data_type,
					DataBase<typename Traits::thing_type>,
					BaseData> {
		static_assert(tDataTraits<Traits>,
		              "Traits is not a DataTrait");
	public:
		using alias = ThingDataFactory<Traits>;
		using base = FactoryWithBases<typename Traits::data_type, DataBase<typename Traits::thing_type>, BaseData>;
		using thing_type = typename Traits::thing_type;
		using data_type = typename Traits::data_type;
		ThingDataFactory_();
		std::unique_ptr<data_type> Make( IExposable* parent ) override;
	};
	// endregion
}

#endif //QUANFLOQ_THING_T_HPP
