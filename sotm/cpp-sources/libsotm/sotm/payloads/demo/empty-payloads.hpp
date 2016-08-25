/*
 * empty-payloads.hpp
 *
 *  Created on: 25 авг. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_DEMO_EMPTY_PAYLOADS_HPP_
#define LIBSOTM_SOTM_PAYLOADS_DEMO_EMPTY_PAYLOADS_HPP_


#include "sotm/base/physical-payload.hpp"

namespace sotm
{

class EmptyNodePayload : public NodePayloadBase
{
public:
	EmptyNodePayload(PhysicalPayloadsRegister* reg, Node* node);

	void makeStep(double dt) override;
};

class EmptyLinkPayload : public LinkPayloadBase
{
public:
	EmptyLinkPayload(PhysicalPayloadsRegister* reg, Link* link);

	void makeStep(double dt) override;
};



}

#endif /* LIBSOTM_SOTM_PAYLOADS_DEMO_EMPTY_PAYLOADS_HPP_ */
