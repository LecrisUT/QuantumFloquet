//
// Created by Le Minh Cristian on 2021/09/28.
//


#ifndef QUANFLOQ_SCRIBER_HPP
#define QUANFLOQ_SCRIBER_HPP

#include "ScribeConcepts.hpp"
#include <span>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>

namespace QuanFloq {
	class ScribeDriver;
	class FactoryRequestBase;
	class DLibrary;

	struct ScriberFile {
		std::filesystem::path path;
		std::ifstream inStream;
		std::ofstream outStream;
		explicit ScriberFile( std::string_view filePath );
		virtual ~ScriberFile() = default;
	};
	struct ScriberIndex {
		std::shared_ptr<ScriberIndex> parent;
		std::shared_ptr<ScriberFile> file;
		std::string name;
		virtual ~ScriberIndex() = default;
		explicit ScriberIndex( std::shared_ptr<ScriberIndex> parent, std::string_view name = "" );
		template<class T = ScriberFile>
		requires std::derived_from<T, ScriberFile>
		explicit ScriberIndex( std::shared_ptr<T> scriberFile );
		[[nodiscard]] std::string GetCurrentPath( std::string_view Path, char delimiter = '/' ) const;
	};

	enum ScribeState : char {
		Load,
		Save,
	};
	class Scriber {
	public:
		IExposable* currentCaller;
		std::shared_ptr<ScriberIndex> index;
		ScribeState state;
		std::shared_ptr<ScribeDriver> driver;

	public:
		Scriber( ScribeState state, std::shared_ptr<ScribeDriver> driver, std::shared_ptr<ScriberIndex> index );
		void Scribe( std::string_view name, std::unique_ptr<FactoryRequestBase>&& request, bool required = false );
		template<Scribeable T>
		void Scribe( std::string_view name, T& value, bool required = false );
		template<Scribeable T>
		void Scribe( std::string_view name, std::vector<T>& value, bool required = false );

		template<Exposable T>
		void Scribe( std::string_view name, std::unique_ptr<T>& value, bool required = false );
		template<Exposable T>
		void Scribe( std::string_view name, std::vector<std::unique_ptr<T>>& value, bool required = false );
		template<StaticScribeable T>
		void Scribe( std::string_view name, std::shared_ptr<T>& value, bool required = false );
		template<StaticScribeable T>
		void Scribe( std::string_view name, std::vector<std::shared_ptr<T>>& values, bool required = false );
		template<Exposable T>
		void Scribe( std::string_view name, std::shared_ptr<T>& value, bool required = false, bool saveData = false );
		template<Exposable T>
		void Scribe( std::string_view name, std::vector<std::shared_ptr<T>>& value,
		             bool required = false, bool saveData = false );
		template<class T>
		void Scribe( std::string_view name, T& value, bool required = false );
	};
}


#endif //QUANFLOQ_SCRIBER_HPP
