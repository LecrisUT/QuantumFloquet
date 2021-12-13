//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRATION_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRATION_HPP

#include "UtilityConcepts.hpp"
#include "RegistrarConcepts.hpp"
#include <memory>
#include <typeindex>

namespace QuanFloq {
	struct FactoryBase;
	struct TypeInfo;

	// region TypeRegistration
	struct TypeRegistrationDataBase {
		const std::type_index type;
		virtual std::string_view Name() = 0;
		std::string_view MangledName() const;
		virtual FactoryBase* Factory() = 0;
		virtual IRegistrar* Registrar() = 0;
	protected:
		explicit TypeRegistrationDataBase( std::type_index&& type );
	};
	/**
	 * Specialize any of these functions for custom data
	 * @tparam T
	 */
	template<class T>
	struct TypeRegistrationData :
			TypeRegistrationDataBase {
		std::string name;
		std::string_view Name() override;
		FactoryBase* Factory() override;
		IRegistrar* Registrar() override;
		TypeRegistrationData();
		explicit TypeRegistrationData( std::string_view name );
	};
	template<>
	struct TypeRegistrationData<void> :
			TypeRegistrationDataBase {
		std::string name;
		FactoryBase* factory;
		IRegistrar* registrar;
		std::string_view Name() override;
		FactoryBase* Factory() override;
		IRegistrar* Registrar() override;
		TypeRegistrationData( std::type_index&& type, std::string_view name,
		                      FactoryBase* factory, IRegistrar* registrar );
	};
	struct TypeRegistration {
		const TypeInfo& type;
	private:
		explicit TypeRegistration( const TypeInfo& type );
	public:
		static TypeRegistration Create( TypeRegistrationDataBase&& data );
		template<class T>
		static TypeRegistration Create();
		template<class T>
		static TypeRegistration Create( std::string_view name );
		template<class T, stdTypeRegistered Base>
		static TypeRegistration Create( std::string_view suffix );
//		template<class T>
//		static TypeRegistration Create( std::string_view name,
//		                                std::string_view factory, IRegistrar* registrar = nullptr );
		template<class T>
		static TypeRegistration Create( std::string_view name,
		                                FactoryBase* factory, IRegistrar* registrar = nullptr );
//		template<class T, stdTypeRegistered Base>
//		static TypeRegistration Create( std::string_view suffix,
//		                                std::string_view factory, IRegistrar* registrar = nullptr );
		template<class T, stdTypeRegistered Base>
		static TypeRegistration Create( std::string_view suffix,
		                                FactoryBase* factory, IRegistrar* registrar = nullptr );
	};
	// endregion
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRATION_HPP
