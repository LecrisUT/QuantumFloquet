//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_REGISTRARCONCEPTS_HPP
#define QUANFLOQ_REGISTRARCONCEPTS_HPP

#include <concepts>
#include <string>
#include <memory>
#include <vector>
#include <exception>
#include <typeindex>

namespace QuanFloq {
	// Forward declarations
	struct IRegistrar;
	template<class>
	struct RefRegistrarRoot;
	template<class>
	struct SharedRegistrarRoot;
	template<class>
	struct ObjectRegistrarRoot;
	struct RegistrationBase;
	template<class>
	class Factory;
	struct TypeInfo;

	struct ResolveFailed : std::exception {
		std::string message;
		std::type_index type;
		explicit ResolveFailed( std::string_view component, const std::type_index& type = typeid(void)) : type(type) {
			message = "Could not resolve " + std::string(component) + " for " + type.name();
		}
		const char* what() const noexcept override {
			return message.c_str();
		}
	};

	template<class T>
	concept stdRegistered = std::derived_from<decltype(T::registration), RegistrationBase>;
	// TODO: Cannot detect if proper type is registered without constexpr magic
	template<class T>
	concept stdTypeRegistered = std::same_as<decltype(T::typeInfo), const TypeInfo* const>;
	template<class T>
	concept stdFactory = std::derived_from<decltype(T::factory), Factory<T>>;

	/**
	 * Helper concept for smart pointers.
	 * @tparam P Template of the smart pointer
	 * @tparam T Type of the object
	 */
	template<class P, class T>
	concept sptr = std::derived_from<P, std::unique_ptr<T>> ||
	               std::derived_from<P, std::shared_ptr<T>>;
	/**
	 * Helper concept for vector of smart pointers.
	 * @tparam P Template of the smart pointer
	 * @tparam T Type of the object
	 */
	template<class P, class T>
	concept sptrVec = std::derived_from<P, std::vector<std::unique_ptr<T>>> ||
	                  std::derived_from<P, std::vector<std::shared_ptr<T>>>;
	/**
	 * Has standardized naming function.
	 */
	template<class T>
	concept stdNamed = requires( const T& a ) {
		// Const type here to enforce a::GetName() const
		{ a.GetName() } -> std::convertible_to<std::string_view>;
	};
	/**
	 * Helper concept for string comparables. Currently using dummy because of recursion error.
	 */
	template<class T>
	concept strComparable = requires( const T& a, std::string_view str ){
		a <=> str;
		a == str;
	};
//	concept strComparable = stdNamed<T>;
	template<stdNamed T>
	auto operator<=>( const T& a, std::string_view str ) {
		return a.GetName() <=> str;
	}
	template<stdNamed T>
	bool operator==( const T& a, std::string_view str ) {
		return a.GetName() == str;
	}
	template<stdNamed T>
	auto operator<=>( const T& a, const T& b ) {
		return a.GetName() <=> b.GetName();
	}
	template<stdNamed T>
	bool operator==( const T& a, const T& b ) {
		return a.GetName() == b.GetName();
	}

	template<class T>
	concept stdIRegistrar = std::derived_from<decltype(T::registrar), IRegistrar> && requires{
		// Static registrar member is an IRegistrar and it has a member type to retrieve the template variable
		typename decltype(T::registrar)::value_type;
	} && std::derived_from<T, typename decltype(T::registrar)::value_type>;
	template<class T>
	concept stdSharedRegistrar = stdIRegistrar<T> && std::derived_from<decltype(T::registrar),
			SharedRegistrarRoot<typename decltype(T::registrar)::value_type>>;
	template<class T>
	concept stdRefRegistrar = stdIRegistrar<T> && std::derived_from<decltype(T::registrar),
			RefRegistrarRoot<typename decltype(T::registrar)::value_type>>;
	template<class T>
	concept stdObjRegistrar = stdIRegistrar<T> && std::derived_from<decltype(T::registrar),
			ObjectRegistrarRoot<typename decltype(T::registrar)::value_type>>;
	template<class T>
	concept stdRegistrar = stdRefRegistrar<T> || stdSharedRegistrar<T> || stdObjRegistrar<T>;

