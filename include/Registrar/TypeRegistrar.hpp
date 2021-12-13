//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRAR_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRAR_HPP

#include "TypeInfo.hpp"
#include "Registrars.tpp"

namespace QuanFloq {
	struct TypeRegistrar :
			ObjectRegistrar<TypeInfo> {
		using ObjectRegistrar::operator[];
		using ObjectRegistrar::Contains;
		bool Contains( const std::type_index& type ) const;
		const TypeInfo& operator[]( const std::type_index& type ) const;
	};
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_TYPEREGISTRAR_HPP
