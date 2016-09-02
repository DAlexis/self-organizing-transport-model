/*
 * global-context.hpp
 *
 *  Created on: 2 июня 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSTEPMOD_STEPMOD_BASE_GLOBAL_CONTEXT_HPP_
#define LIBSTEPMOD_STEPMOD_BASE_GLOBAL_CONTEXT_HPP_

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/time-iter.hpp"
#include "physical-payload.hpp"
#include <memory>

namespace sotm
{

class ModelContext
{
public:
	void setNodePayloadFactory(std::unique_ptr<INodePayloadFactory> factory);
	void setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory> factory);

	NodePayloadBase* createNodePayload(Node* node);
	LinkPayloadBase* createLinkPayload(Link* node);

	void doBranchingIteration(double dt);

	PhysicalPayloadsRegister payloadsRegister;
	GraphRegister graphRegister;

private:
	void branchIteration(double dt, Node* node);
	std::unique_ptr<INodePayloadFactory> m_nodePayloadFactory;
	std::unique_ptr<ILinkPayloadFactory> m_linkPayloadFactory;
};

}







#endif /* LIBSTEPMOD_STEPMOD_BASE_GLOBAL_CONTEXT_HPP_ */
