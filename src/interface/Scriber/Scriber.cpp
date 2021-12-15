//
// Created by lecris on 2021-11-10.
//

#include "Registrar/TypeInfo.hpp"
#include "interface/Scriber/Scriber.tpp"
#include "interface/Scriber/ScribeDriver.hpp"

using namespace QuanFloq;

// region Constructor/Destructor
ScriberFile::ScriberFile( std::string_view filePath ) :
		path(filePath) {
	auto status = std::filesystem::status(path);
	// TODO: Throw if input is not a file
	assert(status.type() == std::filesystem::file_type::regular);
//	// TODO: Throw if don't have read permission
//	assert(status.permissions() == std::filesystem::perms::owner_read);
	inStream = std::ifstream(path);
//	// TODO: Throw if don't have write permission
//	assert(status.permissions() == std::filesystem::perms::owner_write);
	// TODO: outStream should only be constructed conditionally
//	outStream = std::ofstream(path);
	// TODO: Need a proper throw
	assert(inStream.is_open());
}
ScriberIndex::ScriberIndex( std::shared_ptr<ScriberIndex> p, std::string_view name ) :
		parent{std::move(p)}, file{parent->file} {
	file = parent->file;
	if (!name.empty())
		parent->name = name;
}
Scriber::Scriber( ScribeState state, std::shared_ptr<ScribeDriver> driver, std::shared_ptr<ScriberIndex> index ) :
		state{state}, driver{std::move(driver)}, index{std::move(index)} { }
// endregion

std::string ScriberIndex::GetCurrentPath( std::string_view Path, char delimiter ) const {
	if (parent == nullptr || parent->file != file)
		return name + delimiter + std::string(Path);
	else
		return parent->GetCurrentPath(name + delimiter + std::string(Path), delimiter);
}


void Scriber::Scribe( std::string_view name, std::unique_ptr<FactoryRequestBase>&& request, bool required ) {
	driver->Scribe(*this, name, std::move(request), required);
}


template<>
void Scriber::Scribe( std::string_view name, std::shared_ptr<dynamic_library>& value, bool required ) {
	std::string Location;
	if (state == Load) {
		Scribe(name, Location, required);
		value = dynamic_library::registrar[Location];
		if (value == nullptr)
			value = dynamic_library::Create(Location);
	} else {
		if (value != nullptr)
			Location = value->Location;
		Scribe(name, Location, required);
	}
}