//
// Created by lecris on 2021-11-16.
//

#ifndef QUANFLOQ_COMPCACHE_HPP
#define QUANFLOQ_COMPCACHE_HPP

#include "ThingTraits.hpp"
#include <memory>

namespace QuanFloq {
	template<class Traits>
	struct CompCache_ {
		static_assert(tCompTraits<Traits>,
		              "Traits is not a CompTrait");
		using thing_type = typename Traits::thing_type;
		using comp_type = typename Traits::comp_type;
		thing_type& parent;
		/// Cached dependant pointer used to keep the cached object alive.
		std::shared_ptr<comp_type> dependent;

	protected:
		explicit CompCache_( std::shared_ptr<comp_type> dependent );
	};
}
#endif //QUANFLOQ_COMPCACHE_HPP
