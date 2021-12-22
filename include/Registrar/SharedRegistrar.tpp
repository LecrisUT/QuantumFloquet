//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_SHAREDREGISTRAR_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_SHAREDREGISTRAR_TPP

#include "SharedRegistrar.hpp"
#include <cassert>

using namespace QuanFloq;

template<class T>
using spT = std::shared_ptr<T>;

// region Additional class definitions
template<class T>
template<class Key> requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
struct SharedRegistrarRoot<T>::Queue :
		SharedRegistrarRoot<T>::IQueue, Internal::RegistrarKeyMap<Key> {
	using base = SharedRegistrarRoot<T>::IQueue;
	using base::base;
	void Resolve( ptr_type item ) override {
		assert(item);
		if constexpr (TransparentEqualKey<Key, spT<T>>) {
			auto& operand = this->registrar.operators.equal;
			auto mapCopy = map;
			for (auto&[key, value]: mapCopy)
				if (operand(key, *item)) {
					for (auto& handle: value)
						handle.resume();
					if (value.empty())
						map.erase(key);
				}
		} else {
			auto& operand = this->registrar.operators.less;
			auto mapCopy = map;
			for (auto&[key, value]: mapCopy)
				if (!(operand(key, *item) || operand(*item, key))) {
					for (auto& handle: value)
						handle.resume();
					if (value.empty())
						map.erase(key);
				}
		}
	}
};
template<class T>
template<class Key> requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
struct SharedRegistrarRoot<T>::RegistrationTask<Key>::promise_type : Internal::RegistrarKeyHolder<Key> {
	using base = Internal::RegistrarKeyHolder<Key>;
	SharedRegistrarRoot<T>& registrar;

	task_type get_return_object() {
		return {.handle = GetHandle()};
	}
	std::suspend_never initial_suspend() { return {}; }
	std::suspend_never final_suspend() noexcept { return {}; }
	void return_void() { }
	void unhandled_exception() { }
	awaiter_type await_transform( awaiter_type&& awaiter ) {
		auto output = awaiter;
		output.promise = this;
		return output;
	}
	// Specific overload constructors to ensure it is called by AwaitGet or similar syntax functions
	template<class ...Args>
	promise_type( SharedRegistrarRoot<T>& registrar, const Key& key, Args&& ... ):
			base(key), registrar{registrar} { };
	template<class ...Args>
	requires std::move_constructible<Key>
	promise_type( SharedRegistrarRoot<T>& registrar, Key&& key, Args&& ... ):
			base(std::move(key)), registrar{registrar} { };
	// Additional overloaders for non-static function calls
	template<class U, class ...Args>
	promise_type( U&&, SharedRegistrarRoot<T>& registrar, const Key& key, Args&& ... ):
			base(key), registrar{registrar} { };
	template<class U, class ...Args>
	requires std::move_constructible<Key>
	promise_type( U&&, SharedRegistrarRoot<T>& registrar, Key&& key, Args&& ... ):
			base(std::move(key)), registrar{registrar} { };
	std::coroutine_handle<promise_type> GetHandle() {
		return std::coroutine_handle<promise_type>::from_promise(*this);
	}
};
template<class T>
template<class Key> requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
struct SharedRegistrarRoot<T>::Awaiter {
	using ptr_type = typename SharedRegistrarRoot<T>::ptr_type;
	using task_type = typename SharedRegistrarRoot<T>::task_type<Key>;
	using awaiter_type = typename SharedRegistrarRoot<T>::awaiter_type<Key>;
	using promise_type = typename SharedRegistrarRoot<T>::promise_type<Key>;
	promise_type* promise;
	bool await_ready() const {
		if (promise->registrar.Contains(promise->keyRef))
			return true;
		promise->registrar.AwaitFor(promise->keyRef, promise->GetHandle());
	}
	bool await_suspend( std::coroutine_handle<IRegistrationTask::promise_type> h ) const {
		// If Registrar does not contain the required object, continue to co_await
		return !promise->registrar.Contains(promise->keyRef);
	}
	ptr_type await_resume() const {
		// In the end return the requested object to the caller of co_await
		assert(promise->registrar.Contains(promise->keyRef));
		return promise->registrar[promise->keyRef];
	}
};
template<class T>
struct RegistrarAwaitGet<T, SharedRegistrarRoot> :
		RegistrarAwaitGetBase {
	using value_type = T;
	using registrar_type = SharedRegistrarRoot<T>;

	SharedRegistrarRoot<T>& registrar;
	auto await_resume() const {
		assert(registrar.Contains(promise->name));
		return registrar[promise->name];
	}
	explicit RegistrarAwaitGet( SharedRegistrarRoot<T>& registrar ) :
			registrar{registrar} { };
};
// endregion

