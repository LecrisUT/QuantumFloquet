//
// Created by Le Minh Cristian on 2021/10/15.
//

#ifndef QUANFLOQ_COMPDRIVER_HPP
#define QUANFLOQ_COMPDRIVER_HPP

#include "ThingTraits.hpp"
#include "Registrar/SharedRegistrar.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace QuanFloq {
	// region Bases
	class BaseCompDriver {
	public:
		using registrar_type = SharedRegistrarRoot<BaseCompDriver>;
		static registrar_type registrar;
	protected:
		virtual ~BaseCompDriver() = default;
	public:
		virtual std::string_view GetName() const = 0;
	};
	inline BaseCompDriver::registrar_type BaseCompDriver::registrar;
	template<class C>
	class CompDriverBase :
			public BaseCompDriver {
	public:
		using comp_type = C;
		using thing_type = typename CompTraits<C>::thing_type;
	};
	template<class C, class CCache>
	class CompDriverBase2 :
			public CompDriverBase<C> {
	public:
		using typename CompDriverBase<C>::thing_type;
		using cache_type = CCache;
	protected:
		std::unordered_map<thing_type*, std::shared_ptr<cache_type>> cacheMap;
	};
	template<class C>
	class CompDriverBase2<C, void> :
			public CompDriverBase<C> {
	public:
		using cache_type = void;

	};
	// endregion

	// region Main templates
	template<class Traits>
	class CompDriver_ :
			public CompDriverBase2<typename Traits::comp_type,
					typename Traits::cache_type> {
		static_assert(tCompDriverTraits < Traits > ,
		              "Traits is not a CompDriverTrait");
	public:
		using alias = CompDriver_<Traits>;
		using base = CompDriverBase2<typename Traits::comp_type, typename Traits::cache_type>;
		using driver_type = typename Traits::driver_type;
		using interface_type = typename Traits::interface_type;
		using registrar_type = typename Traits::registrar_type;
		static registrar_type registrar;
		static_assert(&registrar);
	protected:
		CompDriver_();
	};
	// endregion
}

#endif //QUANFLOQ_COMPDRIVER_HPP
