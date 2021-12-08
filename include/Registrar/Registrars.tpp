//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_REGISTRARS_TPP
#define QUANFLOQ_REGISTRARS_TPP

#include "Registrars.hpp"
#include <cassert>

using namespace QuanFloq;

// region Helper functions
namespace QuanFloq {
	template<class T, template<class> class R, class U>
	requires std::derived_from<R<T>, IRegistrar>
	inline void AwaitGetHelper( R<T>& registrar, std::string_view name, U& value, bool await ) {
		if (await) {
			registrar.AwaitGet(name, value);
		} else {
			assert(registrar.Contains(name));
			value = registrar[name];
		}
	}
	template<class T, template<class> class R, class U>
	requires std::derived_from<R<T>, IRegistrar>
	inline void AwaitGetHelper( R<T>& registrar, std::string_view name, std::vector<U>& vector, bool await ) {
		if (await) {
			auto& value = vector.emplace_back();
			registrar.AwaitGet(name, value);
		} else {
			assert(registrar.Contains(name));
			vector.push_back(registrar[name]);
		}
	}
	template<class T>
	void ReorderSet( T& set ) {
		auto iter = set.begin();
		while (iter != std::prev(set.end(), 1)) {
			if (*iter >= *std::next(iter, 1)) {
				auto item = *iter;
				iter = set.erase(iter);
				set.insert(item);
			} else
				++iter;
		}
	}
	// Currently not used because cannot get bucket of equivalent key
//	template<class T>
//	void ReorderUnorderedSet( T& set ) {
//		auto bucket = set.bucket("");
//		auto hasher = set.hash_function();
//		auto emptyhash = hasher("");
//		auto iter = set.begin(bucket);
//		while (iter != set.end(bucket)) {
//			if (hasher(*iter) != emptyhash) {
//				auto item = *iter;
//				iter = set.erase(iter);
//				set.insert(item);
//			} else
//				++iter;
//		}
//	}
}
// endregion

// region Constructors
template<class T>
RefRegistrarRoot<T>::RefRegistrarRoot() {
	static_assert(strComparable<T>);
}
template<class T>
ObjectRegistrar<T>::ObjectRegistrar() {
	static_assert(strComparable<T>);
}
template<class T, strComparable U>
RefRegistrar<T, U>::RefRegistrar() requires stdRegistrar<U> :
		Root{U::registrar} {
	static_assert(std::derived_from<T, U>);
}
template<class T, strComparable U>
RefRegistrar<T, U>::RefRegistrar( RefRegistrarRoot<U>& root ) :
		Root{root} {
	static_assert(std::derived_from<T, U>);
}
template<class T, strComparable U>
SharedRegistrar<T, U>::SharedRegistrar() requires stdRegistrar<U> :
		Root{U::registrar} {
	static_assert(std::derived_from<T, U>);
}
template<class T, strComparable U>
SharedRegistrar<T, U>::SharedRegistrar( SharedRegistrarRoot<U>& root ) :
		Root{root} {
	static_assert(std::derived_from<T, U>);
}

template<class T, class TRegistrar>
RegistrationTask::promise_type::promise_type( TRegistrar& registrar,
                                              std::string_view name, const T*& ):
		registrar{registrar}, name{name} {
	static_assert(strComparable<T>);
	static_assert(std::derived_from<std::remove_reference_t<TRegistrar>, IRegistrar>);
}
template<class T, class TRegistrar>
RegistrationTask::promise_type::promise_type( TRegistrar& registrar,
                                              std::string_view name, std::shared_ptr<T>& ):
		registrar{registrar}, name{name} {
	static_assert(strComparable<T>);
	static_assert(std::derived_from<std::remove_reference_t<TRegistrar>, IRegistrar>);
}
template<class T, template<class> class TRegistrar>
requires std::derived_from<TRegistrar<T>, IRegistrar>
RegistrarAwaitGet<T, TRegistrar>::RegistrarAwaitGet( TRegistrar<T>& registrar ):
		RegistrarAwaitGetBase(), registrar{registrar} { }

// endregion

