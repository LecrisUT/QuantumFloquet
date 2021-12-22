//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_UTILITYCONCEPTS_HPP
#define QUANFLOQ_UTILITYCONCEPTS_HPP

#include <concepts>
#include <utility>

namespace QuanFloq {
	// Checker for specialization https://stackoverflow.com/a/31763111
	template<class T, template<class...> class Template>
	struct Is_specialization : std::false_type {
	};
	template<template<class...> class Template, class... Args>
	struct Is_specialization<Template<Args...>, Template> : std::true_type {
	};
	template<class This, template<class...> class Template>
	concept SpecializationOf = Is_specialization<This, Template>{};
}


#endif //QUANFLOQ_UTILITYCONCEPTS_HPP
