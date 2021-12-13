//
// Created by lecris on 2021-12-13.
//

#include "Registrar/TypeInfo.hpp"
#include "Registrar/TypeRegistration.hpp"

using namespace QuanFloq;


// region Constructor/Destructor
TypeRegistrationDataBase::TypeRegistrationDataBase( std::type_index&& type ) :
		type{type} { }
TypeRegistrationData<void>::TypeRegistrationData( std::type_index&& type, std::string_view name,
                                                  FactoryBase* factory, IRegistrar* registrar ) :
		TypeRegistrationDataBase{std::forward<std::type_index>(type)},
		name{name}, factory{factory}, registrar{registrar} { }
TypeRegistration::TypeRegistration( const TypeInfo& type ) :
		type{type} { }
// endregion

// region Factory functions
TypeRegistration TypeRegistration::Create( TypeRegistrationDataBase&& data ) {
	auto res = TypeInfo::registrar.Register(std::forward<TypeRegistrationDataBase>(data));
	assert(res.second);
	return TypeRegistration(*res.first);
}
// endregion


// region TypeRegistrationData
std::string_view TypeRegistrationDataBase::MangledName() const {
	return type.name();
}

std::string_view TypeRegistrationData<void>::Name() {
	return name;
}
FactoryBase* TypeRegistrationData<void>::Factory() {
	return factory;
}
IRegistrar* TypeRegistrationData<void>::Registrar() {
	return registrar;
}
// endregion