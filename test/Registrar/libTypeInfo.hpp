//
// Created by lecris on 2021-12-10.
//

#ifndef QUANFLOQ_LIBTYPEINFO_HPP
#define QUANFLOQ_LIBTYPEINFO_HPP

#include "Registrar/SharedRegistrar.tpp"
#include "Registrar/TypeInfo.tpp"

namespace QuanFloq {
	struct IExposable {
		virtual ~IExposable() = default;
	};
	struct BaseFactory {
	};
	template<class>
	struct Factory : BaseFactory {
	};
	class A1 {
	public:
		static const TypeInfo& typeInfo;
	};
	class A {
	public:
		static const TypeInfo& typeInfo;
		static SharedRegistrarRoot<A> registrar;
		static Factory<A> factory;
		std::string_view GetName() const { return ""; };
	};
	class A2 {
	public:
		static const TypeInfo& typeInfo;
	};
	class B {
	public:
		static const TypeInfo& typeInfo;
	};
	template<class T>
	class C {
	public:
		static const TypeInfo* typeInfo;
		static_assert(&typeInfo);
	};
	template<class T>
	inline const TypeInfo* C<T>::typeInfo = &TypeInfo::Create<C<T>>();
	inline const TypeInfo& A1::typeInfo = TypeInfo::Create<A1, A>("Suffix1");
	inline const TypeInfo& A::typeInfo = TypeInfo::Create<A>();
	inline const TypeInfo& A2::typeInfo = TypeInfo::Create<A2, A>("Suffix2");
	inline const TypeInfo& B::typeInfo = TypeInfo::Create<B>("BNamed");
	inline SharedRegistrarRoot<A> A::registrar;
	inline Factory<A> A::factory;
}

#endif //QUANFLOQ_LIBTYPEINFO_HPP
