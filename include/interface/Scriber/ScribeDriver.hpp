//
// Created by Le Minh Cristian on 2021/09/28.
//

#ifndef QUANFLOQ_ISCRIBEDRIVER_H
#define QUANFLOQ_ISCRIBEDRIVER_H

#include "Thing/ThingDriver.hpp"
#include "ScribeConcepts.hpp"
#include <memory>

namespace QuanFloq {
	// Forward Declarations
	struct FactoryRequestBase;
	enum ScribeState : char;

	// TODO: add concept scribeable
	class ScribeDriver :
			public BaseDriver {
	public:
		using registrar_type = SharedRegistrar<ScribeDriver, BaseDriver>;
	protected:
		static IRegistrationTask AwaitGet( IRegistrar&, std::string_view,
		                                   std::unique_ptr<FactoryRequestBase>&& request );
	public:
		static void Register( IRegistrar& iRegistrar, std::string_view name,
		                      std::unique_ptr<FactoryRequestBase>&& request, bool await = true );

		static registrar_type registrar;
		virtual std::shared_ptr<Scriber> GenScriber( ScribeState state, std::string_view filename ) = 0;
		virtual std::string_view FileExtension() = 0;

		/**
		 * Scribe data within the object
		 * @param base
		 * @param name
		 * @param object
		 */
		virtual void Scribe( Scriber& base, std::string_view name, IExposable& object ) = 0;
		/**
		 * Generate IExposable object for the request from the Def or TypeInfo::Generator.
		 * If request is SharedType, use co_await and Registrar.RegistrationQueue to ensure
		 * the reference is resolved when needed.
		 * @see Scriber::ScribeHelper
		 */
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::unique_ptr<FactoryRequestBase>&& request, bool required ) = 0;

		virtual void Scribe( Scriber& base, std::string_view name,
		                     double& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::vector<double>& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::complex<double>& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::vector<std::complex<double>>& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     long& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::vector<long>& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     size_t& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::vector<size_t>& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::string& value, bool required ) = 0;
		virtual void Scribe( Scriber& base, std::string_view name,
		                     std::vector<std::string>& value, bool required ) = 0;
	};
	inline ScribeDriver::registrar_type ScribeDriver::registrar;
}


#endif //QUANFLOQ_ISCRIBEDRIVER_H
