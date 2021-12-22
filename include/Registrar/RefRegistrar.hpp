//
// Created by lecris on 2021-12-21.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_REFREGISTRAR_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_REFREGISTRAR_HPP

// Needed for std::less specializations and others
#include "RegistrarConcepts.hpp"
#include "IRegistrar.hpp"
#include <set>
#include <unordered_map>

namespace QuanFloq {
	/**
	 * Basic Registrar template for searching non-owning objects
	 * @tparam T
	 */
	template<class T>
	struct RefRegistrarRoot :
			IRegistrar {
		using value_type = T;
		using ref_type = std::reference_wrapper<value_type>;
		// TODO: Clean up to use a common Queue template and specialization
		struct IQueue;
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		struct Queue;
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		struct RegistrationTask;
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		struct Awaiter;
		// TODO: Clean up using a Traits class
		// Could be either const or non-const reference as long as the key is const
		using map_type = std::unordered_map<std::string_view, ref_type>;
		using set_type = std::set<ref_type>;
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		using task_type = RegistrationTask<Key>;
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		using awaiter_type = Awaiter<Key>;
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		struct RegistrationTask {
			using task_type = typename RefRegistrarRoot<T>::task_type<Key>;
			using awaiter_type = typename RefRegistrarRoot<T>::awaiter_type<Key>;
			struct promise_type;
			std::coroutine_handle<promise_type> handle;
			operator std::coroutine_handle<promise_type>() const { return handle; }
			operator std::coroutine_handle<>() const { return handle; }
		};
		template<class Key> requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		using promise_type = typename RegistrationTask<Key>::promise_type;

		using IRegistrar::ResolvePostRegister;
		using IRegistrar::AwaitGet;
		using IRegistrar::Get;

		inline static const RegistrarOperators <ref_type> operators;
		map_type map{};
		set_type set{};
		std::list<IQueue*> queues{};
		std::set<std::string> nameSet{};

		bool Contains( std::string_view str ) const override;
		// Already included in the template
		bool Contains( value_type& item ) const;
		template<class Key>
		requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		bool Contains( const Key& key ) const;
		const IExposable* GetRef( std::string_view str ) const override;

		virtual std::pair<typename set_type::iterator, bool> Register( value_type& item );
		virtual bool RegisterName( std::string_view name, value_type& item );
		virtual bool RegisterName( std::string&& name, value_type& item );
		virtual bool Erase( value_type& item );
		bool Erase( std::string_view item ) override;

		virtual void ResolvePostRegister( value_type& item );
		template<class Key>
		requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		task_type<Key> AwaitGet( const Key& key, value_type*& value );
		template<class Key>
		requires (TransparentEqualKey < Key, ref_type > || TransparentLessKey < Key, ref_type >) &&
		         std::move_constructible<Key>
		task_type<Key> AwaitGet( Key&& key, value_type*& value );
		IRegistrationTask AwaitGet( std::string_view name, value_type*& value );
		template<class Key>
		requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		void AwaitFor( const Key& key, std::coroutine_handle<> handle );
		template<class Key>
		requires (TransparentEqualKey < Key, ref_type > || TransparentLessKey < Key, ref_type >) &&
		         std::move_constructible<Key>
		void AwaitFor( Key&& key, std::coroutine_handle<> handle );
		template<class Key>
		requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		void Get( const Key& key, value_type*& value, bool await = true );
		template<class Key>
		requires (TransparentEqualKey < Key, ref_type > || TransparentLessKey < Key, ref_type >) &&
		         std::move_constructible<Key>
		void Get( Key&& key, value_type*& value, bool await = true );
		void Get( std::string_view name, value_type*& value, bool await = true );
		void Get( std::string_view name, std::vector<value_type*>& value, bool await = true );

		template<class Key>
		requires TransparentEqualKey<Key, ref_type> || TransparentLessKey<Key, ref_type>
		value_type& operator[]( const Key& key ) const;
		value_type& operator[]( std::string_view str ) const;
		struct IQueue {
			using value_type = typename RefRegistrarRoot<T>::value_type;
			RefRegistrarRoot<T>& registrar;
			explicit IQueue( RefRegistrarRoot<T>& registrar ) :
					registrar{registrar} {
				registrar.queues.push_back(this);
			}
			~IQueue() {
				registrar.queues.remove(this);
			}
			virtual void Resolve( const value_type& item ) = 0;
		};
	};
	/**
	 * Subregistrar containing derived classes for faster searching.
	 * Registered reference are saved in root registrar as well.
	 * @tparam T Derived saved class
	 * @tparam U Base class
	 */
	template<class T, class U>
	struct RefRegistrar :
			RefRegistrarRoot<T> {
		using base_type = U;
		using base = RefRegistrarRoot<T>;
		using typename base::value_type;

		RefRegistrarRoot<U>& Root;

		RefRegistrar() noexcept requires stdRefRegistrar<U>:
				Root{U::registrar} {
			static_assert(std::derived_from<T, U>);
		};
		explicit RefRegistrar( RefRegistrarRoot<U>& root ) noexcept:
				Root{root} {
			static_assert(std::derived_from<T, U>);
		};

		std::pair<typename base::set_type::iterator, bool> Register( value_type& item ) override;
		bool RegisterName( std::string_view name, value_type& item ) override;
		bool RegisterName( std::string&& name, value_type& item ) override;
		bool Erase( value_type& item ) override;
		bool Erase( std::string_view item ) override;
		void ResolvePostRegister( std::string_view str ) override;
		void ResolvePostRegister( value_type& item ) override;
	};
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_REFREGISTRAR_HPP
