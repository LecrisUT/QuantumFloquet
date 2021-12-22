//
// Created by Le Minh Cristian on 2021/10/29.
//

#ifndef QUANFLOQ_REGISTRATION_HPP
#define QUANFLOQ_REGISTRATION_HPP

#include "UtilityConcepts.hpp"
#include "RegistrarConcepts.hpp"
#include "Registrars.hpp"
#include <memory>
#include <exception>

namespace QuanFloq {
	// region Framework types
	struct RegistrationException : std::exception {
		const char* what() const noexcept override;
	};
	// endregion
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
		static RefRegistration Create( T& staticItem ) {
			bool res = T::registrar.Register(staticItem).second;
			if (!res)
				throw RegistrationException();
			return RefRegistration(&staticItem);
		}
		template<class Base, std::derived_from<Base> T>
		static RefRegistration Create( RefRegistrarRoot<Base>& registrar, T& staticItem ) {
			bool res = registrar.Register(staticItem).second;
			if (!res)
				throw RegistrationException();
			return RefRegistration(&staticItem);
		}
	};
	struct PtrRegistration :
			RegistrationBase {
		std::shared_ptr<void> pointer;
	private:
		explicit PtrRegistration( std::shared_ptr<void> ptr );
	public:
		template<stdSharedRegistrar T, class ...Args>
		requires std::constructible_from<T, Args...>
		static PtrRegistration Create( Args&& ... args ) {
			auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
			bool res = T::registrar.Register(ptr).second;
			if (!res)
				throw RegistrationException();
			return PtrRegistration(ptr);
		}
		template<class T, class Base, class ...Args>
		requires std::derived_from<T, Base> && std::constructible_from<T, Args...>
		static PtrRegistration Create( SharedRegistrarRoot<Base>& registrar, Args&& ... args ) {
			auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
			bool res = registrar.Register(ptr).second;
			if (!res)
				throw RegistrationException();
			return PtrRegistration(ptr);
		}
	};
	// endregion
}

#endif //QUANFLOQ_REGISTRATION_HPP
