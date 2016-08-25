/*
 * global-context.hpp
 *
 *  Created on: 2 июня 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSTEPMOD_STEPMOD_BASE_GLOBAL_CONTEXT_HPP_
#define LIBSTEPMOD_STEPMOD_BASE_GLOBAL_CONTEXT_HPP_

#include "physical-payload.hpp"
#include "stepping-structure.hpp"

namespace sotm
{

class ModelContext
{
public:
	INodePhysPayload* createNodePayload(Node* node);
	ILinkPhysPayload* createLinkPayload(Link* node);

};

}







#endif /* LIBSTEPMOD_STEPMOD_BASE_GLOBAL_CONTEXT_HPP_ */
