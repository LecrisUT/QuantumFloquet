//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_REFREGISTRAR_TPP
#define QUANFLOQ_INCLUDE_REGISTRAR_REFREGISTRAR_TPP

#include "RefRegistrar.hpp"
#include <cassert>

using namespace QuanFloq;

// region Additional class definitions
template<class T>
using rwT = std::reference_wrapper<T>;
template<class T>
template<class Key> requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
struct RefRegistrarRoot<T>::Queue :
		RefRegistrarRoot<T>::IQueue, Internal::RegistrarKeyMap<Key> {
	using base = RefRegistrarRoot<T>::IQueue;
	using typename base::value_type;
	using base::base;
	void Resolve( const value_type& item ) override {
		if constexpr (TransparentEqualKey<Key, rwT<T>>) {
			auto& operand = this->registrar.operators.equal;
			auto mapCopy = map;
			for (auto&[key, value]: mapCopy)
				if (operand(key, item)) {
					for (auto& handle: value)
						handle.resume();
					if (value.empty())
						map.erase(key);
				}
		} else {
			auto& operand = this->registrar.operators.less;
			auto mapCopy = map;
			for (auto&[key, value]: mapCopy)
				if (!(operand(key, item) || operand(item, key))) {
					for (auto& handle: value)
						handle.resume();
					if (value.empty())
						map.erase(key);
				}
		}
	}
};
template<class T>
template<class Key> requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
struct RefRegistrarRoot<T>::RegistrationTask<Key>::promise_type {
	task_type get_return_object() {
		return {.handle = GetHandle()};
	}
	std::suspend_never initial_suspend() { return {}; }
	std::suspend_never final_suspend() noexcept { return {}; }
	void return_void() { }
	void unhandled_exception() { }
	awaiter_type await_transform( awaiter_type&& awaiter ) {
		auto output = std::move(awaiter);
		output.promise = this;
		return output;
	}
	std::coroutine_handle<promise_type> GetHandle() {
		return std::coroutine_handle<promise_type>::from_promise(*this);
	}
};
template<class T>
template<class Key> requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
struct RefRegistrarRoot<T>::Awaiter : Internal::RegistrarKeyHolder<Key> {
	using base = Internal::RegistrarKeyHolder<Key>;
	using value_type = typename SharedRegistrarRoot<T>::value_type;
	using task_type = typename RefRegistrarRoot<T>::task_type<Key>;
	using awaiter_type = typename RefRegistrarRoot<T>::awaiter_type<Key>;
	using promise_type = typename RefRegistrarRoot<T>::promise_type<Key>;
	RefRegistrarRoot<T>& registrar;
	promise_type* promise;
	Awaiter( RefRegistrarRoot<T>& registrar, const Key& key ) : base(key), registrar{registrar} { }
	Awaiter( RefRegistrarRoot<T>& registrar, Key&& key ) : base(std::forward<Key>(key)), registrar{registrar} { }
	bool await_ready() const {
		if (registrar.Contains(this->keyRef))
			return true;
		registrar.AwaitFor(this->keyRef, promise->GetHandle());
		return false;
	}
	bool await_suspend( std::coroutine_handle<IRegistrationTask::promise_type> h ) const {
		// If Registrar does not contain the required object, continue to co_await
		return !registrar.Contains(this->keyRef);
	}
	auto await_resume() const {
		// In the end return the requested object to the caller of co_await
		assert(registrar.Contains(this->keyRef));
		return registrar[this->keyRef];
	}
};
template<class T>
struct RegistrarAwaitGet<T, RefRegistrarRoot> :
		RegistrarAwaitGetBase {
	using value_type = T;
	using registrar_type = RefRegistrarRoot<T>;

	RefRegistrarRoot<T>& registrar;
	auto await_resume() const {
		assert(registrar.Contains(promise->name));
		return registrar[promise->name];
	}
	explicit RegistrarAwaitGet( RefRegistrarRoot<T>& registrar ) :
			registrar{registrar} { };
};
// endregion

