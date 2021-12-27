//
// Created by Le Minh Cristian on 2021/10/14.
//

#ifndef QUANFLOQ_THINGCACHE_HPP
#define QUANFLOQ_THINGCACHE_HPP

#include "ThingTraits.hpp"
#include <memory>

namespace QuanFloq {
	//  TODO: Add generic std::bind to return parameterless function calls
	//  TODO: Add virtual parser. Interface as (const& this, string, va_list&)
	//  TODO: Also add virtual Driver?: reset, parse
	// Not declaring a common base because no usage for cross casting.
	/**
	 * Base cache class. Saves a pointer to relevant cached object and
	 * cached functions. Pointer of this to be passed to interface function
	 * calls.
	 * @tparam T Derived Thing class
	 */
	template<class Traits>
	struct ThingCache_ {
		static_assert(tThingTraits<Traits>,
		              "Traits is not a ThingTrait");
		using alias = ThingCache<Traits>;
		using thing_type = typename Traits::thing_type;
		//  TODO: Implement cache reset and change to public getter/setter
		/// Cached dependant pointer used to keep the cached object alive.
		std::shared_ptr<thing_type> dependent;

	protected:
		explicit ThingCache_( std::shared_ptr<thing_type> dependent );
	};
}

#endif //QUANFLOQ_THINGCACHE_HPP
