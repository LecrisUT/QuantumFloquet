//
// Created by lecris on 2021-11-29.
//

#include "Registrar/IRegistrar.hpp"

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
			if constexpr(std::same_as<U, const IExposable*>)
				value = registrar.GetRef(name);
			else if constexpr (std::same_as<U, std::shared_ptr<IExposable>>)
				value = registrar.GetPtr(name);
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
			if constexpr(std::same_as<U, const IExposable*>)
				vector.push_back(registrar.GetRef(name));
			else if constexpr (std::same_as<U, std::shared_ptr<IExposable>>)
				vector.push_back(registrar.GetPtr(name));
			else
					static_assert(!std::same_as<U, U>,
					              "Other implementation not supported");
		}
	}
}
// endregion
// region Exceptions
RegistrarError::RegistrarError( const IRegistrar& registrar ) : registrar{registrar} {

}
NotRegistered::NotRegistered( const IRegistrar& registrar, std::string_view item ) :
		RegistrarError(registrar), out_of_range(message) {
	message = "Item [" + std::string(item) + "] not registered in " + typeid(registrar).name();
}
NotImplemented::NotImplemented( const IRegistrar& registrar, std::string_view operation ) :
		RegistrarError(registrar) {
	message = "Operation [" + std::string(operation) + "] not implemented in " + typeid(registrar).name();
}
const char* NotImplemented::what() const noexcept {
	return message.c_str();
}
const char* NotRegistered::what() const noexcept {
	return message.c_str();
}

// region Static function definitions
void IRegistrar::ResolveDanglingRegisters() {
	for (auto& registrar: registrars)
		for (auto& items: registrar->registrationQueue) {
			if (!registrar->Contains(items.first))
				continue;
			for (auto& handle: items.second)
				handle.resume();
		}
}
// endregion
// endregion

// region Constructor/Destructor
IRegistrar::~IRegistrar() {
	registrars.remove(this);
}
IRegistrar::IRegistrar() noexcept {
	// Initialization fiasco is avoided using inline static initializers
	registrars.push_back(this);
}
// endregion

// region Interfaces
std::shared_ptr<IExposable> IRegistrar::GetPtr( std::string_view str ) const {
	throw NotImplemented(*this, "GetPtr");
}
bool IRegistrar::TryRegister( std::shared_ptr<IExposable> ptr ) {
	throw NotImplemented(*this, "TryRegister");
}
// endregion

// region co_await functions
IRegistrarAwaitGetRef IRegistrationTask::promise_type::await_transform( IRegistrarAwaitGetRef&& awaiter ) {
	auto output = awaiter;
	output.promise = this;
	return output;
}
IRegistrarAwaitGetPtr IRegistrationTask::promise_type::await_transform( IRegistrarAwaitGetPtr&& awaiter ) {
	auto output = awaiter;
	output.promise = this;
	return output;
}

void IRegistrar::Get( std::string_view name, const IExposable*& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
void IRegistrar::Get( std::string_view name, std::vector<const IExposable*>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
void IRegistrar::Get( std::string_view name, std::shared_ptr<IExposable>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
void IRegistrar::Get( std::string_view name, std::vector<std::shared_ptr<IExposable>>& value, bool await ) {
	AwaitGetHelper(*this, name, value, await);
}
IRegistrationTask IRegistrar::AwaitGet( std::string_view, const IExposable*& value ) {
	value = co_await IRegistrarAwaitGetRef();
	assert(value != nullptr);
}
IRegistrationTask IRegistrar::AwaitGet( std::string_view, std::shared_ptr<IExposable>& value ) {
	value = co_await IRegistrarAwaitGetPtr();
	assert(value != nullptr);
}
void IRegistrar::ResolvePostRegister( std::string_view str ) {
	auto queue = registrationQueue.find(str);
	if (queue == registrationQueue.end())
		// No queue was formed
		return;
	// Calling handle.resume() does not call awaiter.await_suspend.
	// Making sure the item is registered
	if (!Contains(str))
		return;
	// Resume registration of all queuing items
	// Create a queue copy because handles can get automatically deleted
	auto queueCopy = queue->second;
	for (auto& handle: queueCopy)
		handle.resume();
}
// endregion

// region co_await helper class
std::suspend_never IRegistrationTask::promise_type::initial_suspend() {
	return {};
}
std::suspend_never IRegistrationTask::promise_type::final_suspend() noexcept {
	auto queue = registrar.registrationQueue.find(name);
	if (queue != registrar.registrationQueue.end()) {
		queue->second.remove(GetHandle());
	}
	return {};
}
void IRegistrationTask::promise_type::return_void() { }
void IRegistrationTask::promise_type::unhandled_exception() { }
IRegistrationTask::operator std::coroutine_handle<promise_type>() const { return handle; }
IRegistrationTask::operator std::coroutine_handle<>() const { return handle; }
IRegistrationTask IRegistrationTask::promise_type::get_return_object() {
	return {
			.handle = GetHandle()
	};
}
RegistrationHandle IRegistrationTask::promise_type::GetHandle() {
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
const IExposable* IRegistrarAwaitGetRef::await_resume() const {
	// In the end return the requested object to the caller of co_await
	assert(promise->registrar.Contains(promise->name));
	return promise->registrar.GetRef(promise->name);
}
std::shared_ptr<IExposable> IRegistrarAwaitGetPtr::await_resume() const {
	// In the end return the requested object to the caller of co_await
	assert(promise->registrar.Contains(promise->name));
	return promise->registrar.GetPtr(promise->name);
}
// endregion