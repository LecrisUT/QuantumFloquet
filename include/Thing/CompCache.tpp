//
// Created by lecris on 2021-11-16.
//

#ifndef QUANFLOQ_COMPCACHE_TPP
#define QUANFLOQ_COMPCACHE_TPP

#include "CompCache.hpp"

using namespace QuanFloq;

template<class Traits>
CompCache_<Traits>::CompCache_( std::shared_ptr<comp_type> dependent ) :
		parent{dependent->parent}, dependent{std::move(dependent)} { }


#endif //QUANFLOQ_COMPCACHE_TPP
