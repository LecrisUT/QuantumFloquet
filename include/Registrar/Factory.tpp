//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_TPP

#include "FactoryRequest.hpp"
#include "RefRegistrar.tpp"
#include "TypeInfo.hpp"
#include "Factory.hpp"
#include <utility>
#include <cassert>

using namespace QuanFloq;

// region Constructors/Destructors
template<class T>
Factory<T>::Factory() {
	static_assert(Exposable<T>);
	registrar.Register(*this);
	if constexpr(stdTypeRegistered<T>) {
		// Default name convention
		name = T::typeInfo->name + "Factory";
		registrar.RegisterName(name, *this);
	}
}

template<class This, class T>
concept BaseOfTExposable = std::derived_from<T, This> && std::derived_from<This, IExposable>;
template<class T, class... Args>
FactoryWithBases<T, Args...>::FactoryWithBases() {
	static_assert((BaseOfTExposable<Args, T>&&...),
	              "One of the arguments is not a suitable base");
}
// endregion

// region Interface
template<class T>
std::unique_ptr<T> Factory<T>::Make( IExposable* parent ) {
	if constexpr (std::constructible_from<T>)
		return std::make_unique<T>();
	else
		throw std::runtime_error("Cannot Make");
}
template<class T>
std::shared_ptr<T> Factory<T>::MakeShared( IExposable* parent ) {
	if constexpr (std::constructible_from<T>)
		return std::make_shared<T>();
	else
		throw std::runtime_error("Cannot Make");
}
template<class T>
template<class... Args>
requires std::constructible_from<T, Args...>
std::unique_ptr<T> Factory<T>::MakeUnique( Args&& ... args ) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}
template<class T>
IExposable& Factory<T>::Make( FactoryRequestBase& bReq, IExposable* parent ) {
	auto req = dynamic_cast<IFactoryRequest<T>*>(&bReq);
	assert(req != nullptr);
	if (bReq.sharedType) {
		auto item = MakeShared(parent);
		if constexpr (stdSharedRegistrar<T>)
			T::registrar.Register(item);
		if constexpr (stdRefRegistrar<T>)
			T::registrar.Register(*item);
		return req->IntegrateShared(item);
	} else {
		auto item = Make(parent);
		if constexpr (stdRefRegistrar<T>)
			T::registrar.Register(*item);
		return req->Integrate(std::move(item));
	}
}

template<class T, class ...Args>
IExposable& FactoryWithBases<T, Args...>::Make( FactoryRequestBase& bReq, IExposable* parent ) {
	IExposable* value;
	bool result;
	if (bReq.sharedType) {
		auto object = this->MakeShared(parent);
		if constexpr (stdSharedRegistrar<T>)
			T::registrar.Register(object);
		if constexpr (stdRefRegistrar<T>)
			T::registrar.Register(*object);
		result = IntegrateShared<T>(bReq, object, value) || (IntegrateShared<Args>(bReq, object, value) || ...);
	} else {
		auto object = this->Make(parent);
		if constexpr (stdRefRegistrar<T>)
			T::registrar.Register(*object);
		result = Integrate<T>(bReq, std::forward<std::unique_ptr<T>>(object), value) ||
		         (Integrate<Args>(bReq, std::forward<std::unique_ptr<T>>(object), value) || ...);
		assert(object == nullptr);
	}
	if (!result)
		throw std::runtime_error("Item was not integrated with the requester");
	assert(value != nullptr);
	return *value;
}
template<class T, class ...Args>
template<class U>
inline bool FactoryWithBases<T, Args...>::Integrate( FactoryRequestBase& bReq, std::unique_ptr<T>&& object,
                                                     IExposable*& location ) {
	auto request = dynamic_cast<IFactoryRequest<U>*>(&bReq);
	if (request == nullptr)
		return false;
	location = &request->Integrate(std::move(object));
	assert(location != nullptr);
	assert(object == nullptr);
	return true;
}
template<class T, class ...Args>
template<class U>
inline bool FactoryWithBases<T, Args...>::IntegrateShared( FactoryRequestBase& bReq, std::shared_ptr<T> object,
                                                           IExposable*& location ) {
	auto request = dynamic_cast<IFactoryRequest<U>*>(&bReq);
	if (request == nullptr)
		return false;
	location = &request->IntegrateShared(std::move(object));
	assert(location != nullptr);
	assert(object == nullptr);
	return true;
}
// endregion

#endif //QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_TPP
