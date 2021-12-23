//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_TEST_REGISTRAR_LIBSRIBER_HPP
#define QUANFLOQ_TEST_REGISTRAR_LIBSRIBER_HPP

#include "interface/IExposable.hpp"
#include "Registrar/Factory.tpp"
#include "Registrar/TypeInfo.tpp"
#include "Registrar/Registration.hpp"

namespace QuanFloq {
	struct DBase;
	struct CBase : IExposable {
		static SharedRegistrarRoot<CBase> registrar;
	};
	struct BBase : IExposable {
	};

	struct A :
			IExposable {
		static Factory<A> factory;
		static const TypeInfo* const typeInfo;

		std::string name;
		std::shared_ptr<CBase> CItem;
		std::vector<std::shared_ptr<CBase>> CArray;
		std::shared_ptr<DBase> DItem;
		std::vector<std::shared_ptr<DBase>> DArray;
		std::unique_ptr<BBase> BItem;
		std::vector<std::unique_ptr<BBase>> BArray;
		int intVal;
		std::vector<int> intVec;
		size_t sizetVal;
		std::vector<size_t> sizetVec;
		float floatVal;
		std::vector<float> floatVec;
		double doubleVal;
		std::vector<double> doubleVec;
		std::vector<std::string> stringVec;

		void ExposeData( Scriber& scriber ) override;
		std::string_view GetName() const override;
	};
	struct B : BBase {
		static FactoryWithBases<B, BBase> factory;
		static const TypeInfo* const typeInfo;

		std::string name;
		std::shared_ptr<CBase> CItem;

		void ExposeData( Scriber& scriber ) override;
		std::string_view GetName() const override;
	};
	struct C : CBase {
		static FactoryWithBases<C, CBase> factory;
		static const TypeInfo* const typeInfo;

		std::string name;

		void ExposeData( Scriber& scriber ) override;
		std::string_view GetName() const override;
	};
	struct DBase {
		static SharedRegistrarRoot<DBase> registrar;

		virtual std::string_view GetName() const = 0;
	};
	struct D1 : DBase {
		std::string_view GetName() const override;
		static PtrRegistration registration;
	};
	// Type registration has to occur first
	inline const TypeInfo* const A::typeInfo = &TypeInfo::Create<A>("AType");
	inline const TypeInfo* const B::typeInfo = &TypeInfo::Create<B>("BType");
	inline const TypeInfo* const C::typeInfo = &TypeInfo::Create<C>("CType");
	inline SharedRegistrarRoot<CBase> CBase::registrar;
	inline SharedRegistrarRoot<DBase> DBase::registrar;
	inline Factory<A> A::factory;
	inline FactoryWithBases<B, BBase> B::factory;
	inline FactoryWithBases<C, CBase> C::factory;
	inline PtrRegistration D1::registration = PtrRegistration::Create<D1>();
}

#endif //QUANFLOQ_TEST_REGISTRAR_LIBSRIBER_HPP
