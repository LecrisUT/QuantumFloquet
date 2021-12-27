//
// Created by Le Minh Cristian on 2021/10/15.
//

#ifndef QUANFLOQ_ICOMP_HPP
#define QUANFLOQ_ICOMP_HPP

#include "ThingTraits.hpp"
#include <memory>

namespace QuanFloq {
	// region Bases
	template<class C>
	class ICompBase {
	public:
		using comp_type = C;
		comp_type& comp;
	protected:
		ICompBase();
	};
	template<class CDriver>
	class ICompBase2 {
	public:
		using driver_type = CDriver;
	protected:
		/// Flexible driver implementation for this interface.
		std::shared_ptr<driver_type> driver;
		explicit ICompBase2( std::shared_ptr<driver_type> driver );
	};
	template<class C, class CDriver, class CCache>
	class ICompBase3 :
			public ICompBase<C>, public ICompBase2<CDriver> {
	public:
		using typename ICompBase2<CDriver>::driver_type;
		using cache_type = CCache;
	protected:
		//  TODO: Implement public getter and cache reset on driver change.
		/// Auto-cache for this interface.
		std::shared_ptr<cache_type> cache;
		explicit ICompBase3( std::shared_ptr<driver_type> driver );
	};
	template<class C, class CDriver>
	class ICompBase3<C, CDriver, void> :
			public ICompBase<C>, public ICompBase2<CDriver> {
	public:
		using typename ICompBase2<CDriver>::driver_type;
		using cache_type = void;
	protected:
		using ICompBase2<CDriver>::ICompBase2;
	};
	// endregion
	// region Main Template
	template<class Traits>
	class IComp_ :
			public ICompBase3<typename Traits::comp_type,
					typename Traits::driver_type,
					typename Traits::cache_type> {
		static_assert(tICompTraits<Traits>,
		              "Traits is not a ICompTrait");
	public:
		using alias = IComp_<Traits>;
		using base = ICompBase3<typename Traits::comp_type, typename Traits::driver_type, typename Traits::cache_type>;
		using comp_type = typename Traits::comp_type;
		using driver_type = typename Traits::driver_type;
		using interface_type = typename Traits::interface_type;
		using cache_type = typename Traits::cache_type;
	protected:
		IComp_( std::shared_ptr<driver_type> driver );
	};
	// endregion
}

#endif //QUANFLOQ_ICOMP_HPP
