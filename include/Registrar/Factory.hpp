//
// Created by lecris on 2021-12-13.
//

#ifndef QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_HPP
#define QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_HPP

#include "RegistrarConcepts.hpp"
#include "RefRegistrar.hpp"
#include <exception>

namespace QuanFloq {
	// Predeclare superclasses
	class IExposable;
	class FactoryRequestBase;

	struct BaseFactory {
		using registrar_type = RefRegistrarRoot<BaseFactory>;
		std::string name;
		static registrar_type registrar;
		virtual IExposable& Make( FactoryRequestBase& bReq, IExposable* parent ) = 0;
		virtual std::string_view GetName() const;
	};
	inline BaseFactory::registrar_type BaseFactory::registrar;

	template<class T>
	struct Factory :
			BaseFactory {
		virtual std::unique_ptr<T> Make( IExposable* parent );
		virtual std::shared_ptr<T> MakeShared( IExposable* parent );
		template<class ...Args>
		requires std::constructible_from<T, Args...>
		std::unique_ptr<T> MakeUnique( Args&& ... args );
		IExposable& Make( FactoryRequestBase& bReq, IExposable* parent ) override;
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
		template<class U>
		bool IntegrateShared( FactoryRequestBase& bReq, std::shared_ptr<T> object, IExposable*& location );
	};
}

#endif //QUANFLOQ_INCLUDE_REGISTRAR_FACTORY_HPP
