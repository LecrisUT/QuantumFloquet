//
// Created by lecris on 2021-12-10.
//

#ifndef QUANFLOQ_LIBTYPEINFO_HPP
#define QUANFLOQ_LIBTYPEINFO_HPP

#include "libTypeInfoBase.hpp"

namespace QuanFloq {
	struct F {
		static const TypeInfo& typeInfo;
	};
	struct D1 : D<D1> {
		void Dummy();
	};
	// Not sure why this works
	struct D2 : D<D2> {
	};
	struct D3 : D<D3> {
		using D<D3>::D;
	};
	struct E1 : E<E1> {
		void Dummy();
	};
	struct E2 : E<E2> {
		using E<E2>::E;
	};
	struct E3 : E<E3> {
		E3();
	};
	inline const TypeInfo& F::typeInfo = TypeInfo::Create<F>();
}

#endif //QUANFLOQ_LIBTYPEINFO_HPP
