//
// Created by Le Minh Cristian on 2021/10/29.
//

#ifndef QUANFLOQ_REGISTRARS_HPP
#define QUANFLOQ_REGISTRARS_HPP

#include "UtilityConcepts.hpp"
#include "RegistrarConcepts.hpp"
#include <typeindex>
#include <set>
#include <unordered_set>
#include <memory>
#include <coroutine>
#include <map>
#include <list>

namespace QuanFloq {
	struct IExposable;
	struct RegistrationTask;

	/**
	 * Registrar interface
	 */
	struct IRegistrar {
		// Need to call RegistrationQueue.resume() on all Registrars after static initialization step and other steps
		inline static std::set<IRegistrar*> registrars;
		// Unfortunately std::set does not have constexpr constructor
//		constinit static std::set<IRegistrar*> registrars;
		static void ResolveDanglingRegisters();

		std::map<std::string, std::list<std::coroutine_handle<>>, std::less<>> registrationQueue;

		IRegistrar() noexcept;
		virtual ~IRegistrar();

		virtual bool Contains( std::string_view str ) const = 0;
		virtual const void* VoidRef( std::string_view str ) const = 0;
		virtual std::shared_ptr<void> VoidPtr( std::string_view str ) const;
		virtual bool TryRegister( const std::shared_ptr<IExposable>& ptr );
		virtual bool Erase( std::string_view item ) = 0;
		// Note: Cannot use unordered_map because rehashing before the name is changed does not change the ordering
		virtual void RepairOrder() = 0;

		virtual void ResolvePostRegister( std::string_view str );
		RegistrationTask AwaitGet( std::string_view name, const void*& value );
		RegistrationTask AwaitGet( std::string_view name, std::shared_ptr<void>& value );
		void Get( std::string_view name, const void*& value, bool await = true );
		void Get( std::string_view name, std::vector<const void*>& value, bool await = true );
		void Get( std::string_view name, std::shared_ptr<void>& value, bool await = true );
		void Get( std::string_view name, std::vector<std::shared_ptr<void>>& value, bool await = true );
	};
	// Cannot use concept requirements because it is used with incomplete types. Use static_assert instead.
	/**
	 * Basic Registrar template for searching non-owning objects
	 * @tparam T
	 */
	template<class T>
	struct RefRegistrarRoot :
			IRegistrar {
		using value_type = T;
		// TODO: Switch to unordered_set when can get bucket from equivalent key
		using set_type = std::set<std::reference_wrapper<const T>>;

		set_type Set;

		RefRegistrarRoot() noexcept;
		bool Contains( std::string_view str ) const override;
		const void* VoidRef( std::string_view str ) const override;
		bool TryRegister( const std::shared_ptr<IExposable>& ptr ) override;
		virtual bool Register( T& item );
		virtual bool Erase( T& item );
		bool Erase( std::string_view item ) override;
		void RepairOrder() override;

		RegistrationTask AwaitGet( std::string_view name, T*& value );
		void Get( std::string_view name, const T*& value, bool await = true );
		void Get( std::string_view name, std::vector<const T*>& value, bool await = true );

		const T* operator[]( std::string_view str ) const;
	};
	/**
	 * Basic Registrar owning object instances
	 * @tparam T
	 */
	template<class T>
	struct SharedRegistrarRoot :
			RefRegistrarRoot<T> {
		using value_type = T;
		using set_type = std::set<std::shared_ptr<T>>;
		using IRegistrar::registrationQueue;
		using IRegistrar::ResolvePostRegister;

		set_type Set;

		bool Contains( std::string_view str ) const override;
		std::shared_ptr<void> VoidPtr( std::string_view str ) const override;
		bool TryRegister( const std::shared_ptr<IExposable>& ptr ) override;
		virtual bool Register( std::shared_ptr<T> item );
		virtual bool Erase( std::shared_ptr<T> item );
		bool Erase( std::string_view item ) override;
		void RepairOrder() override;

		RegistrationTask AwaitGet( std::string_view name, std::shared_ptr<T>& value );
		void Get( std::string_view name, std::shared_ptr<T>& value, bool await = true );
		void Get( std::string_view name, std::vector<std::shared_ptr<T>>& value, bool await = true );

		std::shared_ptr<T> operator[]( std::string_view str ) const;
	};

