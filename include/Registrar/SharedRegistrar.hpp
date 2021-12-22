//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_SHAREDREGISTRAR_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_SHAREDREGISTRAR_HPP
// Needed for std::less specializations and others

#include "RegistrarConcepts.hpp"
#include "IRegistrar.hpp"
#include <set>
#include <unordered_map>

namespace QuanFloq {
	/**
	 * Basic Registrar owning object instances
	 * @tparam T
	 */
	template<class T>
	struct SharedRegistrarRoot :
			IRegistrar {
		using value_type = T;
		using ptr_type = std::shared_ptr<value_type>;
		struct IQueue;
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		struct Queue;
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		struct RegistrationTask;
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		struct Awaiter;
		using map_type = std::unordered_map<std::string_view, ptr_type>;
		using set_type = std::set<ptr_type>;
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		using task_type = RegistrationTask<Key>;
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		using awaiter_type = Awaiter<Key>;
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		struct RegistrationTask {
			using task_type = typename SharedRegistrarRoot<T>::task_type<Key>;
			using awaiter_type = typename SharedRegistrarRoot<T>::awaiter_type<Key>;
			struct promise_type;
			std::coroutine_handle<promise_type> handle;
			operator std::coroutine_handle<promise_type>() const { return handle; }
			operator std::coroutine_handle<>() const { return handle; }
		};
		template<class Key> requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		using promise_type = typename RegistrationTask<Key>::promise_type;

		using IRegistrar::ResolvePostRegister;
		using IRegistrar::AwaitGet;
		using IRegistrar::Get;

		inline static const RegistrarOperators <ptr_type> operators;
		inline static const RegistrarOperators <value_type> valueOperators;
		map_type map{};
		set_type set{};
		std::list<IQueue*> queues{};
		std::set<std::string, std::less<>> nameSet{};


		bool Contains( std::string_view str ) const override;
		bool Contains( value_type& item ) const;
		bool Contains( value_type* const item ) const;
		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		bool Contains( const Key& key ) const;
		const IExposable* GetRef( std::string_view str ) const override;
		std::shared_ptr<IExposable> GetPtr( std::string_view str ) const override;

		bool TryRegister( std::shared_ptr<IExposable> ptr ) override;
		virtual std::pair<typename set_type::iterator, bool> Register( ptr_type item );
		virtual bool RegisterName( std::string_view name, ptr_type item );
		bool RegisterName( std::string&& name, ptr_type item );
		virtual bool Erase( value_type& item );
		virtual bool Erase( value_type* const item );
		bool Erase( ptr_type item );
		bool Erase( std::string_view item ) override;

		virtual void ResolvePostRegister( ptr_type item );
		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		task_type<Key> AwaitGet( const Key& key, value_type*& value );
		template<class Key> requires (TransparentEqualKey <
		                              Key, std::shared_ptr<T>> || TransparentLessKey <Key, std::shared_ptr<T>>) &&
		std::move_constructible<Key>
		task_type<Key> AwaitGet( Key&& key, value_type*& value );
		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		task_type<Key> AwaitGet( const Key& key, ptr_type& value );
		template<class Key>
		requires (TransparentEqualKey < Key, ptr_type > || TransparentLessKey < Key, ptr_type >) &&
		         std::move_constructible<Key>
		task_type<Key> AwaitGet( Key&& key, ptr_type& value );
		IRegistrationTask AwaitGet( std::string_view name, ptr_type& value );
		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		void AwaitFor( const Key& key, std::coroutine_handle<> handle );
		template<class Key> requires (TransparentEqualKey <
		                              Key, std::shared_ptr<T>> || TransparentLessKey <Key, ptr_type>) &&
		std::move_constructible<Key>
		void AwaitFor( Key&& key, std::coroutine_handle<> handle );
		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		void Get( const Key& key, value_type*& value, bool await = true );
		template<class Key>
		requires (TransparentEqualKey < Key, std::shared_ptr<T> > || TransparentLessKey < Key, std::shared_ptr<T> >) &&
		         std::move_constructible<Key>
		void Get( Key&& key, value_type*& value, bool await = true );
		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		void Get( const Key& key, ptr_type& value, bool await = true );
		template<class Key>
		requires (TransparentEqualKey < Key, std::shared_ptr<T> > || TransparentLessKey < Key, std::shared_ptr<T> >) &&
		         std::move_constructible<Key>
		void Get( Key&& key, ptr_type& value, bool await = true );
		void Get( std::string_view name, value_type*& value, bool await = true );
		void Get( std::string_view name, std::vector<value_type*>& value, bool await = true );
		void Get( std::string_view name, ptr_type& value, bool await = true );
		void Get( std::string_view name, std::vector<ptr_type>& value, bool await = true );

		template<class Key>
		requires TransparentEqualKey<Key, ptr_type> || TransparentLessKey<Key, ptr_type>
		ptr_type operator[]( const Key& key ) const;
		ptr_type operator[]( std::string_view str ) const;
		ptr_type operator[]( value_type& str ) const;
		ptr_type operator[]( value_type* const str ) const;
		struct IQueue {
			using value_type = typename SharedRegistrarRoot<T>::value_type;
			using ptr_type = typename SharedRegistrarRoot<T>::ptr_type;
			SharedRegistrarRoot<T>& registrar;
			explicit IQueue( SharedRegistrarRoot<T>& registrar ) :
					registrar{registrar} {
				registrar.queues.push_back(this);
			}
//			~IQueue() {
//				registrar.queues.remove(this);
//			}
			virtual void Resolve( ptr_type item ) = 0;
		};
	};
	/**
	 * Subregistrar containing derived classes for faster searching.
	 * Registered objects are saved in root registrar as well.
	 * @tparam T Derived saved class
	 * @tparam U Base class
	 */
	template<class T, class U>
	struct SharedRegistrar :
			SharedRegistrarRoot<T> {
		using base_type = U;
		using base = SharedRegistrarRoot<T>;
		using typename base::value_type;
		using typename base::ptr_type;
		using base::RegisterName;

		SharedRegistrarRoot<U>& Root;

		SharedRegistrar() noexcept requires stdSharedRegistrar<U>:
				Root{U::registrar} {
			static_assert(std::derived_from<T, U>);
		};
		explicit SharedRegistrar( SharedRegistrarRoot<U>& root ) noexcept:
				Root{root} {
			static_assert(std::derived_from<T, U>);
		};

		std::pair<typename base::set_type::iterator, bool> Register( ptr_type item ) override;
		bool RegisterName( std::string_view name, ptr_type item ) override;
		bool Erase( value_type& item ) override;
		bool Erase( value_type* const item ) override;
		bool Erase( std::string_view item ) override;
		void ResolvePostRegister( std::string_view str ) override;
		void ResolvePostRegister( ptr_type item ) override;
	};
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_SHAREDREGISTRAR_HPP
