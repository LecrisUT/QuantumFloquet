//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_IREGISTRAR_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_IREGISTRAR_HPP

#include <exception>
#include <map>
#include <list>
#include <coroutine>
#include <vector>
#include <string>
#include <memory>

namespace QuanFloq {
	struct IExposable;
	struct IRegistrationTask;
	struct IRegistrar;

	struct RegistrarError {
		const IRegistrar& registrar;
		RegistrarError( const IRegistrar& registrar );
	};
	struct NotRegistered : RegistrarError, std::out_of_range {
		std::string_view item;
		std::string message;
		NotRegistered( const IRegistrar& registrar, std::string_view item );
		const char* what() const noexcept override;
	};
	struct NotImplemented : RegistrarError, std::exception {
		std::string_view operation;
		std::string message;
		NotImplemented( const IRegistrar& registrar, std::string_view operation );
		const char* what() const noexcept override;
	};


	/**
	 * Registrar interface
	 */
	struct IRegistrar {
		using queue_type = std::map<std::string, std::list<std::coroutine_handle<>>, std::less<>>;
		// Need to call RegistrationQueue.resume() on all Registrars after static initialization step and other steps
		// Unfortunately std::set does not have constexpr constructor. Using inline static instead.
//		constinit static std::set<IRegistrar*> registrars;
		inline static std::list<IRegistrar*> registrars;
		static void ResolveDanglingRegisters();

		queue_type registrationQueue{};

		IRegistrar() noexcept;
		virtual ~IRegistrar();

		virtual bool Contains( std::string_view str ) const = 0;
		virtual const IExposable* GetRef( std::string_view str ) const = 0;
		virtual std::shared_ptr<IExposable> GetPtr( std::string_view str ) const;
		virtual bool TryRegister( std::shared_ptr<IExposable> ptr );
		virtual bool Erase( std::string_view item ) = 0;

		virtual void ResolvePostRegister( std::string_view str );
		IRegistrationTask AwaitGet( std::string_view name, const IExposable*& value );
		IRegistrationTask AwaitGet( std::string_view name, std::shared_ptr<IExposable>& value );
		void Get( std::string_view name, const IExposable*& value, bool await = true );
		void Get( std::string_view name, std::vector<const IExposable*>& value, bool await = true );
		void Get( std::string_view name, std::shared_ptr<IExposable>& value, bool await = true );
		void Get( std::string_view name, std::vector<std::shared_ptr<IExposable>>& value, bool await = true );
	};
	// region Helpers
	template<class T, template<class> class TRegistrar> requires std::derived_from<TRegistrar<T>, IRegistrar>
	struct RegistrarAwaitGet;
	struct IRegistrarAwaitGetRef;
	struct IRegistrarAwaitGetPtr;
	struct RegistrarAwaitGetBase;
	/**
	 * Co_await return type for registrar operations
	 */
	struct IRegistrationTask {
		struct promise_type {
			IRegistrar& registrar;
			std::string name;

			IRegistrationTask get_return_object();
			std::suspend_never initial_suspend();
			std::suspend_never final_suspend() noexcept;
			void return_void();
			void unhandled_exception();
			template<class T, template<class> class TRegistrar>
			requires std::derived_from<TRegistrar<T>, IRegistrar>
			RegistrarAwaitGet<T, TRegistrar> await_transform( RegistrarAwaitGet<T, TRegistrar>&& awaiter ) {
				auto output = std::move(awaiter);
				output.promise = this;
				return output;
			}
			IRegistrarAwaitGetRef await_transform( IRegistrarAwaitGetRef&& awaiter );
			IRegistrarAwaitGetPtr await_transform( IRegistrarAwaitGetPtr&& awaiter );
			// Specific overload constructors to ensure it is called by AwaitGet or similar syntax functions
			template<class ...Args>
			promise_type( IRegistrar& registrar, std::string_view name, Args&& ... ):
					registrar{registrar}, name{name} { };
			template<class TRegistrar, class ...Args>
			promise_type( TRegistrar& registrar, std::string_view name, Args&& ... ):
					registrar{registrar}, name{name} { };
			// Additional overloaders for non-static function calls
			template<class T, class ...Args>
			promise_type( T&&, IRegistrar& registrar, std::string_view name, Args&& ... ):
					registrar{registrar}, name{name} { };
			template<class T, class TRegistrar, class ...Args>
			promise_type( T&&, TRegistrar& registrar, std::string_view name, Args&& ... ):
					registrar{registrar}, name{name} { };
			std::coroutine_handle<promise_type> GetHandle();
		};
		std::coroutine_handle<promise_type> handle;
		operator std::coroutine_handle<promise_type>() const;
		operator std::coroutine_handle<>() const;
	};
	using RegistrationPromise = IRegistrationTask::promise_type;
	using RegistrationHandle = std::coroutine_handle<RegistrationPromise>;
	struct RegistrarAwaitGetBase {
		RegistrationPromise* promise;
		bool await_ready() const;
		bool await_suspend( std::coroutine_handle<IRegistrationTask::promise_type> h ) const;
	protected:
		RegistrarAwaitGetBase() = default;
	};
//	/**
//	 * Helper struct to co_await registrar.get
//	 * @tparam TRegistrar
//	 * @tparam T
//	 */
	template<class T, template<class> class TRegistrar> requires std::derived_from<TRegistrar<T>, IRegistrar>
	struct RegistrarAwaitGet :
			RegistrarAwaitGetBase {
		using value_type = T;
		using registrar_type = TRegistrar<T>;

		TRegistrar<T>& registrar;
		auto await_resume() const;
		explicit RegistrarAwaitGet( TRegistrar<T>& registrar ) :
				registrar{registrar} { };
		static_assert(std::same_as<T, T>,
		              "Specialization required for each registrar type in order to get a useful return type");
	};
	struct IRegistrarAwaitGetRef :
			RegistrarAwaitGetBase {
		const IExposable* await_resume() const;
	};
	struct IRegistrarAwaitGetPtr :
			RegistrarAwaitGetBase {
		std::shared_ptr<IExposable> await_resume() const;
	};
	namespace Internal {
		template<class T>
		struct RegistrarKeyMap {
			using map_type = std::map<std::reference_wrapper<const T>, std::list<std::coroutine_handle<>>>;
			map_type map;
		};
		template<class T> requires std::copy_constructible<T> || std::move_constructible<T>
		struct RegistrarKeyMap<T> {
			using map_type = std::map<T, std::list<std::coroutine_handle<>>>;
			map_type map;
		};
		template<class T>
		struct RegistrarKeyHolder {
			using has_key_copy = std::false_type;
			const T& keyRef;
			explicit RegistrarKeyHolder( const T& key ) : keyRef{key} { }
		};
		template<class T> requires std::move_constructible<T> && std::copy_constructible<T>
		struct RegistrarKeyHolder<T> {
			using has_key_copy = std::true_type;
			T key;
			const T& keyRef;
			explicit RegistrarKeyHolder( const T& key ) : key{key}, keyRef(this->key) { }
			explicit RegistrarKeyHolder( T&& key ) : key{std::move(key)}, keyRef(this->key) { }
		};
		template<class T> requires std::move_constructible<T> && (!std::copy_constructible<T>)
		struct RegistrarKeyHolder<T> {
			using has_key_copy = std::true_type;
			T key;
			const T& keyRef;
			explicit RegistrarKeyHolder( const T& key ) = delete;
			explicit RegistrarKeyHolder( T&& key ) : key{key}, keyRef(this->key) { }
		};
	}
	// endregion
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_IREGISTRAR_HPP
