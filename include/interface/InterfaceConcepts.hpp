//
// Created by lecris on 2021-12-23.
//

#ifndef QUANFLOQ_INCLUDE_INTERFACE_INTERFACECONCEPTS_HPP
#define QUANFLOQ_INCLUDE_INTERFACE_INTERFACECONCEPTS_HPP

#include <concepts>

namespace QuanFloq {
	// Forward declarations
	class IExposable;

	template<class T>
	concept Exposable = std::derived_from<T, IExposable>;
}

#endif //QUANFLOQ_INCLUDE_INTERFACE_INTERFACECONCEPTS_HPP
