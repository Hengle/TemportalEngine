#include "ecs\Core.hpp"
#include <string>
#include "ecs/component/ComponentTransform.hpp"

using namespace ecs;

Core::Core()
{
	memset(mpComponentPools, 0, sizeof(void*) * ECS_MAX_UNIQUE_COMPONENT_TYPES);
	
	// TODO: Dispatch event to register all component types
	RegisterComponent(mpComponentTypeRegistry, ComponentTransform);

	// TODO: Iterate through all component types and create managers for each using the memory manager

}

Core::~Core()
{
}

void Core::updateTick(f32 const & deltaTime)
{
}

Entity * Core::createEntity()
{
	return nullptr;
}
