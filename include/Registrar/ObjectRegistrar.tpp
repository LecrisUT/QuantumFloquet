//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_OBJECTREGISTRAR_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_OBJECTREGISTRAR_TPP

#include "ObjectRegistrar.hpp"
#include <cassert>

using namespace QuanFloq;

// region Additional class definitions
template<class T>
template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
struct ObjectRegistrarRoot<T>::Queue :
		ObjectRegistrarRoot<T>::IQueue, Internal::RegistrarKeyMap<Key> {
	using base = ObjectRegistrarRoot<T>::IQueue;
	using base::base;
	void Resolve( const value_type& item ) override {
		assert(item);
		if constexpr (TransparentEqualKey<Key, T>) {
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
template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
struct ObjectRegistrarRoot<T>::RegistrationTask<Key>::promise_type : Internal::RegistrarKeyHolder<Key> {
	using base = Internal::RegistrarKeyHolder<Key>;
	ObjectRegistrarRoot<T>& registrar;

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
	promise_type( ObjectRegistrarRoot<T>& registrar, const Key& key, Args&& ... ):
			base(key), registrar{registrar} { };
	template<class ...Args>
	requires std::move_constructible<Key>
	promise_type( ObjectRegistrarRoot<T>& registrar, Key&& key, Args&& ... ):
			base(std::move(key)), registrar{registrar} { };
	// Additional overloaders for non-static function calls
	template<class U, class ...Args>
	promise_type( U&&, ObjectRegistrarRoot<T>& registrar, const Key& key, Args&& ... ):
			base(key), registrar{registrar} { };
	template<class U, class ...Args>
	requires std::move_constructible<Key>
	promise_type( U&&, ObjectRegistrarRoot<T>& registrar, Key&& key, Args&& ... ):
			base(std::move(key)), registrar{registrar} { };
	std::coroutine_handle<promise_type> GetHandle() {
		return std::coroutine_handle<promise_type>::from_promise(*this);
	}
};
template<class T>
template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
struct ObjectRegistrarRoot<T>::Awaiter {
	using value_type = typename ObjectRegistrarRoot<T>::value_type;
	using task_type = typename ObjectRegistrarRoot<T>::task_type<Key>;
	using awaiter_type = typename ObjectRegistrarRoot<T>::awaiter_type<Key>;
	using promise_type = typename ObjectRegistrarRoot<T>::promise_type<Key>;
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
	const value_type& await_resume() const {
		// In the end return the requested object to the caller of co_await
		assert(promise->registrar.Contains(promise->keyRef));
		return promise->registrar[promise->keyRef];
	}
};
template<class T>
struct RegistrarAwaitGet<T, ObjectRegistrarRoot> :
		RegistrarAwaitGetBase {
	using value_type = T;
	using registrar_type = ObjectRegistrarRoot<T>;

	ObjectRegistrarRoot<T>& registrar;
	auto await_resume() const {
		assert(registrar.Contains(promise->name));
		return registrar[promise->name];
	}
	explicit RegistrarAwaitGet( ObjectRegistrarRoot<T>& registrar ) :
			registrar{registrar} { };
};
// endregion

// region Interface Functions
// region Getters and Checkers
template<class T>
bool ObjectRegistrarRoot<T>::Contains( std::string_view str ) const {
	return map.contains(str);
}
template<class T>
bool ObjectRegistrarRoot<T>::Contains( const value_type& item ) const {
	return set.contains(item);
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
bool ObjectRegistrarRoot<T>::Contains( const Key& key ) const {
	if constexpr (TransparentLessKey<Key, T>)
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
const IExposable* ObjectRegistrarRoot<T>::GetRef( std::string_view str ) const {
	if constexpr (std::derived_from<T, IExposable>) {
		if (!Contains(str))
			return nullptr;
		return *operator[](str);
	} else {
		throw NotImplemented(*this, "GetRef");
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
const typename ObjectRegistrarRoot<T>::value_type& ObjectRegistrarRoot<T>::operator[]( const Key& key ) const {
	if constexpr (TransparentLessKey<Key, T>) {
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
const typename ObjectRegistrarRoot<T>::value_type& ObjectRegistrarRoot<T>::operator[]( std::string_view str ) const {
	auto key = map.find(str);
	if (key == map.end())
		return nullptr;
	return key->second.get();
}
// endregion
// region Register and Erase
template<class T>
std::pair<typename ObjectRegistrarRoot<T>::set_type::iterator, bool>
ObjectRegistrarRoot<T>::Register( value_type&& item ) {
	if constexpr (NameResolvable<T>) {
		auto res = map.find(Resolver<T>::Name(item));
		if (res != map.end()) {
			auto location = set.find(res->second);
			if (location != set.end())
				return {location, false};
		}
	}
	auto res = set.insert(std::move(item));
	if (!res.second)
		return res;
	std::string_view currName;
	if constexpr (NameResolvable<T>) {
		currName = Resolver<T>::Name(*res.first);
		if (!currName.empty()) {
			if (map.contains(currName)) {
				if (&map.at(currName).get() != &*res.first)
					throw NotRegistered(*this, currName);
			} else {
				bool res2 = RegisterName(currName, *res.first);
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
template<class ...Args>
requires std::constructible_from<T, Args...>
std::pair<typename ObjectRegistrarRoot<T>::set_type::iterator, bool>
ObjectRegistrarRoot<T>::Register( Args&& ...args ) {
	auto res = set.emplace(std::forward<Args>(args)...);
	if (!res.second)
		return res;
	std::string_view currName;
	if constexpr (NameResolvable<T>) {
		currName = Resolver<T>::Name(*res.first);
		if (!currName.empty()) {
			if (map.contains(currName)) {
				if (map.at(currName).get() != *res.first)
					throw NotRegistered(*this, currName);
			} else {
				bool res2 = RegisterName(currName, *res.first);
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
bool ObjectRegistrarRoot<T>::RegisterName( std::string_view name, const value_type& item ) {
	if (map.contains(name))
		return false;
	map.insert_or_assign(name, std::ref(operator[](item)));
	return true;
}
template<class T>
bool ObjectRegistrarRoot<T>::RegisterName( std::string&& name, const value_type& item ) {
	if (map.contains(name))
		return false;
	auto res = nameSet.insert(name);
	assert(res.second);
	map.insert_or_assign(*res.first, std::ref(operator[](item)));
	return true;
}
template<class T>
bool ObjectRegistrarRoot<T>::Erase( const value_type& item ) {
	auto iter = set.find(item);
	if (iter == set.end())
		return false;
	for (auto&[key, value]: map)
		if (&value.get() == &*iter) {
			map.erase(key);
			break;
		}
	set.erase(iter);
	return true;
}
template<class T>
bool ObjectRegistrarRoot<T>::Erase( std::string_view item ) {
	if (!map.contains(item))
		return false;
	assert(set.contains(map.at(item)));
	set.erase(map.at(item));
	map.erase(item);
	return true;
}
// endregion
// region AwaitInterface
template<class T>
template<class Key>
requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
void ObjectRegistrarRoot<T>::AwaitFor( const Key& key, std::coroutine_handle<> handle ) {
	static Queue<Key> queue(*this);
	if constexpr (std::copy_constructible<Key>)
		queue.map[std::ref(key)].push_back(handle);
	else
		queue.map[key].push_back(handle);
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>) && std::move_constructible<Key>
void ObjectRegistrarRoot<T>::AwaitFor( Key&& key, std::coroutine_handle<> handle ) {
	static Queue<Key> queue(*this);
	queue.map[std::forward<Key>(key)].push_back(handle);
}
template<class T>
void ObjectRegistrarRoot<T>::ResolvePostRegister( const value_type& item ) {
	if (!Contains(item))
		return;
	for (auto& queue: queues) {
		queue->Resolve(item);
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
typename ObjectRegistrarRoot<T>::template task_type<Key>
ObjectRegistrarRoot<T>::AwaitGet( const Key& key, value_type*& value ) {
	value = &co_await Awaiter();
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>) && std::move_constructible<Key>
typename ObjectRegistrarRoot<T>::template task_type<Key>
ObjectRegistrarRoot<T>::AwaitGet( Key&& key, value_type*& value ) {
	value = &co_await Awaiter();
}
template<class T>
IRegistrationTask ObjectRegistrarRoot<T>::AwaitGet( std::string_view name, value_type*& value ) {
	std::string nameCopy{name};
	value = co_await RegistrarAwaitGet(*this);
	if (value != nullptr)
		throw NotRegistered(*this, nameCopy);
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
void ObjectRegistrarRoot<T>::Get( const Key& key, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(key, value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = &operator[](key);
	}
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>) && std::move_constructible<Key>
void ObjectRegistrarRoot<T>::Get( Key&& key, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(std::forward<Key>(key), value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = &operator[](key);
	}
}
template<class T>
void ObjectRegistrarRoot<T>::Get( std::string_view name, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		value = map[name];
	}
}
template<class T>
void ObjectRegistrarRoot<T>::Get( std::string_view name, std::vector<value_type*>& vector, bool await ) {
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
bool ObjectRegistrar<T, U>::RegisterName( std::string_view name, const value_type& item ) {
	bool res = RefRegistrarRoot<T>::RegisterName(name, item);
	if (!res)
		return false;
	bool res2 = Root.RegisterName(name, item);
	if (!res2) {
		throw NotRegistered(*this, name);
	}
	return true;
}
template<class T, class U>
bool ObjectRegistrar<T, U>::RegisterName( std::string&& name, const value_type& item ) {
	auto nameCopy = std::string(std::string_view(name));
	bool res = RefRegistrarRoot<T>::RegisterName(std::move(name), item);
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
bool ObjectRegistrar<T, U>::Erase( const value_type& item ) {
	if (!RefRegistrarRoot<T>::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, class U>
bool ObjectRegistrar<T, U>::Erase( std::string_view str ) {
	if (!RefRegistrarRoot<T>::Erase(str))
		return false;
	Root.Erase(str);
	return true;
}
template<class T, class U>
void ObjectRegistrar<T, U>::ResolvePostRegister( std::string_view str ) {
	RefRegistrarRoot<T>::ResolvePostRegister(str);
	Root.ResolvePostRegister(str);
}
template<class T, class U>
void ObjectRegistrar<T, U>::ResolvePostRegister( const value_type& item ) {
	RefRegistrarRoot<T>::ResolvePostRegister(item);
	Root.ResolvePostRegister(item);
}
// endregion
#endif //QUANFLOQ_INCLUDE_REGISTRAR_OBJECTREGISTRAR_TPP
