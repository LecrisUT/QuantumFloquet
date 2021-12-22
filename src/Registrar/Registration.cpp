//
// Created by lecris on 2021-12-09.
//

#include "Registrar/Registration.hpp"

using namespace QuanFloq;

// region Constructor/Destructor
RefRegistration::RefRegistration( void* ref ) :
		reference{ref} { }
PtrRegistration::PtrRegistration( std::shared_ptr<void> ptr ) :
		pointer{std::move(ptr)} { }
// endregion
const char* RegistrationException::what() const noexcept {
	return "Failed to register item";
}
