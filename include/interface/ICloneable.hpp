//
// Created by lecris on 2021-12-15.
//

#ifndef QUANFLOQ_INCLUDE_INTERFACE_ICLONEABLE_HPP
#define QUANFLOQ_INCLUDE_INTERFACE_ICLONEABLE_HPP

#include <memory>
#include <concepts>

namespace QuanFloq {
	template<class Base>
	struct ICloneable {
		virtual std::unique_ptr<Base> Clone() const = 0;
		template<std::derived_from<Base> Derived>
		std::unique_ptr<Base> CloneThis( const Derived& item ) const {
			return std::make_unique<Derived>(item);
		}
	};
}

#endif //QUANFLOQ_INCLUDE_INTERFACE_ICLONEABLE_HPP