// region Interface Functions
// region Getters and Checkers
template<class T>
bool SharedRegistrarRoot<T>::Contains( std::string_view str ) const {
	return map.contains(str);
}
template<class T>
bool SharedRegistrarRoot<T>::Contains( value_type& item ) const {
	return set.contains(&item);
}
template<class T>
bool SharedRegistrarRoot<T>::Contains( value_type* const item ) const {
	return set.contains(item);
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
bool SharedRegistrarRoot<T>::Contains( const Key& key ) const {
	if constexpr (TransparentLessKey<Key, spT<T>>)
		return set.contains(key);
	else {
		auto& operand = operators.equal;
		for (auto& item: set)
			if (operand(item, key))
				return true;
		return false;
	}
}
template<class T>
const IExposable* SharedRegistrarRoot<T>::GetRef( std::string_view str ) const {
	if constexpr (std::derived_from<T, IExposable>)
		return operator[](str).get();
	else {
		throw NotImplemented(*this, "GetRef");
	}
}
template<class T>
std::shared_ptr<IExposable> SharedRegistrarRoot<T>::GetPtr( std::string_view str ) const {
	if constexpr (std::derived_from<T, IExposable>)
		return operator[](str);
	else {
		throw NotImplemented(*this, "GetRef");
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
typename SharedRegistrarRoot<T>::ptr_type SharedRegistrarRoot<T>::operator[]( const Key& key ) const {
	if constexpr (TransparentLessKey<Key, spT<T>>) {
		auto location = set.find(key);
		if (location == set.end())
			throw NotRegistered(*this, "No name calculated");
		return *location;
	} else {
		auto& operand = operators.equal;
		for (auto& item: set)
			if (operand(item, key))
				return item;
		throw NotRegistered(*this, "No name calculated");
	}
}
template<class T>
typename SharedRegistrarRoot<T>::ptr_type SharedRegistrarRoot<T>::operator[]( std::string_view str ) const {
	auto key = map.find(str);
	if (key == map.end())
		return nullptr;
	return key->second;
}
template<class T>
typename SharedRegistrarRoot<T>::ptr_type SharedRegistrarRoot<T>::operator[]( value_type& item ) const {
	auto key = set.find(&item);
	if (key == set.end())
		return nullptr;
	return *key;
}
template<class T>
typename SharedRegistrarRoot<T>::ptr_type SharedRegistrarRoot<T>::operator[]( value_type* const item ) const {
	auto key = set.find(item);
	if (key == set.end())
		return nullptr;
	return *key;
}
// endregion
// region Register and Erase
template<class T>
bool SharedRegistrarRoot<T>::TryRegister( std::shared_ptr<IExposable> ptr ) {
	auto valuePtr = std::dynamic_pointer_cast<value_type>(ptr);
	if (!valuePtr)
		return false;
	return Register(valuePtr).second;
}
template<class T>
std::pair<typename SharedRegistrarRoot<T>::set_type::iterator, bool> SharedRegistrarRoot<T>::Register( ptr_type item ) {
	if constexpr (NameResolvable<T>) {
		auto res = map.find(Resolver<T>::Name(*item));
		if (res != map.end()) {
			auto location = set.find(res->second);
			if (location != set.end())
				// If it is the same item, consider successful, otherwise a conflict
				return {location, *location == item};
		}
	}
	auto res = set.insert(item);
	if (!res.second)
		return res;
	std::string_view currName;
	if constexpr (NameResolvable<T>) {
		currName = Resolver<T>::Name(*item);
		if (!currName.empty()) {
			if (map.contains(currName)) {
				if (map.at(currName) != item)
					throw NotRegistered(*this, currName);
			} else {
				bool res2 = RegisterName(currName, item);
				if (!res2)
					throw NotRegistered(*this, currName);
			}
		}
	}
	ResolvePostRegister(*res.first);
	if (!currName.empty())
		ResolvePostRegister(currName);
	return res;
}
template<class T>
bool SharedRegistrarRoot<T>::RegisterName( std::string_view name, ptr_type item ) {
	if (map.contains(name))
		return false;
	map.insert_or_assign(name, item);
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::RegisterName( std::string&& name, ptr_type item ) {
	if (map.contains(name) || !Contains(item))
		return false;
	auto res = nameSet.insert(name);
	assert(res.second);
	map.insert_or_assign(*res.first, item);
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::Erase( value_type& item ) {
	if (!Contains(item))
		return false;
	auto ptr = operator[](item);
	auto res = set.erase(ptr);
	assert(res > 0);
	for (auto&[key, value]: map)
		if (value == ptr) {
			map.erase(key);
			break;
		}
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::Erase( value_type* const item ) {
	if (!Contains(item))
		return false;
	auto ptr = operator[](item);
	auto res = set.erase(ptr);
	assert(res > 0);
	for (auto&[key, value]: map)
		if (value == ptr) {
			map.erase(key);
			break;
		}
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::Erase( std::string_view item ) {
	if (!map.contains(item))
		return false;
	assert(set.contains(map[item]));
	set.erase(map[item]);
	map.erase(item);
	return true;
}
template<class T>
bool SharedRegistrarRoot<T>::Erase( SharedRegistrarRoot::ptr_type item ) {
	auto iter = set.find(item);
	if (iter == set.end())
		return false;
	set.erase(iter);
	for (auto&[key, value]: map)
		if (value == item) {
			map.erase(key);
			break;
		}
	return true;
}
// endregion
// region AwaitInterface
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
void SharedRegistrarRoot<T>::AwaitFor( const Key& key, std::coroutine_handle<> handle ) {
	static Queue<Key> queue(*this);
	if constexpr (std::copy_constructible<Key>)
		queue.map[std::ref(key)].push_back(handle);
	else
		queue.map[key].push_back(handle);
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>) && std::move_constructible<Key>
void SharedRegistrarRoot<T>::AwaitFor( Key&& key, std::coroutine_handle<> handle ) {
	static Queue<Key> queue(*this);
	queue.map[std::forward<Key>(key)].push_back(handle);
}
template<class T>
void SharedRegistrarRoot<T>::ResolvePostRegister( ptr_type item ) {
	if (!Contains(item))
		return;
	for (auto& queue: queues) {
		queue->Resolve(item);
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
typename SharedRegistrarRoot<T>::template task_type<Key>
SharedRegistrarRoot<T>::AwaitGet( const Key& key, value_type*& value ) {
	value = co_await Awaiter();
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>) && std::move_constructible<Key>
typename SharedRegistrarRoot<T>::template task_type<Key>
SharedRegistrarRoot<T>::AwaitGet( Key&& key, value_type*& value ) {
	value = co_await Awaiter();
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
typename SharedRegistrarRoot<T>::template task_type<Key>
SharedRegistrarRoot<T>::AwaitGet( const Key& key, ptr_type& value ) {
	value = co_await Awaiter();
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>) && std::move_constructible<Key>
typename SharedRegistrarRoot<T>::template task_type<Key>
SharedRegistrarRoot<T>::AwaitGet( Key&& key, ptr_type& value ) {
	value = co_await Awaiter();
}
template<class T>
IRegistrationTask SharedRegistrarRoot<T>::AwaitGet( std::string_view name, ptr_type& value ) {
	std::string nameCopy{name};
	value = co_await RegistrarAwaitGet(*this);
	if (value == nullptr)
		throw NotRegistered(*this, nameCopy);
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
void SharedRegistrarRoot<T>::Get( const Key& key, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(key, value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = operator[](key);
	}
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>) && std::move_constructible<Key>
void SharedRegistrarRoot<T>::Get( Key&& key, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(std::forward<Key>(key), value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = operator[](key);
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>
void SharedRegistrarRoot<T>::Get( const Key& key, ptr_type& value, bool await ) {
	if (await) {
		AwaitGet(key, value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = operator[](key);
	}
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, spT<T>> || TransparentLessKey<Key, spT<T>>) && std::move_constructible<Key>
void SharedRegistrarRoot<T>::Get( Key&& key, ptr_type& value, bool await ) {
	if (await) {
		AwaitGet(std::forward<Key>(key), value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = operator[](key);
	}
}
template<class T>
void SharedRegistrarRoot<T>::Get( std::string_view name, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		value = map[name];
	}
}
template<class T>
void SharedRegistrarRoot<T>::Get( std::string_view name, std::vector<value_type*>& vector, bool await ) {
	if (await) {
		auto& value = vector.emplace_back();
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		vector.push_back(map[name]);
	}
}
template<class T>
void SharedRegistrarRoot<T>::Get( std::string_view name, ptr_type& value, bool await ) {
	if (await) {
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		value = map[name];
	}
}
template<class T>
void SharedRegistrarRoot<T>::Get( std::string_view name, std::vector<ptr_type>& vector, bool await ) {
	if (await) {
		auto& value = vector.emplace_back();
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		vector.push_back(map[name]);
	}
}
// endregion
// endregion

// region RefRegistrar Interface
template<class T, class U>
std::pair<typename SharedRegistrarRoot<T>::set_type::iterator, bool>
SharedRegistrar<T, U>::Register( ptr_type item ) {
	auto res = base::Register(item);
	if (!res.second)
		return res;
	auto res2 = Root.Register(item);
	if (!res2.second) {
		throw NotRegistered(*this, "[No name calculated]");
	}
	return res;
}
template<class T, class U>
bool SharedRegistrar<T, U>::RegisterName( std::string_view name, ptr_type item ) {
	bool res = base::RegisterName(name, item);
	if (!res)
		return false;
	bool res2 = Root.RegisterName(name, item);
	if (!res2) {
		throw NotRegistered(*this, name);
	}
	return true;
}
template<class T, class U>
bool SharedRegistrar<T, U>::RegisterName( std::string&& name, ptr_type item ) {
	auto nameCopy = std::string(std::string_view(name));
	bool res = base::RegisterName(std::move(name), item);
	if (!res)
		return false;
	auto namePtr = this->nameSet.find(nameCopy);
	assert(namePtr != this->nameSet.end());
	bool res2 = Root.RegisterName(*namePtr, item);
	if (!res2) {
		throw NotRegistered(*this, *namePtr);
	}
	return true;
}
template<class T, class U>
bool SharedRegistrar<T, U>::Erase( value_type& item ) {
	if (!base::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, class U>
bool SharedRegistrar<T, U>::Erase( value_type* const item ) {
	if (!base::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, class U>
bool SharedRegistrar<T, U>::Erase( std::string_view str ) {
	if (!base::Erase(str))
		return false;
	Root.Erase(str);
	return true;
}
template<class T, class U>
void SharedRegistrar<T, U>::ResolvePostRegister( std::string_view str ) {
	base::ResolvePostRegister(str);
	Root.ResolvePostRegister(str);
}
template<class T, class U>
void SharedRegistrar<T, U>::ResolvePostRegister( ptr_type item ) {
	base::ResolvePostRegister(item);
	Root.ResolvePostRegister(item);
}
// endregion
#endif //QUANFLOQ_INCLUDE_REGISTRAR_SHAREDREGISTRAR_TPP
