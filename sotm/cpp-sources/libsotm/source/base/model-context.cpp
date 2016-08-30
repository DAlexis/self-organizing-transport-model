#include "sotm/base/model-context.hpp"

using namespace sotm;

void ModelContext::setNodePayloadFactory(std::unique_ptr<INodePayloadFactory> factory)
{
	m_nodePayloadFactory = std::move(factory);
}

void ModelContext::setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory> factory)
{
	m_linkPayloadFactory = std::move(factory);
}

NodePayloadBase* ModelContext::createNodePayload(Node* node)
{
	ASSERT(m_nodePayloadFactory != nullptr, "Node physical payload factory is not set!");
	return m_nodePayloadFactory->create(&payloadsRegister, node);
}

LinkPayloadBase* ModelContext::createLinkPayload(Link* link)
{
	ASSERT(m_linkPayloadFactory != nullptr, "Link physical payload factory is not set!");
	return m_linkPayloadFactory->create(&payloadsRegister, link);
}
