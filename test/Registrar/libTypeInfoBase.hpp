//
// Created by lecris on 2021-12-23.
//

#ifndef QUANFLOQ_TEST_REGISTRAR_LIBTYPEINFOBASE_HPP
#define QUANFLOQ_TEST_REGISTRAR_LIBTYPEINFOBASE_HPP

#include "Registrar/SharedRegistrar.tpp"
#include "Registrar/TypeInfo.tpp"

namespace QuanFloq {
	struct IExposable {
		virtual ~IExposable() = default;
	};
	template<class T>
	class C {
	public:
		static const TypeInfo& typeInfo;
	};
	// The following should instantiate typeInfo because of the static_assert
	template<class T>
	class D {
	public:
		static const TypeInfo* typeInfo;
		static_assert(&typeInfo);
	};
	// The following is less robust because the parent constructor needs to be defined
	template<class T>
	class E {
	public:
		static const TypeInfo& typeInfo;
		E() {
			(void)typeInfo;
		}
	};
	template<class T>
	inline const TypeInfo& C<T>::typeInfo = TypeInfo::Create<C<T>>();
	template<class T>
	inline const TypeInfo* D<T>::typeInfo = &TypeInfo::Create<T>();
	template<class T>
	inline const TypeInfo& E<T>::typeInfo = TypeInfo::Create<T>();
}

#endif //QUANFLOQ_TEST_REGISTRAR_LIBTYPEINFOBASE_HPP
