//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_THINGDRIVER_HPP
#define QUANFLOQ_THINGDRIVER_HPP

#include "ThingTraits.hpp"
#include "Registrar/SharedRegistrar.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace QuanFloq {
	// region Bases
	class BaseDriver {
	public:
		using registrar_type = SharedRegistrarRoot<BaseDriver>;
		static registrar_type registrar;
		virtual ~BaseDriver() = default;
		virtual std::string_view GetName() const = 0;
	};
	inline BaseDriver::registrar_type BaseDriver::registrar;
	template<class T>
	class ThingDriverBase :
			public BaseDriver {
	public:
		using thing_type = T;
	};
	template<class T, class TCache>
	class ThingDriverBase2 :
			public ThingDriverBase<T> {
		static_assert(tCache < TCache, T > ,
		              "TCache is not ThingCache");
	public:
		using thing_type = T;
		using cache_type = TCache;
	protected:
		//  TODO: Implement public getter and cache control.
		//  TODO: Test cacheMap size limit.
		/// Cache map to be used for auto-caching.
		std::unordered_map<thing_type*, std::shared_ptr<cache_type>> cacheMap;;
	};
	template<class T>
	class ThingDriverBase2<T, void> :
			public ThingDriverBase<T> {
	public:
		using thing_type = T;
		using cache_type = void;
	};
	// endregion

	// region Main templates
	template<class Traits>
	class ThingDriver_ :
			public ThingDriverBase2<typename Traits::thing_type, typename Traits::cache_type> {
		static_assert(tDriverTraits < Traits > ,
		              "Traits is not a DriverTrait");
	public:
		using alias = ThingDriver_<Traits>;
		using base = ThingDriverBase2<typename Traits::thing_type, typename Traits::cache_type>;
		using thing_type = typename Traits::thing_type;
		using driver_type = typename Traits::driver_type;
		using interface_type = typename Traits::interface_type;
		using cache_type = typename Traits::cache_type;
		using registrar_type = typename Traits::registrar_type;
		static registrar_type registrar;
		static_assert(&registrar);
	protected:
		ThingDriver_();
	};
	// endregion
}

#endif //QUANFLOQ_THINGDRIVER_HPP
