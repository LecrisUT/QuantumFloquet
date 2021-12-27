//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_THINGCACHE_TPP
#define QUANFLOQ_THINGCACHE_TPP

#include "ThingCache.hpp"

using namespace QuanFloq;

template<class Traits>
ThingCache_<Traits>::ThingCache_( std::shared_ptr<thing_type> dependent ) :
		dependent{std::move(dependent)} { }

#endif //QUANFLOQ_THINGCACHE_TPP
