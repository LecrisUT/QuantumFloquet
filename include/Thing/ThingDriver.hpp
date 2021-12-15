//
// Created by Le Minh Cristian on 2021/10/13.
//

#ifndef QUANFLOQ_THINGDRIVER_HPP
#define QUANFLOQ_THINGDRIVER_HPP

#include "Registrar/Registrars.tpp"

namespace QuanFloq {
	class BaseDriver {
	public:
		static SharedRegistrarRoot<BaseDriver> registrar;
		virtual ~BaseDriver() = default;
		virtual std::string_view GetName() const = 0;
	};
	inline SharedRegistrarRoot<BaseDriver> BaseDriver::registrar;
}

#endif //QUANFLOQ_THINGDRIVER_HPP