// region Interface Functions
// region Getters and Checkers
template<class T>
bool RefRegistrarRoot<T>::Contains( std::string_view str ) const {
	return map.contains(str);
}
template<class T>
bool RefRegistrarRoot<T>::Contains( value_type& item ) const {
	return set.contains(&item);
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
bool RefRegistrarRoot<T>::Contains( const Key& key ) const {
	if constexpr (TransparentLessKey<Key, rwT<T>>)
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
const IExposable* RefRegistrarRoot<T>::GetRef( std::string_view str ) const {
	if constexpr (std::derived_from<T, IExposable>) {
		if (!Contains(str))
			return nullptr;
		return &operator[](str);
	} else {
		throw NotImplemented(*this, "GetRef");
	}
}
template<class T>
typename RefRegistrarRoot<T>::value_type& RefRegistrarRoot<T>::operator[]( std::string_view str ) const {
	auto key = map.find(str);
	if (key == map.end())
		throw NotRegistered(*this, str);
	return key->second.get();
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
typename RefRegistrarRoot<T>::value_type& RefRegistrarRoot<T>::operator[]( const Key& key ) const {
	if constexpr (TransparentLessKey<Key, rwT<T>>) {
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
// endregion
// region Register and Erase
template<class T>
std::pair<typename RefRegistrarRoot<T>::set_type::iterator, bool>
RefRegistrarRoot<T>::Register( value_type& item ) {
	if constexpr (NameResolvable<T>) {
		auto res = map.find(Resolver<T>::Name(item));
		if (res != map.end()) {
			auto location = set.find(&res->second.get());
			if (location != set.end())
				// If it is the same item, consider successful, otherwise a conflict
				return {location, &location->get() == &item};
		}
	}
	auto res = set.insert(item);
	if (!res.second)
		return res;
	std::string_view currName;
	if constexpr (NameResolvable<T>) {
		currName = Resolver<T>::Name(res.first->get());
		if (!currName.empty()) {
			if (map.contains(currName)) {
				if (&map.at(currName).get() != &res.first->get())
					throw NotRegistered(*this, currName);
			} else {
				bool res2 = RegisterName(currName, *res.first);
				if (!res2)
					throw NotRegistered(*this, currName);
			}
		}
	}
	ResolvePostRegister(res.first->get());
	if (!currName.empty())
		ResolvePostRegister(currName);
	return res;
}
template<class T>
bool RefRegistrarRoot<T>::RegisterName( std::string_view name, value_type& item ) {
	// TODO: Need to check that the item is included in the set if called outside of Register
	if (map.contains(name))
		return false;
	map.insert_or_assign(name, std::ref(item));
	for (auto iter = map.begin(); iter != map.end();) {
		if (&iter->second.get() == &item && iter->first != name) {
			auto nameIter = nameSet.find(iter->first);
			if (nameIter != nameSet.end())
				nameSet.erase(nameIter);
			iter = map.erase(iter);
		} else
			iter++;
	}
	return true;
}
template<class T>
bool RefRegistrarRoot<T>::RegisterName( std::string&& name, value_type& item ) {
	if (map.contains(name))
		return false;
	auto res = nameSet.insert(name);
	assert(res.second);
	return RegisterName(*res.first, item);
}
template<class T>
bool RefRegistrarRoot<T>::Erase( value_type& item ) {
	auto iter = set.find(&item);
	if (iter == set.end())
		return false;
	for (auto&[key, value]: map)
		if (value == *iter) {
			map.erase(key);
			break;
		}
	set.erase(iter);
	return true;
}
template<class T>
bool RefRegistrarRoot<T>::Erase( std::string_view item ) {
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
requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
void RefRegistrarRoot<T>::AwaitFor( const Key& key, std::coroutine_handle<> handle ) {
	static Queue<Key> queue(*this);
	if constexpr (std::copy_constructible<Key>)
		queue.map[std::ref(key)].push_back(handle);
	else
		queue.map[key].push_back(handle);
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>) && std::move_constructible<Key>
void RefRegistrarRoot<T>::AwaitFor( Key&& key, std::coroutine_handle<> handle ) {
	static Queue<Key> queue(*this);
	queue.map[std::forward<Key>(key)].push_back(handle);
}
template<class T>
void RefRegistrarRoot<T>::ResolvePostRegister( value_type& item ) {
	if (!Contains(item))
		return;
	for (auto& queue: queues) {
		queue->Resolve(item);
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
typename RefRegistrarRoot<T>::template task_type<Key>
RefRegistrarRoot<T>::AwaitGet( const Key& key, value_type*& value ) {
	value = &co_await Awaiter(*this, key);
}
template<class T>
template<class Key>
requires (TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>) && std::move_constructible<Key>
typename RefRegistrarRoot<T>::template task_type<Key>
RefRegistrarRoot<T>::AwaitGet( Key&& key, value_type*& value ) {
	value = &co_await Awaiter(*this, std::forward<Key>(key));
}
template<class T>
IRegistrationTask RefRegistrarRoot<T>::AwaitGet( std::string_view name, value_type*& value ) {
	std::string nameCopy{name};
	value = co_await RegistrarAwaitGet(*this);
	if (value != nullptr)
		throw NotRegistered(*this, nameCopy);
}
template<class T>
void RefRegistrarRoot<T>::Get( std::string_view name, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		value = &map[name];
	}
}
template<class T>
template<class Key>
requires TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>
void RefRegistrarRoot<T>::Get( const Key& key, value_type*& value, bool await ) {
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
requires (TransparentEqualKey<Key, rwT<T>> || TransparentLessKey<Key, rwT<T>>) && std::move_constructible<Key>
void RefRegistrarRoot<T>::Get( Key&& key, value_type*& value, bool await ) {
	if (await) {
		AwaitGet(std::forward<Key>(key), value);
	} else {
		if (!Contains(key))
			throw NotRegistered(*this, "[No name calculated]");
		value = &operator[](key);
	}
}
template<class T>
void RefRegistrarRoot<T>::Get( std::string_view name, std::vector<value_type*>& vector, bool await ) {
	if (await) {
		auto& value = vector.emplace_back();
		AwaitGet(name, value);
	} else {
		if (!Contains(name))
			throw NotRegistered(*this, name);
		vector.push_back(&map[name]);
	}
}
// endregion
// endregion

// region RefRegistrar Interface
template<class T, class U>
std::pair<typename RefRegistrarRoot<T>::set_type::iterator, bool>
RefRegistrar<T, U>::Register( value_type& item ) {
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
bool RefRegistrar<T, U>::RegisterName( std::string_view name, value_type& item ) {
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
bool RefRegistrar<T, U>::Erase( value_type& item ) {
	if (!base::Erase(item))
		return false;
	Root.Erase(item);
	return true;
}
template<class T, class U>
bool RefRegistrar<T, U>::Erase( std::string_view str ) {
	if (!base::Erase(str))
		return false;
	Root.Erase(str);
	return true;
}
template<class T, class U>
void RefRegistrar<T, U>::ResolvePostRegister( std::string_view str ) {
	base::ResolvePostRegister(str);
	Root.ResolvePostRegister(str);
}
template<class T, class U>
void RefRegistrar<T, U>::ResolvePostRegister( value_type& item ) {
	base::ResolvePostRegister(item);
	Root.ResolvePostRegister(item);
}
// endregion
#endif //QUANFLOQ_INCLUDE_REGISTRAR_REFREGISTRAR_TPP
