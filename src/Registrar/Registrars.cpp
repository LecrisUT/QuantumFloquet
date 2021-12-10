//
// Created by lecris on 2021-11-29.
//

#include "Registrar/Registrars.hpp"

#include <cassert>

using namespace QuanFloq;

// region Helper functions
namespace QuanFloq {
	template<class U>
	inline void AwaitGetHelper( IRegistrar& registrar, std::string_view name, U& value, bool await ) {
		if (await) {
			registrar.AwaitGet(name, value);
		} else {
			assert(registrar.Contains(name));
			if constexpr(std::same_as<U, const void*>)
				value = registrar.VoidRef(name);
			else if constexpr (std::same_as<U, std::shared_ptr<void>>)
				value = registrar.VoidPtr(name);
			else
					static_assert(!std::same_as<U, U>,
					              "Other implementation not supported");
		}
	}
	template<class U>
	inline void AwaitGetHelper( IRegistrar& registrar, std::string_view name, std::vector<U>& vector, bool await ) {
		if (await) {
			auto& value = vector.emplace_back();
			registrar.AwaitGet(name, value);
		} else {
			assert(registrar.Contains(name));
			if constexpr(std::same_as<U, const void*>)
				vector.push_back(registrar.VoidRef(name));
			else if constexpr (std::same_as<U, std::shared_ptr<void>>)
				vector.push_back(registrar.VoidPtr(name));
			else
					static_assert(!std::same_as<U, U>,
					              "Other implementation not supported");
		}
	}
}
// endregion

// region Static function definitions
void IRegistrar::ResolveDanglingRegisters() {
	for (auto& registrar: registrars)
		for (auto& items: registrar->registrationQueue)
			for (auto& handle: items.second)
				handle.resume();
}
// endregion

// region Constructor/Destructor
IRegistrar::~IRegistrar() {
	registrars.erase(this);
}
IRegistrar::IRegistrar() noexcept {
	// Initialization fiasco is avoided by the order in this file.
	registrars.insert(this);
}
RegistrationTask::promise_type::promise_type( IRegistrar& registrar,
                                              std::string_view name, const void*& ) :
		registrar{registrar}, name{name} { }
RegistrationTask::promise_type::promise_type( IRegistrar& registrar,
                                              std::string_view name, std::shared_ptr<void>& ) :
		registrar{registrar}, name{name} { }
// endregion

// region Interfaces
std::shared_ptr<void> IRegistrar::VoidPtr( std::string_view str ) const { return nullptr; }
bool IRegistrar::TryRegister( const std::shared_ptr<IExposable>& ptr ) { return false; }
// endregion

// region co_await functions
IRegistrarAwaitGetRef RegistrationTask::promise_type::await_transform( IRegistrarAwaitGetRef&& awaiter ) {
	auto output = awaiter;
	output.promise = this;
	return output;
}
IRegistrarAwaitGetPtr RegistrationTask::promise_type::await_transform( IRegistrarAwaitGetPtr&& awaiter ) {
	auto output = awaiter;
	output.promise = this;
	return output;
}

void IRegistrar::Get( std::string_view name, const void*& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
void IRegistrar::Get( std::string_view name, std::vector<const void*>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
void IRegistrar::Get( std::string_view name, std::shared_ptr<void>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
void IRegistrar::Get( std::string_view name, std::vector<std::shared_ptr<void>>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
RegistrationTask IRegistrar::AwaitGet( std::string_view, const void*& value ) {
	value = co_await IRegistrarAwaitGetRef();
	assert(value != nullptr);
}
RegistrationTask IRegistrar::AwaitGet( std::string_view, std::shared_ptr<void>& value ) {
	value = co_await IRegistrarAwaitGetPtr();
	assert(value != nullptr);
}
void IRegistrar::ResolvePostRegister( std::string_view str ) {
	auto queue = registrationQueue.find(str);
	if (queue == registrationQueue.end())
		// No queue was formed
		return;
	// Need to rehash to correctly check contains
	RepairOrder();
	// Resume registration of all queuing items
	// Create a queue copy because handles can get automatically deleted
	auto queueCopy = queue->second;
	for (auto& handle: queueCopy)
		handle.resume();
}
// endregion

// region co_await helper class
std::suspend_never RegistrationTask::promise_type::initial_suspend() {
	return {};
}
std::suspend_never RegistrationTask::promise_type::final_suspend() noexcept {
	auto queue = registrar.registrationQueue.find(name);
	if (queue != registrar.registrationQueue.end()) {
		queue->second.remove(GetHandle());
	}
	return {};
}
void RegistrationTask::promise_type::return_void() { }
void RegistrationTask::promise_type::unhandled_exception() { }
RegistrationTask::operator std::coroutine_handle<promise_type>() const { return handle; }
RegistrationTask::operator std::coroutine_handle<>() const { return handle; }
RegistrationTask RegistrationTask::promise_type::get_return_object() {
	return {
			.handle = GetHandle()
	};
}
RegistrationHandle RegistrationTask::promise_type::GetHandle() {
	return RegistrationHandle::from_promise(*this);
}

bool RegistrarAwaitGetBase::await_ready() const {
	// If object is already registered, no need to co_await
	if (promise->registrar.Contains(promise->name))
		return true;
	promise->registrar.registrationQueue[promise->name].push_back(promise->GetHandle());
	return false;
}
bool RegistrarAwaitGetBase::await_suspend( RegistrationHandle h ) const {
	// TODO: Add DebugLog for when function is called, but object is not ready
//	assert(promise->registrar.Contains(promise->name));
	// If Registrar does not contain the required object, continue to co_await
	return !promise->registrar.Contains(promise->name);
}
const void* IRegistrarAwaitGetRef::await_resume() const {
	// In the end return the requested object to the caller of co_await
	assert(promise->registrar.Contains(promise->name));
	return promise->registrar.VoidRef(promise->name);
}
std::shared_ptr<void> IRegistrarAwaitGetPtr::await_resume() const {
	// In the end return the requested object to the caller of co_await
	assert(promise->registrar.Contains(promise->name));
	return promise->registrar.VoidPtr(promise->name);
}
// endregion