	// Cannot use template parameter or derived typename to simplify the following
	/**
	 * Default comparator helper for smart pointers. (Shared pointer version)
	 * @tparam T Type of class
	 */
	template<strComparable T>
	auto operator<=>( const std::shared_ptr<T>& t1, std::string_view t2 ) {
		if (t1 == nullptr)
			return "" <=> t2;
		return *t1 <=> t2;
	}
	/**
	 * Default comparator helper for smart pointers. (Unique pointer version)
	 * @tparam T Type of class
	 */
	template<strComparable T>
	auto operator<=>( const std::unique_ptr<T>& t1, std::string_view t2 ) {
		if (t1 == nullptr)
			return "" <=> t2;
		return *t1 <=> t2;
	}
	/**
	 * Default comparator between reference_wrappers required to use std::set with std::less<>
	 * @tparam T
	 */
	template<class T>
	auto operator<=>( const std::reference_wrapper<T>& t1, const std::reference_wrapper<T>& t2 ) {
		return t1.get() <=> t2.get();
	}
	/**
	 * Default comparator of reference_wrapper with string_view for operator[]
	 * @tparam T
	 */
	template<strComparable T>
	auto operator<=>( const std::reference_wrapper<T>& t1, std::string_view t2 ) {
		return t1.get() <=> t2;
	}
	/**
	 * Default equality comparator helper for smart pointers. (Shared pointer version)
	 * @tparam T Type of class
	 */
	template<strComparable T>
	auto operator==( const std::shared_ptr<T>& t1, std::string_view t2 ) {
		if (t1 == nullptr)
			return t2.empty();
		return *t1 == t2;
	}
	/**
	 * Default equality comparator helper for smart pointers. (Unique pointer version)
	 * @tparam T Type of class
	 */
	template<strComparable T>
	auto operator==( const std::unique_ptr<T>& t1, std::string_view t2 ) {
		if (t1 == nullptr)
			return t2.empty();
		return *t1 == t2;
	}
	template<class T>
	bool operator==( const std::reference_wrapper<T>& t1, const std::reference_wrapper<T>& t2 ) {
		return t1.get() == t2.get();
	}
	/**
	 * Default equality comparator of reference_wrapper with string_view for operator[]
	 * @tparam T
	 */
	template<strComparable T>
	bool operator==( const std::reference_wrapper<T>& t1, std::string_view t2 ) {
		return t1.get() == t2;
	}

