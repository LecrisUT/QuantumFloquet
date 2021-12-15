//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_INTERFACE_SCRIBER_FACTORYREQUEST_HPP
#define QUANFLOQ_INCLUDE_INTERFACE_SCRIBER_FACTORYREQUEST_HPP

#include "ScribeConcepts.hpp"
#include "interface/ICloneable.hpp"

namespace QuanFloq {
	// Predeclare superclasses
	class IExposable;
	struct RegistrationTask;

	struct FactoryRequestBase :
			ICloneable<FactoryRequestBase> {
		bool arrayType;
		bool sharedType;
		bool scribeData;
		std::vector<std::reference_wrapper<IExposable>> values;
		int requestsPending = 0;
		int requestsFinished = 0;
		virtual RegistrationTask AwaitGet( IRegistrar& registrar, std::string_view name ) = 0;
		virtual void Get( IRegistrar& registrar, std::string_view name, bool await = true ) = 0;
		virtual void Integrate( std::shared_ptr<IExposable> item ) = 0;
		virtual ~FactoryRequestBase() = default;
	};
	template<Exposable T>
	struct IFactoryRequest :
			FactoryRequestBase {
		virtual IExposable& Integrate( std::unique_ptr<T>&& item ) = 0;
	};

	template<template<class> class P, Exposable T> requires sptr<P<T>, T>
	struct FactoryRequest :
			IFactoryRequest<T> {
		P<T>* location;
		std::vector<P<T>>* vecLocation;
		std::unique_ptr<FactoryRequestBase> Clone() const override;
		RegistrationTask AwaitGet( IRegistrar& registrar, std::string_view name ) override;
		void Get( IRegistrar& registrar, std::string_view name, bool await = true ) override;
		IExposable& Integrate( std::unique_ptr<T>&& item ) override;
		void Integrate( std::shared_ptr<IExposable> item ) override;
		explicit FactoryRequest( P<T>& loc, bool scribeData = false );
		explicit FactoryRequest( std::vector<P<T>>& loc, bool scribeData = false );
	};
}

#endif //QUANFLOQ_INCLUDE_INTERFACE_SCRIBER_FACTORYREQUEST_HPP
