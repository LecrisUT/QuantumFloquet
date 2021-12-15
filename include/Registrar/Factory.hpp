//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_HPP

#include "RegistrarConcepts.hpp"

namespace QuanFloq {
	// Predeclare superclasses
	class IExposable;
	class FactoryRequestBase;

	struct FactoryBase {
		static RefRegistrarRoot<FactoryBase> registrar;
		virtual IExposable& Make( FactoryRequestBase& bReq, IExposable* parent ) = 0;
		virtual void Make( FactoryRequestBase& bReq, IExposable* parent, std::shared_ptr<IExposable>& location ) = 0;
		virtual std::string GetName() const = 0;
	};

	template<class T>
	struct Factory :
			FactoryBase {
		std::string GetName() const final;
		virtual std::unique_ptr<T> Make( IExposable* parent );
		template<std::constructible_from<T> ...Args>
		std::unique_ptr<T> Make( Args&& ... args );
		IExposable& Make( FactoryRequestBase& bReq, IExposable* parent ) override;
		void Make( FactoryRequestBase& bReq, IExposable* parent, std::shared_ptr<IExposable>& location ) final;
		Factory();
	};

	template<class T, class ...Args>
	struct FactoryWithBases :
			Factory<T> {
		using Factory<T>::Make;
		FactoryWithBases();
		IExposable& Make( FactoryRequestBase& bReq, IExposable* parent ) override;
	private:
		template<class U>
		bool Integrate( FactoryRequestBase& bReq, std::unique_ptr<T>&& object, IExposable*& location );
	};
}

#include "Registrars.tpp"

inline RefRegistrarRoot<FactoryBase> FactoryBase::registrar;
#endif //QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_HPP
