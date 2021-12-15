//
// Created by lecris on 2021-11-10.
//

#include "interface/Scriber/ScribeDriver.hpp"
#include "interface/Scriber/FactoryRequest.hpp"
#include "interface/IExposable.hpp"
#include "Registrar/Registrars.tpp"

using namespace QuanFloq;

RegistrationTask
ScribeDriver::AwaitGet( IRegistrar& iregistrar, std::string_view name, std::unique_ptr<FactoryRequestBase>&& request ) {
	auto requestCopy = std::move(request);
	auto value = co_await IRegistrarAwaitGetPtr();
	assert(value != nullptr);
	requestCopy->Integrate(value);
	co_return;
}
void
ScribeDriver::Register( IRegistrar& iRegistrar, std::string_view name, std::unique_ptr<FactoryRequestBase>&& request,
                        bool await ) {
	if (await)
		AwaitGet(iRegistrar, name, std::move(request));
	else {
		assert(iRegistrar.Contains(name));
		auto value = iRegistrar.GetPtr(name);
		assert(value != nullptr);
		request->Integrate(value);
	}
}
