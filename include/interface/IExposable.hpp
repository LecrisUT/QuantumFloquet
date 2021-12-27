//
// Created by Le Minh Cristian on 2021/09/27.
//

#ifndef QUANFLOQ_IEXPOSABLE_HPP
#define QUANFLOQ_IEXPOSABLE_HPP

#include <memory>
#include <concepts>
#include <cassert>
#include <set>

namespace QuanFloq {
//	Predeclare dependents
	class ScribeDriver;
	class Scriber;
	struct TypeInfo;

	class IExposable {
	public:
		std::shared_ptr<ScribeDriver> format;

	public:
		IExposable();
		explicit IExposable( std::string_view format );
	public:
		virtual void ExposeData( Scriber& scriber );
		virtual std::string_view GetName() const;
		virtual const TypeInfo& GetType() const;
		virtual void RegisterName( std::string_view name = "" );
	};
}


#endif //QUANFLOQ_IEXPOSABLE_HPP
