//
// Created by Le Minh Cristian on 2021/10/28.
//

#include "interface/Scriber/JSONDriver.hpp"
#include "interface/Scriber/Scriber.tpp"
#include "Registrar/FactoryRequest.hpp"
#include "interface/IExposable.hpp"
#include "Registrar/TypeInfo.tpp"
#include "Registrar/Factory.hpp"
#include "Thing/Thing.hpp"
#include "Registrar/SharedRegistrar.tpp"
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <scn/scn.h>

using namespace QuanFloq;
using JSON = nlohmann::json;


std::string_view JSONDriver::FileExtension() {
	return ".json";
}
std::string_view JSONDriver::GetName() const {
	return "JSON Driver";
}

namespace QuanFloq {
	struct JSONFile :
			ScriberFile {
		JSON jsonRoot;
		explicit JSONFile( std::string_view filePath ) :
				ScriberFile{filePath} {
			// TODO: Warning if not a proper extension
			assert(path.extension() == ".json");
			inStream >> jsonRoot;
			assert(!jsonRoot.empty());
		}
	};
	struct JSONIndex :
			ScriberIndex {
		JSONFile* jsonFile;
		JSON& json;
		JSONIndex( std::shared_ptr<ScriberIndex> parent, std::string_view name, int ind = -1 ) :
				ScriberIndex{std::move(parent), name}, jsonFile{dynamic_cast<JSONFile*>(file.get())},
				json{GetJsonRef(name, ind)} { }
		JSONIndex( std::shared_ptr<ScriberIndex> parent, std::string_view name, JSON& j ) :
				ScriberIndex{std::move(parent), name}, jsonFile{dynamic_cast<JSONFile*>(file.get())}, json{j} { }
		// TODO: Should read JSON object from file at construction.
		explicit JSONIndex( std::string_view filename ) :
				ScriberIndex{std::make_shared<JSONFile>(filename)}, jsonFile{dynamic_cast<JSONFile*>(file.get())},
				json{jsonFile->jsonRoot} { }
		JSON& GetJsonRef( std::string_view name, int ind ) {
			auto p = dynamic_cast<JSONIndex*>(parent.get());
			assert(p != nullptr);
			if (name.empty())
				return ind < 0 ? p->json : p->json[ind];
			assert(p->json.contains(name));
			return ind < 0 ? p->json[std::string(name)] : p->json[std::string(name)][ind];
		}
	};
}

std::shared_ptr<Scriber> JSONDriver::GenScriber( ScribeState state, std::string_view filename ) {
	return std::make_shared<Scriber>(state, std::static_pointer_cast<JSONDriver>(registration.pointer),
	                                 std::make_shared<JSONIndex>(filename));
}

template<typename T>
void JSONDriver::ScribeValue( Scriber& base, std::string_view name, T& object, bool required ) {
	auto index = dynamic_cast<JSONIndex*>(base.index.get());
	assert(index != nullptr);
	if (base.state == Save)
		// TODO: Temporary fix until nlohmann supports std::string_view (gh://nlohmann/json#1529)
		index->json[std::string(name)] = object;
	else {
		if (index->json.contains(name))
			index->json[std::string(name)].get_to(object);
		else
			assert(!required);
	}
}

namespace nlohmann {
	template<>
	struct adl_serializer<std::complex<double>> {
		static std::complex<double> from_json( const json& j ) {
			auto string = j.get<std::string>();
			double real, imag;
			scn::scan(string, "({},{})", real, imag);
			return {real, imag};
		}
		static void from_json( const json& j, std::complex<double>& val ) {
			auto string = j.get<std::string>();
			double real, imag;
			scn::scan(string, "({},{})", real, imag);
			val = {real, imag};
		}
		static void to_json( json& j, const std::complex<double>& val ) {
			j = fmt::format("({},{})", val.real(), val.imag());
		}
	};
}


void JSONDriver::Scribe( Scriber& base, std::string_view name, IExposable& object ) {
	auto index = std::dynamic_pointer_cast<JSONIndex>(base.index);
	assert(index != nullptr);
	IExposable* OtherCaller = &object;
	std::swap(base.currentCaller, OtherCaller);
	if (object.format == "JSON Driver") {
		// Make a new indexer that contains the JSON object.
		auto cptr = static_pointer_cast<ScriberIndex>(std::make_shared<JSONIndex>(index, name));
		base.index.swap(cptr);
		object.ExposeData(base);
		base.index.swap(cptr);
	} else {
		// Have to switch to child's driver
		std::string filename;
		std::shared_ptr<Scriber> child;
		if (base.state == Save) {
			filename = name;
			filename += object.format->FileExtension();
			filename = index->GetCurrentPath(filename, '_');
			if (name.empty())
				index->json["Filename"] = filename;
			else
				index->json[std::string(name)]["Filename"] = filename;
		} else {
			if (name.empty())
				filename = index->json["Filename"].get<std::string>();
			else
				filename = index->json[std::string(name)]["Filename"].get<std::string>();
		}
		child = object.format->GenScriber(base.state, filename);
		object.ExposeData(*child);
	}
	std::swap(base.currentCaller, OtherCaller);
}


