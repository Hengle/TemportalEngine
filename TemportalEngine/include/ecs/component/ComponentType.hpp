#ifndef TE_ECS_COMPONENT_TYPE_HPP
#define TE_ECS_COMPONENT_TYPE_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Libraries ------------------------------------------------------------------
#include <functional>

NS_ENGINE
class Engine;
NS_END

// ----------------------------------------------------------------------------
NS_ECS

struct ComponentType
{
	std::function<void*(engine::Engine *pEngine)> mfAllocateManager;
};

NS_END
// ----------------------------------------------------------------------------

#endif