//
// Created by lecris on 2021-11-12.
//

#ifndef QUANFLOQ_SCRIBER_TPP
#define QUANFLOQ_SCRIBER_TPP

#include "Scriber.hpp"
#include "ScribeDriver.hpp"
#include "Registrar/TypeInfo.hpp"
#include "Registrar/Registrar.hpp"
#include "FactoryRequest.tpp"

using namespace QuanFloq;

// region Constructor/Destructor
template<class T>
requires std::derived_from<T, ScriberFile>
ScriberIndex::ScriberIndex( std::shared_ptr<T> scriberFile ) :
		file{std::move(scriberFile)} { }
// endregion

// region Value types
template<Scribeable T>
void Scriber::Scribe( std::string_view name, T& value, bool required ) {
	typename Scribe_traits<T>::equivalent_type tempValue = value;
	driver->Scribe(*this, name, tempValue, required);
	if (state == Load)
		value = tempValue;
}
template<Scribeable T>
void Scriber::Scribe( std::string_view name, std::vector<T>& value, bool required ) {
	std::vector<typename Scribe_traits<T>::equivalent_type> tempValue;
	if (state == Save)
		for (typename Scribe_traits<T>::equivalent_type val: value)
			tempValue.push_back(val);
	driver->Scribe(*this, name, tempValue, required);
	if (state == Load)
		for (auto& val: tempValue)
			value.push_back(val);
}
// endregion
template<Exposable T>
void Scriber::Scribe( std::string_view name, std::unique_ptr<T>& value, bool required ) {
	Scribe(name, std::make_unique<FactoryRequest<std::unique_ptr, T>>(value), required);
}
template<Exposable T>
void Scriber::Scribe( std::string_view name, std::vector<std::unique_ptr<T>>& value, bool required ) {
	Scribe(name, std::make_unique<FactoryRequest<std::unique_ptr, T>>(value), required);
}

template<StaticScribeable T>
void Scriber::Scribe( std::string_view name, std::shared_ptr<T>& value, bool required ) {
	std::string Name;
	if (state == Load) {
		Scribe(name, Name, required);
		if (!Name.empty())
			T::registrar.Get(Name, value, true);
	} else {
		assert(value != nullptr);
		Name = value->GetName();
		Scribe(name, Name, required);
	}
}
template<StaticScribeable T>
void Scriber::Scribe( std::string_view name, std::vector<std::shared_ptr<T>>& values, bool required ) {
	std::vector<std::string> Names;
	if (state == Load) {
		Scribe(name, Names, required);
		for (std::string_view Name: Names)
			T::registrar.Get(Name, values, true);
	} else {
		for (auto& val: values)
			Names.emplace_back(val->GetName());
		Scribe(name, Names, required);
	}
}
template<Exposable T>
void Scriber::Scribe( std::string_view name, std::shared_ptr<T>& value, bool required, bool saveData ) {
	Scribe(name, std::make_unique<FactoryRequest<std::shared_ptr, T>>(value, saveData), required);
}
template<Exposable T>
void Scriber::Scribe( std::string_view name, std::vector<std::shared_ptr<T>>& value, bool required, bool saveData ) {
	Scribe(name, std::make_unique<FactoryRequest<std::shared_ptr, T>>(value, saveData), required);
}

template<>
void Scriber::Scribe( std::string_view name, std::shared_ptr<dynamic_library>& value, bool required );

#endif //QUANFLOQ_SCRIBER_TPP
