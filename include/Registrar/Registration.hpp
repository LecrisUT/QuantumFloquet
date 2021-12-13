//
// Created by Le Minh Cristian on 2021/10/29.
//

#ifndef QUANFLOQ_REGISTRATION_HPP
#define QUANFLOQ_REGISTRATION_HPP

#include "UtilityConcepts.hpp"
#include "RegistrarConcepts.hpp"
#include <memory>

namespace QuanFloq {
	// region Bases
	struct RegistrationBase {
	protected:
		RegistrationBase() = default;
	};
	// endregion
	// region Implementations
	struct RefRegistration :
			RegistrationBase {
		void* reference;
	private:
		explicit RefRegistration( void* ref );
	public:
		template<stdRegistrar T>
		static RefRegistration Create( T& staticItem );
		template<class Base, std::derived_from<Base> T>
		static RefRegistration Create( RefRegistrarRoot<Base>& registrar, T& staticItem );
	};
	struct PtrRegistration :
			RegistrationBase {
		std::shared_ptr<void> pointer;
	private:
		explicit PtrRegistration( std::shared_ptr<void> ptr );
	public:
		template<stdSharedRegistrar T, class ...Args>
		requires ctrArgs<T, Args...>
		static PtrRegistration Create( Args&& ... args );
		template<class T, class Base, class ...Args>
		requires std::derived_from<T, Base> && ctrArgs<T, Args...>
		static PtrRegistration Create( SharedRegistrarRoot<Base>& registrar, Args&& ... args );
	};
	// endregion
}

#endif //QUANFLOQ_REGISTRATION_HPP
