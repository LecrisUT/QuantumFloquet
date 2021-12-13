//
// Created by Le Minh Cristian on 2021/10/29.
//

#ifndef QUANFLOQ_TYPEINFO_HPP
#define QUANFLOQ_TYPEINFO_HPP

#include "RegistrarConcepts.hpp"
#include <typeindex>
#include <string>
#include <memory>

namespace QuanFloq {
	// Forward Declarations
	struct TypeRegistrar;
	struct TypeRegistrationDataBase;
	struct FactoryBase;

	struct dynamic_library :
			std::enable_shared_from_this<dynamic_library> {
		static SharedRegistrarRoot<dynamic_library> registrar;
		static std::shared_ptr<dynamic_library> Create( std::string_view libray );

		static void CloseDL( void* ptr );
		const std::string Location;
		const std::unique_ptr<void, decltype(&CloseDL)> DLL;
	private:
		explicit dynamic_library( std::string_view library );

	public:
		std::string_view GetName() const;
		bool operator==( const dynamic_library& t2 ) const;
	};

	struct TypeInfo {
		friend TypeRegistrar;
		static TypeRegistrar registrar;

		const std::type_index type;
		const std::string name;
		const std::string mangledName;
		mutable std::shared_ptr<dynamic_library> library;
		FactoryBase* factory;
		IRegistrar* iRegistrar;
		static TypeInfo Create( TypeRegistrationDataBase&& data );
	private:
		explicit TypeInfo( TypeRegistrationDataBase&& data );

	public:
		std::string_view GetName() const;
		bool operator==( const TypeInfo& t2 ) const;
		bool operator==( const std::type_index& t2 ) const;
	};
}

// Ugly post include needed in order to define inline instantiation in the header
#include "Registrars.tpp"
#include "TypeRegistrar.hpp"

inline QuanFloq::SharedRegistrarRoot<QuanFloq::dynamic_library> QuanFloq::dynamic_library::registrar;
inline QuanFloq::TypeRegistrar QuanFloq::TypeInfo::registrar;

#endif //QUANFLOQ_TYPEINFO_HPP
