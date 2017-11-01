/*
 * electrostatic-scaler.hpp
 *
 *  Created on: 16 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SCALER_HPP_
#define LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SCALER_HPP_

#include "sotm/payloads/electrostatics/electrostatics.hpp"

namespace sotm {

class ElectrostaticScaler
{
public:
	ElectrostaticScaler(ElectrostaticPhysicalContext* context);

	void scaleStep(double factor);

private:
	ElectrostaticPhysicalContext* m_context;
};

} // namespace sotm



#endif /* LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SCALER_HPP_ */