inline void ScribeHelper( JSONDriver* driver, nlohmann::json& j, Scriber& base,
                          std::string_view name, std::unique_ptr<FactoryRequestBase>&& request ) {
	// Helper function for loading an IExposable
	assert(j.is_object());
	if (request->sharedType && j.contains("Pointer")) {
		// Load from shared pointer registrar on any shareable type
		assert(j.contains("Type"));
		auto ptr = j["Pointer"].get<std::string_view>();
		auto typeName = j["Type"].get<std::string_view>();
		assert(!typeName.empty());
		assert(TypeInfo::registrar.Contains(typeName));
		auto& type = TypeInfo::registrar[typeName];
		auto iregistrar = type.iRegistrar;
		assert(iregistrar != nullptr);
		QuanFloq::JSONDriver::Register(*iregistrar, ptr, std::move(request));
		// No need to call Scribe on the IExposable because it is handled by its constructor
	} else if (j.contains("Def")) {
		// Special constructors for Things and Comps using their Defs
		auto defName = j["Def"].get<std::string_view>();
		if (BaseDef::registrar.Contains(defName)) {
			auto thingDef = BaseDef::registrar[defName];
			auto& obj = thingDef->thingFactory.Make(*request, base.currentCaller);
			driver->Scribe(base, name, obj);
		} else if (BaseCompDef::registrar.Contains(defName)) {
			auto compDef = BaseCompDef::registrar[defName];
			auto& obj = compDef->compFactory.Make(*request, base.currentCaller);
			driver->Scribe(base, name, obj);
		} else
			throw std::runtime_error("Def not loaded");
	} else {
		// Other IExposable constructor Factories derived from their TypeInfo
		assert(j.contains("Type"));
		assert(!j.contains("Pointer"));
		auto typeName = j["Type"].get<std::string_view>();
		assert(TypeInfo::registrar.Contains(typeName));
		auto type = TypeInfo::registrar[typeName];
		auto factory = type.factory;
		assert(factory != nullptr);
		auto& obj = factory->Make(*request, base.currentCaller);
		driver->Scribe(base, name, obj);
	}
}
inline void ScribeHelper( JSONDriver* driver, nlohmann::json& j, Scriber& base,
                          std::string_view name, FactoryRequestBase& request, IExposable& object ) {
	// Helper function for saving IExposable
	BaseThing* thing;
	BaseComp* comp;
	if ((thing = dynamic_cast<BaseThing*>(&object)) != nullptr) {
		j["Def"] = thing->def->GetName();
	} else if ((comp = dynamic_cast<BaseComp*>(&object)) != nullptr) {
		j["Def"] = comp->def->GetName();
	}
	auto type = object.GetType();
	j["Type"] = type.name;
	if (request.sharedType && !request.scribeData)
		// Only save the pointer
		j["Pointer"] = object.GetName();
	else
		// Save object's data
		driver->Scribe(base, name, object);
}

void JSONDriver::Scribe( Scriber& base, std::string_view name, std::unique_ptr<FactoryRequestBase>&& request,
                         bool required ) {
	auto index = std::dynamic_pointer_cast<JSONIndex>(base.index);
	assert(index != nullptr);
	if (base.state == Load) {
		if (!name.empty() && !index->json.contains(name)) {
			if (required)
				assert(false);
			return;
		}
		// If no name is given assume the file contains the whole object
		auto& j = name.empty() ? index->json : index->json[std::string(name)];
		if (j.is_array()) {
			assert(request->arrayType);
			for (int ind = 0; auto& jj: j) {
				// Replace the original json index with the one in jj
				auto currName = fmt::format("{}_{}", name, ind++);
				auto cptr = std::static_pointer_cast<ScriberIndex>(std::make_shared<JSONIndex>(index, currName, jj));
				base.index.swap(cptr);
				ScribeHelper(this, jj, base, "", std::move(request->Clone()));
				// Replace back the original json index
				base.index.swap(cptr);
			}
		} else
			ScribeHelper(this, j, base, name, std::move(request));
	} else {
		auto& j = index->json[std::string(name)];
		if (request->arrayType) {
			j = nlohmann::json::array();
			for (int ind = 0; IExposable& object: request->values) {
				auto& jj = j[ind++];
				auto currName = fmt::format("{}_{}", name, ind);
				auto cptr = std::static_pointer_cast<ScriberIndex>(std::make_shared<JSONIndex>(index, currName, jj));
				base.index.swap(cptr);
				ScribeHelper(this, jj, base, "", *request, object);
				base.index.swap(cptr);
			}
		} else {
			assert(request->values.size() <= 1);
			j = nullptr;
			if (!request->values.empty())
				ScribeHelper(this, j, base, name, *request, request->values[0]);
		}
	}
}

void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         double& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         std::vector<double>& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         std::complex<double>& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         std::vector<std::complex<double>>& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         long& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         std::vector<long>& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name, size_t& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name, std::vector<size_t>& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         std::string& value, bool required ) {
	ScribeValue(base, name, value, false);
}
void JSONDriver::Scribe( Scriber& base, std::string_view name,
                         std::vector<std::string>& value, bool required ) {
	ScribeValue(base, name, value, false);
}
