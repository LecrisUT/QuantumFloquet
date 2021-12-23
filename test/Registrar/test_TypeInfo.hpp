//
// Created by lecris on 2021-12-23.
//

#ifndef QUANFLOQ_TEST_REGISTRAR_TEST_TYPEINFO_HPP
#define QUANFLOQ_TEST_REGISTRAR_TEST_TYPEINFO_HPP

#include "libTypeInfoBase.hpp"
#include "Registrar/SharedRegistrar.tpp"

namespace QuanFloq {
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
		static const TypeInfo* const typeInfo;
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
	inline const TypeInfo& A1::typeInfo = TypeInfo::Create<A1, A>("Suffix1");
	inline const TypeInfo* const A::typeInfo = &TypeInfo::Create<A>();
	inline const TypeInfo& A2::typeInfo = TypeInfo::Create<A2, A>("Suffix2");
	inline const TypeInfo& B::typeInfo = TypeInfo::Create<B>("BNamed");
	inline SharedRegistrarRoot<A> A::registrar;
	inline Factory<A> A::factory;
}

#endif //QUANFLOQ_TEST_REGISTRAR_TEST_TYPEINFO_HPP