// region RefRegistrarRoot
template<class T>
const T* RefRegistrarRoot<T>::operator[]( std::string_view str ) const {
	auto key = Set.find(str);
	if (key == Set.end())
		return nullptr;
	return &key->get();
}
template<class T>
bool RefRegistrarRoot<T>::Contains( std::string_view str ) const {
	return Set.contains(str);
}
template<class T>
const void* RefRegistrarRoot<T>::VoidRef( std::string_view str ) const {
	return operator[](str);
}
template<class T>
bool RefRegistrarRoot<T>::TryRegister( const std::shared_ptr<IExposable>& ptr ) {
	if constexpr(std::derived_from<T, IExposable>) {
		auto sptr = std::dynamic_pointer_cast<T>(ptr);
		if (sptr == nullptr)
			return false;
		Register(*sptr);
		return true;
	} else
		return false;
}
template<class T>
bool RefRegistrarRoot<T>::Register( T& item ) {
	if constexpr(stdNamed<T>) {
		// Can check if prior item exists is standard named
		if (Set.contains(item.GetName()))
			return false;
	}
	auto res = Set.insert(item);
	if (!res.second)
		return false;
	if constexpr(stdNamed<T>) {
		// Can resolve dangling registration queue
		auto currName = res.first->get().GetName();
		if (!currName.empty())
			ResolvePostRegister(currName);
	}
	return true;
}
template<class T>
bool RefRegistrarRoot<T>::Erase( T& item ) {
	return Set.erase(item) > 0;
}
template<class T>
bool RefRegistrarRoot<T>::Erase( std::string_view item ) {
	// TODO: Available in C++23
//	return Set.erase(item) > 0;
	auto item2 = Set.find(item);
	if (item2 == Set.end())
		return false;
	return Set.erase(*item2);
}
template<class T>
void RefRegistrarRoot<T>::RepairOrder() {
	ReorderSet(Set);
//	ReorderUnorderedSet(Set);
}

// endregion

