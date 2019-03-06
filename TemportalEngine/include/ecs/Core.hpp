#ifndef TE_ECS_CORE_HPP
#define TE_ECS_CORE_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "ecs/Entity.hpp"
#include "ecs/component/ComponentTypeRegistry.hpp"
#include "types/real.h"

// ----------------------------------------------------------------------------
NS_ECS

/**
* Central structure which tracks all entities, components, and systems.
*/
class TEMPORTALENGINE_API Core
{
private:

	ComponentTypeRegistry mpComponentTypeRegistry[1];
	void* mpComponentPools[ECS_MAX_UNIQUE_COMPONENT_TYPES];

public:
	Core();
	~Core();

	void updateTick(f32 const &deltaTime);

	Entity* createEntity();
	
};

NS_END
// ----------------------------------------------------------------------------

#endif