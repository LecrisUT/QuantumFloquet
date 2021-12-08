//
// Created by lecris on 2021-11-15.
//

#ifndef QUANFLOQ_UTILITYCONCEPTS_HPP
#define QUANFLOQ_UTILITYCONCEPTS_HPP

#include <concepts>
#include <utility>

namespace QuanFloq {
	template<class T, class ...Args>
	concept publicCtrArgs = requires( Args&& ... args ) {
		T(std::forward<Args>(args)...);
	};
	template<class T, class ...Args>
	concept createCtrArgs = requires( Args&& ... args ) {
		T::Create(std::forward<Args>(args)...);
	};
	template<class T, class ...Args>
	concept ctrArgs = publicCtrArgs<T, Args...> || createCtrArgs<T, Args...>;
}


#endif //QUANFLOQ_UTILITYCONCEPTS_HPP
