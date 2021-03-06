//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_INTERFACE_SCRIBER_FACTORYREQUEST_TPP
#define QUANFLOQ_INCLUDE_INTERFACE_SCRIBER_FACTORYREQUEST_TPP

#include "FactoryRequest.hpp"
//#include "interface/IExposable.hpp"
#include "Registrar/Registrars.hpp"
#include <cassert>

using namespace QuanFloq;

// region Constructor/Destructor
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
FactoryRequest<P, T>::FactoryRequest( P<T>& loc, bool scribeData ):
		location{&loc} {
	this->arrayType = false;
	this->values.push_back(*loc);
	if constexpr(std::derived_from<P<T>, std::unique_ptr<T>>) {
		this->sharedType = false;
		this->scribeData = true;
	} else if constexpr(std::derived_from<P<T>, std::shared_ptr<T>>) {
		this->sharedType = true;
		this->scribeData = scribeData;
	} else
		throw std::runtime_error("Could not detect pointer template");
}
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
FactoryRequest<P, T>::FactoryRequest( std::vector<P<T>>& loc, bool scribeData ):
		vecLocation{&loc} {
	this->arrayType = true;
	for (auto& value: loc)
		this->values.push_back(*value);
	if constexpr(std::derived_from<P<T>, std::unique_ptr<T>>) {
		this->sharedType = false;
		this->scribeData = true;
	} else if constexpr(std::derived_from<P<T>, std::shared_ptr<T>>) {
		this->sharedType = true;
		this->scribeData = scribeData;
	} else
		throw std::runtime_error("Could not detect pointer template");
}
// endregion

// region Constructor Helpers
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
std::unique_ptr<FactoryRequestBase> FactoryRequest<P, T>::Clone() const {
	return CloneThis<FactoryRequest<P, T>>(*this);
}
// endregion

// region Interfaces
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
IRegistrationTask FactoryRequest<P, T>::AwaitGet( IRegistrar& registrar, std::string_view name ) {
	if constexpr(std::derived_from<P<T>, std::shared_ptr<T>>) {
		this->requestsPending++;
		if (this->arrayType) {
			assert(vecLocation != nullptr);
			auto vecLocationCopy = vecLocation;
			auto exposable = co_await IRegistrarAwaitGetPtr();
			auto value = std::dynamic_pointer_cast<T>(exposable);
			assert(value != nullptr);
			vecLocationCopy->push_back(value);
		} else {
			assert(location != nullptr);
			auto locationCopy = location;
			assert(this->requestsPending == 1);
			auto exposable = co_await IRegistrarAwaitGetPtr();
			auto value = std::dynamic_pointer_cast<T>(exposable);
			assert(value != nullptr);
			*locationCopy = value;
		}
		this->requestsFinished++;
	} else
		throw std::runtime_error("Can only AwaitGet shared_ptr type");
}
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
void FactoryRequest<P, T>::Get( IRegistrar& registrar, std::string_view name, bool await ) {
	if constexpr(std::derived_from<P<T>, std::shared_ptr<T>>) {
		if (await)
			AwaitGet(registrar, name);
		else {
			this->requestsPending++;
			assert(registrar.Contains(name));
			if (this->arrayType) {
				assert(vecLocation != nullptr);
				auto value = std::dynamic_pointer_cast<T>(registrar.GetPtr(name));
				assert(value != nullptr);
				vecLocation->push_back(value);
			} else {
				assert(location != nullptr);
				assert(this->requestsPending == 1);
				auto value = std::dynamic_pointer_cast<T>(registrar.GetPtr(name));
				assert(value != nullptr);
				*location = value;
			}
			this->requestsFinished++;
		}
	} else
		throw std::runtime_error("Can only AwaitGet shared_ptr type");
}
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
IExposable& FactoryRequest<P, T>::Integrate( std::unique_ptr<T>&& item ) {
	assert(item != nullptr);
	this->requestsPending++;
	this->requestsFinished++;
	if (this->arrayType) {
		assert(vecLocation != nullptr);
		vecLocation->emplace_back(std::move(item));
		return *vecLocation->back();
	} else {
		assert(location != nullptr);
		assert(this->requestsPending == 1);
		*location = std::move(item);
		return **location;
	}
}
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
IExposable& FactoryRequest<P, T>::IntegrateShared( std::shared_ptr<T> item ) {
	if constexpr (!std::derived_from<P<T>, std::shared_ptr<T>>)
		throw std::runtime_error("Request is not shared type");
	else {
		if (!item)
			throw std::runtime_error("Pointer is null");
		this->requestsPending++;
		this->requestsFinished++;
		if (this->arrayType) {
			assert(vecLocation != nullptr);
			vecLocation->emplace_back(std::move(item));
			return *vecLocation->back();
		} else {
			assert(location != nullptr);
			assert(this->requestsPending == 1);
			*location = std::move(item);
			return **location;
		}
	}
}
template<template<class> class P, Exposable T>
requires sptr<P<T>, T>
void FactoryRequest<P, T>::IntegrateShared( std::shared_ptr<IExposable> item ) {
	IntegrateShared(std::dynamic_pointer_cast<T>(item));
}
// endregion

#endif //QUANFLOQ_INCLUDE_INTERFACE_SCRIBER_FACTORYREQUEST_TPP
