//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_TEST_REGISTRAR_LIBFACTORY_HPP
#define QUANFLOQ_TEST_REGISTRAR_LIBFACTORY_HPP

#include "Registrar/Factory.tpp"

namespace QuanFloq {
	struct IExposable {
		virtual ~IExposable() = default;
	};

	struct A :
			IExposable {
		static Factory<A> factory;
	};
	template<>
	std::string Factory<A>::GetName() const;
	inline Factory<A> A::factory;
}

#endif //QUANFLOQ_TEST_REGISTRAR_LIBFACTORY_HPP
