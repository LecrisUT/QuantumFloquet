//
// Created by lecris on 2021-12-09.
//

#ifndef QUANFLOQ_LIBREGISTRATION_HPP
#define QUANFLOQ_LIBREGISTRATION_HPP

#include "Registrar/Registrars.tpp"
#include "Registrar/Registration.tpp"

namespace QuanFloq {
	struct IExposable {
		virtual ~IExposable() = default;
	};

	class A {

	public:
		static SharedRegistrarRoot<A> registrar;
		virtual std::string_view GetName() const = 0;
	};
	inline SharedRegistrarRoot<A> A::registrar;
	class A1 :
			public A {
	public:
		static PtrRegistration registration;
		std::string_view GetName() const override;
	};
	class B {
	public:
		static RefRegistrarRoot<B> registrar;
		virtual std::string_view GetName() const = 0;
	};
	inline RefRegistrarRoot<B> B::registrar;
	class B1 :
			public B {
	public:
		static B1 instance;
		static RefRegistration registration;
		std::string_view GetName() const override;
	};

	inline B1 B1::instance;
	inline PtrRegistration A1::registration = PtrRegistration::Create<A1>();
	inline RefRegistration B1::registration = RefRegistration::Create(B1::instance);
}

#endif //QUANFLOQ_LIBREGISTRATION_HPP