	template<class T>
	struct ObjectRegistrar :
			IRegistrar {
		using value_type = T;
		using set_type = std::set<T>;

		set_type Set;

		ObjectRegistrar() noexcept;
		bool Contains( std::string_view str ) const override;
		const void* VoidRef( std::string_view str ) const override;
		template<class ...Args>
		requires ctrArgs<T, Args...>
		std::pair<typename std::set<T>::iterator, bool> Register( Args&& ... );
		virtual bool Erase( T& item );
		bool Erase( std::string_view item ) override;
		void RepairOrder() override;

		RegistrationTask AwaitGet( std::string_view name, const T*& value );
		void Get( std::string_view name, const T*& value, bool await = true );
		void Get( std::string_view name, std::vector<const T*>& value, bool await = true );
		const T* operator[]( std::string_view str ) const;
	};
	// Can use concept requirement for the baseclass because it's already implemented.
	/**
	 * Subregistrar containing derived classes for faster searching.
	 * Registered reference are saved in root registrar as well.
	 * @tparam T Derived saved class
	 * @tparam U Base class
	 */
	template<class T, strComparable U>
	struct RefRegistrar :
			RefRegistrarRoot<T> {
		using base_type = U;

		RefRegistrarRoot<U>& Root;

		RefRegistrar() noexcept requires stdRegistrar<U>;
		explicit RefRegistrar( RefRegistrarRoot<U>& root ) noexcept;

		bool Register( T& item ) override;
		bool Erase( T& item ) override;
		bool Erase( std::string_view item ) override;
		void RepairOrder() override;
		void ResolvePostRegister( std::string_view str ) override;
	};
	/**
	 * Subregistrar containing derived classes for faster searching.
	 * Registered objects are saved in root registrar as well.
	 * @tparam T Derived saved class
	 * @tparam U Base class
	 */
	template<class T, strComparable U>
	struct SharedRegistrar :
			SharedRegistrarRoot<T> {
		using base_type = U;

		SharedRegistrarRoot<U>& Root;

		SharedRegistrar() noexcept requires stdRegistrar<U>;
		explicit SharedRegistrar( SharedRegistrarRoot<U>& root ) noexcept;

		bool Register( std::shared_ptr<T> item ) override;
		bool Erase( std::shared_ptr<T> item ) override;
		bool Erase( std::string_view item ) override;
		void RepairOrder() override;
		void ResolvePostRegister( std::string_view str ) override;
	};

	template<class T, template<class> class TRegistrar> requires std::derived_from<TRegistrar<T>, IRegistrar>
	struct RegistrarAwaitGet;
	struct IRegistrarAwaitGetRef;
	struct IRegistrarAwaitGetPtr;
	struct RegistrarAwaitGetBase;
	/**
	 * Co_await return type for registrar operations
	 */
	struct RegistrationTask {
		struct promise_type {
			IRegistrar& registrar;
			std::string name;

			RegistrationTask get_return_object();
			std::suspend_never initial_suspend();
			std::suspend_never final_suspend() noexcept;
			void return_void();
			void unhandled_exception();
			template<class T, template<class> class TRegistrar>
			requires std::derived_from<TRegistrar<T>, IRegistrar>
			RegistrarAwaitGet<T, TRegistrar> await_transform( RegistrarAwaitGet<T, TRegistrar>&& awaiter );
			IRegistrarAwaitGetRef await_transform( IRegistrarAwaitGetRef&& awaiter );
			IRegistrarAwaitGetPtr await_transform( IRegistrarAwaitGetPtr&& awaiter );
			// Specific overload constructors to ensure it is called by AwaitGet or similar syntax functions
			promise_type( IRegistrar& registrar, std::string_view name, const void*& );
			promise_type( IRegistrar& registrar, std::string_view name, std::shared_ptr<void>& );
			template<class T, class TRegistrar>
			promise_type( TRegistrar& registrar, std::string_view name, const T*& );
			template<class T, class TRegistrar>
			promise_type( TRegistrar& registrar, std::string_view name, std::shared_ptr<T>& );
			std::coroutine_handle<promise_type> GetHandle();
		};
		std::coroutine_handle<promise_type> handle;
		operator std::coroutine_handle<promise_type>() const;
		operator std::coroutine_handle<>() const;
	};
	using RegistrationPromise = RegistrationTask::promise_type;
	using RegistrationHandle = std::coroutine_handle<RegistrationPromise>;
	struct RegistrarAwaitGetBase {
		RegistrationPromise* promise;
		bool await_ready() const;
		bool await_suspend( std::coroutine_handle<RegistrationTask::promise_type> h ) const;
	protected:
		RegistrarAwaitGetBase() = default;
	};
	/**
	 * Helper struct to co_await registrar.get
	 * @tparam TRegistrar
	 * @tparam T
	 */
	template<class T, template<class> class TRegistrar = SharedRegistrarRoot> requires std::derived_from<TRegistrar<T>, IRegistrar>
	struct RegistrarAwaitGet :
			RegistrarAwaitGetBase {
		using value_type = T;
		using registrar_type = TRegistrar<T>;

		TRegistrar<T>& registrar;
		auto await_resume() const;
		explicit RegistrarAwaitGet( TRegistrar<T>& registrar );
		static_assert(std::same_as<T, T>,
		              "Specialization required for each registrar type in order to get a useful return type");
	};
	struct IRegistrarAwaitGetRef :
			RegistrarAwaitGetBase {
		const void* await_resume() const;
	};
	struct IRegistrarAwaitGetPtr :
			RegistrarAwaitGetBase {
		std::shared_ptr<void> await_resume() const;
	};
}

#endif //QUANFLOQ_REGISTRARS_HPP
