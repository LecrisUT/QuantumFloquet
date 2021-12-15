//
// Created by lecris on 2021-11-10.
//

#ifndef QUANFLOQ_SCRIBECONCEPTS_HPP
#define QUANFLOQ_SCRIBECONCEPTS_HPP

#include "Registrar/RegistrarConcepts.hpp"
#include <concepts>
#include <vector>
#include <complex>

namespace QuanFloq {
	// Forward declarations
	class IExposable;
	class ScribeDriver;
	class Scriber;

	template<class T>
	concept Exposable = std::derived_from<T, IExposable>;

	template<class T>
	concept IntType = std::signed_integral<T>;
	template<class T>
	concept UIntType = std::unsigned_integral<T>;
	template<class T>
	concept DoubleType = std::floating_point<T>;;
	template<class T>
	concept CDoubleType = !IntType<T> && !UIntType<T> && !DoubleType<T> &&
	                      std::convertible_to<T, std::complex<double>>;
	template<class T>
	concept StringType = std::convertible_to<T, std::string>;
	template<class T>
	concept Scribeable = IntType<T> || UIntType<T> || DoubleType<T> || CDoubleType<T> || StringType<T>;
	template<Scribeable T>
	struct Scribe_traits {
		using equivalent_type = void;
		static_assert(!std::same_as<T, T>,
		              "Scribe_traits has to be specialized");
	};
	template<IntType T>
	struct Scribe_traits<T> {
		using equivalent_type = long;
	};
	template<UIntType T>
	struct Scribe_traits<T> {
		using equivalent_type = size_t;
	};
	template<DoubleType T>
	struct Scribe_traits<T> {
		using equivalent_type = double;
	};
	template<CDoubleType T>
	struct Scribe_traits<T> {
		using equivalent_type = std::complex<double>;
	};
	template<StringType T>
	struct Scribe_traits<T> {
		using equivalent_type = std::string;
	};

	template<class T>
	concept StaticScribeable = !Exposable<T> && stdSharedRegistrar<T> && stdNamed<T>;
}

#endif //QUANFLOQ_SCRIBECONCEPTS_HPP
