//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_TPP

#include "interface/Scriber/FactoryRequest.hpp"
#include "Registrars.tpp"
#include "TypeInfo.hpp"
#include "TypeRegistration.hpp"
#include "Factory.hpp"
#include <utility>

using namespace QuanFloq;

// region Constructors/Destructors
template<class T>
Factory<T>::Factory() {
	static_assert(Exposable<T>);
	registrar.Register(*this);
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
	return std::make_unique<T>();
}
template<class T>
template<std::constructible_from<T>... Args>
std::unique_ptr<T> Factory<T>::Make( Args&& ... args ) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}
template<class T>
IExposable& Factory<T>::Make( FactoryRequestBase& bReq, IExposable* parent ) {
	auto req = dynamic_cast<IFactoryRequest<T>*>(&bReq);
	assert(req != nullptr);
	return req->Integrate(Make(parent));
}
template<class T>
void Factory<T>::Make( FactoryRequestBase& bReq, IExposable* parent, std::shared_ptr<IExposable>& location ) {
	assert(bReq.sharedType);
	std::shared_ptr<IExposable> object = Make(parent);
	bReq.Integrate(object);
	location = object;
}
template<class T>
std::string Factory<T>::GetName() const {
	static_assert(stdTypeRegistered<T>,
	              "Cannot automatically deduce name for non stdTypeRegistered. GetName() has to be fully specialized");
	return T::typeRegistration.type.name + "Generator";
}

template<class T, class ...Args>
IExposable& FactoryWithBases<T, Args...>::Make( FactoryRequestBase& bReq, IExposable* parent ) {
	IExposable* value;
	auto object = Make(parent);
	bool result = Integrate<T>(bReq, std::forward<std::unique_ptr<T>>(object), value) ||
	              (Integrate<Args>(bReq, std::forward<std::unique_ptr<T>>(object), value) || ...);
	assert(result);
	assert(value != nullptr);
	assert(object == nullptr);
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
// endregion

#endif //QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_TPP
