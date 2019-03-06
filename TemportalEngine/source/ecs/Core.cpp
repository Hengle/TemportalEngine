#include "ecs\Core.hpp"
#include <string>
#include "ecs/component/ComponentTransform.hpp"
#include "memory/ObjectPool.hpp"

using namespace ecs;

Core::Core()
{	
	// TODO: Dispatch event to register all component types
	{
		bool didRegister;

		didRegister = RegisterComponent(mpComponentTypeRegistry, ComponentTransform, nullptr);
		if (!didRegister)
		{
			// TODO: Could not register the transform type
		}
	}

	// TODO: Iterate through all component types and create managers for each using the memory manager
	uSize const typeCount = mpComponentTypeRegistry->getTypeCount();
	for (uSize iCompType = 0; iCompType < typeCount; ++iCompType)
	{
		ComponentType const &type = mpComponentTypeRegistry->at(iCompType);
		mpComponentPools[iCompType] = type.mpfCreateManager();
	}
	memset(mpComponentPools + typeCount, 0, sizeof(void*) * (ECS_MAX_UNIQUE_COMPONENT_TYPES - typeCount));

	memory::ObjectPool<ComponentTransform, 2> tmp;

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
