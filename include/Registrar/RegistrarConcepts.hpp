//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_REGISTRARCONCEPTS_HPP
#define QUANFLOQ_REGISTRARCONCEPTS_HPP

#include <concepts>
#include <string>
#include <memory>
#include <vector>

namespace QuanFloq {
	// Forward declarations
	struct IRegistrar;
	template<class>
	struct RefRegistrarRoot;
	template<class>
	struct SharedRegistrarRoot;
	struct RegistrationBase;
	struct TypeRegistration;
	template<class>
	class Factory;


	template<class T>
	concept stdRegistered = std::derived_from<decltype(T::registration), RegistrationBase>;
	// TODO: Cannot detect if proper type is registered without constexpr magic
	template<class T>
	concept stdTypeRegistered = std::derived_from<decltype(T::typeRegistration), TypeRegistration>;
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
	};
	template<class T, class Base = typename decltype(T::registrar)::value_type>
	concept stdRegistrar = std::derived_from<T, Base> && stdIRegistrar<T> &&
	                       std::derived_from<decltype(T::registrar), RefRegistrarRoot<Base>>;
	template<class T, class Base = typename decltype(T::registrar)::value_type>
	concept stdSharedRegistrar = std::derived_from<T, Base> && stdIRegistrar<T> &&
	                             std::derived_from<decltype(T::registrar), SharedRegistrarRoot<Base>>;
	template<class T, class Base = typename decltype(T::registrar)::value_type>
	concept stdRefRegistrar = stdRegistrar<T, Base> && !stdSharedRegistrar<T, Base>;

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
	// TODO: Custom hash redefinition because specialization fails
	template<QuanFloq::stdNamed T>
	struct hash : std::hash<std::string_view> {
		using is_transparent = void;
		using std::hash<std::string_view>::operator();
		size_t operator()( const std::shared_ptr<T>& val ) const noexcept {
			if (val == nullptr)
				return operator()("");
			return operator()(val->GetName());
		}
	};
}
template<QuanFloq::stdNamed T>
struct std::hash<T> : std::hash<std::string_view> {
	using is_transparent = void;
	using std::hash<std::string_view>::operator();
	size_t operator()( const T& val ) const noexcept {
		return operator()(val.GetName());
	}
};
template<QuanFloq::stdNamed T>
struct std::hash<std::reference_wrapper<T>> : std::hash<std::string_view> {
	using is_transparent = void;
	using std::hash<std::string_view>::operator();
	size_t operator()( const std::reference_wrapper<T>& val ) const noexcept {
		return operator()(val.get().GetName());
	}
};

// TODO: This specialization fails
//template<QuanFloq::stdNamed T>
//struct std::hash<std::shared_ptr<T>> : std::hash<std::string_view> {
//	using is_transparent = void;
//	using std::hash<std::string_view>::operator();
//	size_t operator()( const std::shared_ptr<T>& val ) const noexcept {
//		if (val == nullptr)
//			return operator()("");
//		return operator()(val->GetName());
//	}
//};

template<QuanFloq::stdNamed T>
struct std::less<T> {
	using is_transparent = void;
	constexpr bool operator()( const T& lhs, const T& rhs ) const {
		return lhs.GetName() < rhs.GetName();
	}
	constexpr bool operator()( const T& lhs, std::string_view rhs ) const {
		return lhs.GetName() < rhs;
	}
	constexpr bool operator()( std::string_view lhs, const T& rhs ) const {
		return lhs < rhs.GetName();
	}
};
template<QuanFloq::stdNamed T>
struct std::less<std::reference_wrapper<T>> {
	using is_transparent = void;
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const std::reference_wrapper<T>& rhs ) const {
		return lhs.get().GetName() < rhs.get().GetName();
	}
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, std::string_view rhs ) const {
		return lhs.get().GetName() < rhs;
	}
	constexpr bool operator()( std::string_view lhs, const std::reference_wrapper<T>& rhs ) const {
		return lhs < rhs.get().GetName();
	}
};
template<QuanFloq::stdNamed T>
struct std::less<std::shared_ptr<T>> {
	using is_transparent = void;
	constexpr bool operator()( const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs ) const {
		if (lhs == nullptr)
			return rhs != nullptr;
		if (rhs == nullptr)
			return false;
		return lhs->GetName() < rhs->GetName();
	}
	constexpr bool operator()( const std::shared_ptr<T>& lhs, std::string_view rhs ) const {
		if (lhs == nullptr)
			return !rhs.empty();
		return lhs->GetName() < rhs;
	}
	constexpr bool operator()( std::string_view lhs, const std::shared_ptr<T>& rhs ) const {
		if (rhs == nullptr)
			return false;
		return lhs < rhs->GetName();
	}
};
template<QuanFloq::stdNamed T>
struct std::equal_to<T> {
	using is_transparent = void;
	constexpr bool operator()( const T& lhs, const T& rhs ) const {
		return lhs == rhs;
	}
	constexpr bool operator()( const T& lhs, std::string_view rhs ) const {
		return lhs.GetName() == rhs;
	}
	constexpr bool operator()( std::string_view lhs, const T& rhs ) const {
		return lhs == rhs.GetName();
	}
};
template<QuanFloq::stdNamed T>
struct std::equal_to<std::reference_wrapper<T>> {
	using is_transparent = void;
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, const std::reference_wrapper<T>& rhs ) const {
		return lhs.get() == rhs.get();
	}
	constexpr bool operator()( const std::reference_wrapper<T>& lhs, std::string_view rhs ) const {
		return lhs.get().GetName() == rhs;
	}
	constexpr bool operator()( std::string_view lhs, const std::reference_wrapper<T>& rhs ) const {
		return lhs == rhs.get().GetName();
	}
};
template<QuanFloq::stdNamed T>
struct std::equal_to<std::shared_ptr<T>> {
	using is_transparent = void;
	constexpr bool operator()( const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs ) const {
		return lhs.get() == rhs.get();
	}
	constexpr bool operator()( const std::shared_ptr<T>& lhs, std::string_view rhs ) const {
		if (lhs == nullptr)
			return rhs.empty();
		return lhs->GetName() == rhs;
	}
	constexpr bool operator()( std::string_view lhs, const std::shared_ptr<T>& rhs ) const {
		if (rhs == nullptr)
			return lhs.empty();
		return lhs == rhs->GetName();
	}
};
#endif //QUANFLOQ_REGISTRARCONCEPTS_HPP
