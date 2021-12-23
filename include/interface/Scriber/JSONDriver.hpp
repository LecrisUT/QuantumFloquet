//
// Created by Le Minh Cristian on 2021/10/28.
//

#ifndef QUANFLOQ_JSONDRIVER_H
#define QUANFLOQ_JSONDRIVER_H

#include "ScribeDriver.hpp"
#include "Registrar/Registration.hpp"

namespace QuanFloq {
	// Predeclare classes dependant on implementation
	struct JSONIndex;
	struct JSONFile;

	class JSONDriver :
			public ScribeDriver {
	public:
		static PtrRegistration registration;
		std::shared_ptr<Scriber> GenScriber( ScribeState state, std::string_view filename ) override;
		std::string_view FileExtension() override;
		std::string_view GetName() const override;
		template<typename T>
		void ScribeValue( Scriber& base, std::string_view name, T& object, bool required );

		void Scribe( Scriber& base, std::string_view name, IExposable& object ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::unique_ptr<FactoryRequestBase>&& request, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             double& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::vector<double>& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::complex<double>& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::vector<std::complex<double>>& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             long& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::vector<long>& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name, size_t& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name, std::vector<size_t>& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::string& value, bool required ) override;
		void Scribe( Scriber& base, std::string_view name,
		             std::vector<std::string>& value, bool required ) override;
	};
	inline PtrRegistration JSONDriver::registration = PtrRegistration::Create<JSONDriver>();
}


#endif //QUANFLOQ_JSONDRIVER_H