// region SharedRegistrar
template<class T>
std::shared_ptr<T> SharedRegistrarRoot<T>::operator[]( std::string_view str ) const {
	auto key = Set.find(str);
	if (key == Set.end())
		return nullptr;
	return *key;
}
template<class T>
bool SharedRegistrarRoot<T>::Contains( std::string_view str ) const {
	assert(RefRegistrarRoot<T>::Contains(str) == Set.contains(str));
	return Set.contains(str);
}
template<class T>
std::shared_ptr<void> SharedRegistrarRoot<T>::VoidPtr( std::string_view str ) const {
	return operator[](str);
}
template<class T>
bool SharedRegistrarRoot<T>::TryRegister( const std::shared_ptr<IExposable>& ptr ) {
	if constexpr(std::derived_from<T, IExposable>) {
		auto sptr = std::dynamic_pointer_cast<T>(ptr);
		if (sptr == nullptr)
			return false;
		Register(sptr);
		return true;
	} else
		return false;
}
template<class T>
bool SharedRegistrarRoot<T>::Register( std::shared_ptr<T> item ) {
	assert(item != nullptr);
	if constexpr(stdNamed<T>)
		// Can check if standard named
		if (Set.contains(item->GetName()))
			return false;
	auto res = Set.insert(item);
	if (!res.second)
		return false;
	if (!RefRegistrarRoot<T>::Register(**res.first)) {
		// ResolvePostRegister is handled here as well
		Set.erase(res.first);
		return false;
	}
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::Erase( std::shared_ptr<T> item ) {
	if (Set.erase(item) == 0)
		return false;
	RefRegistrarRoot<T>::Erase(*item);
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::Erase( std::string_view item ) {
	// TODO: Available in C++23
//	if (Set.erase(item) == 0)
//		return false;
	auto item2 = Set.find(item);
	if (item2 == Set.end())
		return false;
	if (Set.erase(*item2) == 0)
		return false;
	RefRegistrarRoot<T>::Erase(**item2);
	return true;
}
template<class T>
void SharedRegistrarRoot<T>::RepairOrder() {
	ReorderSet(Set);
//	ReorderUnorderedSet(Set);
	RefRegistrarRoot<T>::RepairOrder();
}
// endregion

// region ObjectRegistrar
template<class T>
const T* ObjectRegistrar<T>::operator[]( std::string_view str ) const {
	auto key = Set.find(str);
	if (key == Set.end())
		return nullptr;
	return &*key;
}
template<class T>
bool ObjectRegistrar<T>::Contains( std::string_view str ) const {
	return Set.contains(str);
}
template<class T>
const void* ObjectRegistrar<T>::VoidRef( std::string_view str ) const {
	return operator[](str);
}
template<class T>
template<class... Args>
requires ctrArgs<T, Args...>
std::pair<typename std::set<T>::iterator, bool> ObjectRegistrar<T>::Register( Args&& ... args ) {
	// TODO: This does not pick up friend constructor
	if constexpr (publicCtrArgs<T, Args...>)
		return Set.emplace(std::forward<Args>(args)...);
	else
		return Set.insert(T::Create(std::forward<Args>(args)...));
}
template<class T>
bool ObjectRegistrar<T>::Erase( T& item ) {
	return Set.erase(item) > 0;
}
template<class T>
bool ObjectRegistrar<T>::Erase( std::string_view item ) {
	// TODO: Available in C++23
//	return Set.erase(item) > 0;
	auto item2 = Set.find(item);
	if (item2 == Set.end())
		return false;
	return Set.erase(*item2);
}
template<class T>
void ObjectRegistrar<T>::RepairOrder() {
	// TODO: Set items are const. Either throw or use the same as reference
}
// endregion


// region co_await functions
template<class T, template<class> class TRegistrar>
requires std::derived_from<TRegistrar<T>, IRegistrar>
RegistrarAwaitGet<T, TRegistrar> RegistrationPromise::await_transform( RegistrarAwaitGet<T, TRegistrar>&& awaiter ) {
	auto output = std::move(awaiter);
	output.promise = this;
	return output;
}

template<class T>
void RefRegistrarRoot<T>::Get( std::string_view name, const T*& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
template<class T>
void RefRegistrarRoot<T>::Get( std::string_view name, std::vector<const T*>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
template<class T>
void SharedRegistrarRoot<T>::Get( std::string_view name, std::shared_ptr<T>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
template<class T>
void SharedRegistrarRoot<T>::Get( std::string_view name, std::vector<std::shared_ptr<T>>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
template<class T>
void ObjectRegistrar<T>::Get( std::string_view name, std::vector<const T*>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
template<class T>
void ObjectRegistrar<T>::Get( std::string_view name, const T*& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}

template<class T>
RegistrationTask RefRegistrarRoot<T>::AwaitGet( std::string_view, T*& value ) {
	value = co_await RegistrarAwaitGet(*this);
	assert(value != nullptr);
}
template<class T>
RegistrationTask SharedRegistrarRoot<T>::AwaitGet( std::string_view, std::shared_ptr<T>& value ) {
	value = co_await RegistrarAwaitGet(*this);
	assert(value != nullptr);
}
template<class T>
RegistrationTask ObjectRegistrar<T>::AwaitGet( std::string_view, const T*& value ) {
	value = co_await RegistrarAwaitGet(*this);
	assert(value != nullptr);
}
// endregion

// region NestedRegistrar functions
template<class T, strComparable U>
bool RefRegistrar<T, U>::Register( T& item ) {
	if (!RefRegistrarRoot<T>::Register(item))
		return false;
	if (!Root.Register(item)) {
		Erase(item);
		return false;
	}
	return true;
}
template<class T, strComparable U>
bool RefRegistrar<T, U>::Erase( T& item ) {
	if (!RefRegistrarRoot<T>::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, strComparable U>
bool RefRegistrar<T, U>::Erase( std::string_view item ) {
	if (!RefRegistrarRoot<T>::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, strComparable U>
void RefRegistrar<T, U>::RepairOrder() {
	RefRegistrarRoot<T>::RepairOrder();
	Root.RepairOrder();
}
template<class T, strComparable U>
void RefRegistrar<T, U>::ResolvePostRegister( std::string_view str ) {
	RefRegistrarRoot<T>::ResolvePostRegister(str);
	Root.ResolvePostRegister(str);
}
template<class T, strComparable U>
bool SharedRegistrar<T, U>::Register( std::shared_ptr<T> item ) {
	if (!SharedRegistrarRoot<T>::Register(item))
		return false;
	if (!Root.Register(item)) {
		Erase(item);
		return false;
	}
	return true;
}
template<class T, strComparable U>
bool SharedRegistrar<T, U>::Erase( std::shared_ptr<T> item ) {
	if (!SharedRegistrarRoot<T>::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, strComparable U>
bool SharedRegistrar<T, U>::Erase( std::string_view item ) {
	if (!RefRegistrarRoot<T>::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, strComparable U>
void SharedRegistrar<T, U>::RepairOrder() {
	SharedRegistrarRoot<T>::RepairOrder();
	Root.RepairOrder();
}
template<class T, strComparable U>
void SharedRegistrar<T, U>::ResolvePostRegister( std::string_view str ) {
	SharedRegistrarRoot<T>::ResolvePostRegister(str);
	Root.ResolvePostRegister(str);
}
// endregion

// region co_await structs
template<class T, template<class> class TRegistrar>
requires std::derived_from<TRegistrar<T>, IRegistrar>
auto RegistrarAwaitGet<T, TRegistrar>::await_resume() const {
	// In the end return the requested object to the caller of co_await
	assert(registrar.Contains(promise->name));
	// TODO: Check is not needed if change to reference return
	//  Need  to standardize and throw if trying to access non-existing type,
	//  and add a safe version returning nullptr
	//  STL standard: at() -> throws std::out_of_range; [] -> creates new; no safe get standard (can use get())
	if constexpr(std::derived_from<TRegistrar<T>, SharedRegistrarRoot<T>>) {
		assert(registrar[promise->name] != nullptr);
		// Return is std::shared_ptr
		return registrar[promise->name];
	} else
		// Return is a reference
		return static_cast<T&>(*registrar[promise->name]);
}
// endregion

#endif //QUANFLOQ_REGISTRARS_TPP
