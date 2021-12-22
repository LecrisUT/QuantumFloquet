//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_OBJECTREGISTRAR_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_OBJECTREGISTRAR_HPP

#include "RegistrarConcepts.hpp"
#include "IRegistrar.hpp"
#include <set>
#include <unordered_map>

namespace QuanFloq {
	template<class T>
	struct ObjectRegistrarRoot :
			IRegistrar {
		struct IQueue;
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		struct Queue;
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		struct RegistrationTask;
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		struct Awaiter;
		using value_type = T;
		using ref_type = std::reference_wrapper<const value_type>;
		using map_type = std::unordered_map<std::string_view, ref_type>;
		using set_type = std::set<value_type>;
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		using task_type = RegistrationTask<Key>;
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		using awaiter_type = Awaiter<Key>;
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		struct RegistrationTask {
			using task_type = typename ObjectRegistrarRoot<T>::task_type<Key>;
			using awaiter_type = typename ObjectRegistrarRoot<T>::awaiter_type<Key>;
			struct promise_type;
			std::coroutine_handle<promise_type> handle;
			operator std::coroutine_handle<promise_type>() const { return handle; }
			operator std::coroutine_handle<>() const { return handle; }
		};
		template<class Key> requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		using promise_type = typename RegistrationTask<Key>::promise_type;

		using IRegistrar::ResolvePostRegister;
		using IRegistrar::AwaitGet;
		using IRegistrar::Get;

		inline static const RegistrarOperators <value_type> operators;
		map_type map{};
		set_type set{};
		std::list<IQueue*> queues{};
		std::set<std::string, std::less<>> nameSet{};

		bool Contains( std::string_view str ) const override;
		bool Contains( const value_type& item ) const;
		template<class Key>
		requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		bool Contains( const Key& key ) const;
		const IExposable* GetRef( std::string_view str ) const override;

		virtual std::pair<typename set_type::iterator, bool> Register( value_type&& item );
		template<class ...Args>
		requires std::constructible_from<T, Args...>
		std::pair<typename set_type::iterator, bool> Register( Args&& ... );
		virtual bool RegisterName( std::string_view name, const value_type& item );
		bool RegisterName( std::string&& name, const value_type& item );
		virtual bool Erase( const value_type& item );
		bool Erase( std::string_view item ) override;

		virtual void ResolvePostRegister( const value_type& item );
		template<class Key>
		requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		task_type<Key> AwaitGet( const Key& key, value_type*& value );
		template<class Key>
		requires (TransparentEqualKey < Key, T > || TransparentLessKey < Key, T >) && std::move_constructible<Key>
		task_type<Key> AwaitGet( Key&& key, value_type*& value );
		IRegistrationTask AwaitGet( std::string_view name, value_type*& value );
		template<class Key>
		requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		void AwaitFor( const Key& key, std::coroutine_handle<> handle );
		template<class Key>
		requires (TransparentEqualKey < Key, T > || TransparentLessKey < Key, T >) && std::move_constructible<Key>
		void AwaitFor( Key&& key, std::coroutine_handle<> handle );
		template<class Key>
		requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		void Get( const Key& key, value_type*& value, bool await = true );
		template<class Key>
		requires (TransparentEqualKey < Key, T > || TransparentLessKey < Key, T >) && std::move_constructible<Key>
		void Get( Key&& key, value_type*& value, bool await = true );
		void Get( std::string_view name, value_type*& value, bool await = true );
		void Get( std::string_view name, std::vector<value_type*>& value, bool await = true );

		template<class Key>
		requires TransparentEqualKey<Key, T> || TransparentLessKey<Key, T>
		const value_type& operator[]( const Key& key ) const;
		const value_type& operator[]( std::string_view str ) const;
		struct IQueue {
			using value_type = typename ObjectRegistrarRoot<T>::value_type;
			ObjectRegistrarRoot<T>& registrar;
			explicit IQueue( ObjectRegistrarRoot<T>& registrar ) :
					registrar{registrar} {
				registrar.queues.push_back(this);
			}
			// Possible leaking, but cannot resolve this due to destructor order fiasco
//			~IQueue() {
//				registrar.queues.remove(this);
//			}
			virtual void Resolve( const value_type& item ) = 0;
		};
	};
	/**
	 * Subregistrar containing derived classes for faster searching.
	 * Registered objects are saved in root registrar as well.
	 * @tparam T Derived saved class
	 * @tparam U Base class
	 */
	template<class T, class U>
	struct ObjectRegistrar :
			ObjectRegistrarRoot<T> {
		using base_type = U;
		using base = ObjectRegistrarRoot<T>;
		using typename base::value_type;
		using base::Register;
		using base::RegisterName;

		SharedRegistrarRoot <U>& Root;

		ObjectRegistrar() noexcept requires stdObjRegistrar<U>:
				Root{U::registrar} {
			static_assert(std::derived_from<T, U>);
		};
		explicit ObjectRegistrar( ObjectRegistrarRoot<U>& root ) noexcept:
				Root{root} {
			static_assert(std::derived_from<T, U>);
		};

		std::pair<typename base::set_type::iterator, bool> Register( value_type&& item ) override;
		bool RegisterName( std::string_view name, const value_type& item ) override;
		bool Erase( const value_type& item ) override;
		bool Erase( std::string_view item ) override;
		void ResolvePostRegister( std::string_view str ) override;
		void ResolvePostRegister( const value_type& item ) override;
	};
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_OBJECTREGISTRAR_HPP