	template<class T>
	concept Hashable = requires( const T& a ) {
		{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
	};

	template<class T>
	struct DefaultRegistrarOperators {
		using less_type = std::less<T>;
		using equal_Type = std::equal_to<T>;
		using hash_type = std::false_type;
		const less_type less{};
		const equal_Type equal{};
	};
	template<Hashable T>
	struct DefaultRegistrarOperators<T> {
		using less_type = std::less<T>;
		using equal_Type = std::equal_to<T>;
		using hash_type = std::hash<T>;
		const less_type less{};
		const equal_Type equal{};
		const hash_type hash{};
	};
	template<class T>
	struct RegistrarOperators : DefaultRegistrarOperators<T> {
	};

	template<class T>
	struct BaseResolver {
		static std::string_view Name( const T& item ) {
			throw ResolveFailed("Resolve.Name", typeid(T));
		}
	};
	template<class T>
	struct Resolver;
	template<stdNamed T>
	struct Resolver<T> : BaseResolver<T> {
		using base = BaseResolver<T>;
		static std::string_view Name( const T& item ) noexcept {
			return item.GetName();
		}
	};
	template<class T>
	concept NameResolvable = requires( const T& item ){
		{ Resolver<T>::Name(item) } -> std::convertible_to<std::string_view>;
	};
	template<class Key, class T>
	concept TransparentLessKey = std::same_as<Key, T> ||
	                             requires( const std::less<T>& comparator, const Key& key, const T& item ){
		                             typename std::less<T>::is_transparent;
		                             comparator(key, item);
		                             comparator(item, key);
	                             };
	template<class Key, class T>
	concept TransparentEqualKey = std::same_as<Key, T> ||
	                              requires( const std::equal_to<T>& comparator, const Key& key, const T& item ){
		                              typename std::equal_to<T>::is_transparent;
		                              comparator(key, item);
		                              comparator(item, key);
	                              };
	template<class Key, class T>
	concept TransparentKey = std::same_as<Key, T> ||
	                         (TransparentLessKey<Key, T> && TransparentEqualKey<Key, T> &&
	                          requires( const std::hash<T>& hasher, const Key& key ){
		                          typename std::hash<T>::is_transparent;
		                          hasher(key);
	                          });
}
template<class T>
struct std::less<std::reference_wrapper<T>>
		: std::less<T&>, std::less<T*> {
	using is_transparent = void;
	using std::less<T&>::operator();
	using std::less<T*>::operator();
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const std::reference_wrapper<T>& rhs ) const {
		return &lhs < &rhs;
	}
	template<QuanFloq::TransparentLessKey<T&> Key>
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const Key& rhs ) const {
		return std::less<T&>::operator()(lhs.get(), rhs);
	}
	template<QuanFloq::TransparentLessKey<T&> Key>
	constexpr bool operator()( const Key& lhs, const std::reference_wrapper<T>& rhs ) const {
		return std::less<T&>::operator()(lhs, rhs.get());
	}
	template<QuanFloq::TransparentLessKey<T*> Key>
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const Key& rhs ) const {
		return std::less<T*>::operator()(&lhs.get(), rhs);
	}
	template<QuanFloq::TransparentLessKey<T*> Key>
	constexpr bool operator()( const Key& lhs, const std::reference_wrapper<T>& rhs ) const {
		return std::less<T*>::operator()(lhs, &rhs.get());
	}
};
template<QuanFloq::Hashable T>
struct std::hash<std::reference_wrapper<T>>
		: std::hash<T> {
	using is_transparent = void;
	using std::hash<T>::operator();
	constexpr size_t operator()( const std::reference_wrapper<T>& val ) const {
		return std::hash<T>::operator()(val.get());
	}
};
template<class T>
struct std::equal_to<std::reference_wrapper<T>>
		: std::equal_to<T&>, std::equal_to<T*> {
	using is_transparent = void;
	using std::equal_to<T&>::operator();
	using std::equal_to<T*>::operator();
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const std::reference_wrapper<T>& rhs ) const {
		return &lhs == &rhs;
	}
	template<QuanFloq::TransparentEqualKey<T&> Key>
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const Key& rhs ) const {
		return std::equal_to<T&>::operator()(lhs.get(), rhs);
	}
	template<QuanFloq::TransparentEqualKey<T&> Key>
	constexpr bool operator()( const Key& lhs, const std::reference_wrapper<T>& rhs ) const {
		return std::equal_to<T&>::operator()(lhs, rhs.get());
	}
	template<QuanFloq::TransparentEqualKey<T*> Key>
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const Key& rhs ) const {
		return std::equal_to<T*>::operator()(&lhs.get(), rhs);
	}
	template<QuanFloq::TransparentEqualKey<T*> Key>
	constexpr bool operator()( const Key& lhs, const std::reference_wrapper<T>& rhs ) const {
		return std::equal_to<T*>::operator()(lhs, &rhs.get());
	}
};
template<class T>
struct std::less<std::shared_ptr<T>>
		: std::less<T*> {
	using is_transparent = void;
	using std::less<T*>::operator();
	constexpr bool operator()( const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs ) const {
		return std::less<T*>::operator()(lhs.get(), rhs.get());
	}
	constexpr bool operator()( T* const lhs, const std::shared_ptr<T>& rhs ) const {
		return std::less<T*>::operator()(lhs, rhs.get());
	}
	constexpr bool operator()( const std::shared_ptr<T>& lhs, T* const rhs ) const {
		return std::less<T*>::operator()(lhs.get(), rhs);
	}
	template<QuanFloq::TransparentLessKey<T*> Key>
	constexpr bool operator()( const std::shared_ptr<T>& lhs, const Key& rhs ) const {
		return std::less<T*>::operator()(lhs.get(), rhs);
	}
	template<QuanFloq::TransparentLessKey<T*> Key>
	constexpr bool operator()( const Key& lhs, const std::shared_ptr<T>& rhs ) const {
		return std::less<T*>::operator()(lhs, rhs.get());
	}
};
template<class T>
struct std::equal_to<std::shared_ptr<T>>
		: std::equal_to<T*> {
	using is_transparent = void;
	using std::equal_to<T*>::operator();
	constexpr bool operator()( const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs ) const {
		return std::equal_to<T*>::operator()(lhs.get(), rhs.get());
	}
	constexpr bool operator()( T* const lhs, const std::shared_ptr<T>& rhs ) const {
		return std::equal_to<T*>::operator()(lhs, rhs.get());
	}
	constexpr bool operator()( const std::shared_ptr<T>& lhs, T* const rhs ) const {
		return std::equal_to<T*>::operator()(lhs.get(), rhs);
	}
	template<QuanFloq::TransparentEqualKey<T*> Key>
	constexpr bool operator()( const std::shared_ptr<T>& lhs, const Key& rhs ) const {
		return std::equal_to<T*>::operator()(lhs.get(), rhs);
	}
	template<QuanFloq::TransparentEqualKey<T*> Key>
	constexpr bool operator()( const Key& lhs, const std::shared_ptr<T>& rhs ) const {
		return std::equal_to<T*>::operator()(lhs, rhs.get());
	}
};
#endif //QUANFLOQ_REGISTRARCONCEPTS_HPP
