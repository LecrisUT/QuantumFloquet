//
// Created by lecris on 2021-12-10.
//

#ifndef QUANFLOQ_LIBTYPEINFO_HPP
#define QUANFLOQ_LIBTYPEINFO_HPP

#include "Registrar/TypeRegistration.tpp"
#include "Registrar/TypeInfo.hpp"

namespace QuanFloq {
	struct IExposable {
		virtual ~IExposable() = default;
	};
	struct FactoryBase {
	};
	template<class>
	struct Factory : FactoryBase {
	};

	class A {
	public:
		static TypeRegistration typeRegistration;
		static SharedRegistrarRoot<A> registrar;
		static Factory<A> factory;
		virtual std::string_view
		GetName() const { return ""; };
	};
	inline TypeRegistration A::typeRegistration = TypeRegistration::Create<A>();
	inline SharedRegistrarRoot<A> A::registrar;
	inline Factory<A> A::factory;
}

#endif //QUANFLOQ_LIBTYPEINFO_HPP
