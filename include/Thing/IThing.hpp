//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_ITHING_HPP
#define QUANFLOQ_ITHING_HPP

#include "ThingTraits.hpp"
#include <memory>

namespace QuanFloq {
	// region Bases
	template<class T>
	class IThingBase {
	public:
		using thing_type = T;
		thing_type& thing;
	protected:
		IThingBase();
	};
	template<class TDriver>
	class IThingBase2 {
	public:
		using driver_type = TDriver;
	protected:
		/// Flexible driver implementation for this interface.
		std::shared_ptr<driver_type> driver;
		explicit IThingBase2( std::shared_ptr<driver_type> driver );
	};
	template<class T, class TDriver, class TCache>
	class IThingBase3 :
			public IThingBase<T>, public IThingBase2<TDriver> {
	public:
		using thing_type = T;
		using driver_type = TDriver;
		using cache_type = TCache;
	protected:
		//  TODO: Implement public getter and cache reset on driver change.
		/// Auto-cache for this interface.
		std::shared_ptr<cache_type> cache;
		explicit IThingBase3( std::shared_ptr<driver_type> driver );
	};
	template<class T, class TDriver>
	class IThingBase3<T, TDriver, void> :
			public IThingBase<T>, public IThingBase2<TDriver> {
	public:
		using thing_type = T;
		using driver_type = TDriver;
		using cache_type = void;
	protected:
		using IThingBase2<TDriver>::IThingBase2;
	};
	// endregion
	// region Main Template
	template<class Traits>
	class IThing_ :
			public IThingBase3<typename Traits::thing_type,
					typename Traits::driver_type,
					typename Traits::cache_type> {
		static_assert(tIThingTraits<Traits>,
		              "Traits is not a IThingTrait");
	public:
		using alias = IThing_<Traits>;
		using base = IThingBase3<typename Traits::thing_type, typename Traits::driver_type, typename Traits::cache_type>;
		using thing_type = typename Traits::thing_type;
		using driver_type = typename Traits::driver_type;
		using interface_type = typename Traits::interface_type;
		using cache_type = typename Traits::cache_type;
	protected:
		explicit IThing_( std::shared_ptr<driver_type> driver );
	};
	// endregion
}

#endif //QUANFLOQ_ITHING_HPP
