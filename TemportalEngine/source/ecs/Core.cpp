#include "ecs\Core.hpp"

// Libraries ------------------------------------------------------------------
#include <string>

// Engine ---------------------------------------------------------------------
#include "Engine.hpp"
#include "ecs/ObjectManager.hpp"
#include "ecs/component/ComponentType.hpp"
#include "ecs/component/ComponentTransform.hpp"

// ----------------------------------------------------------------------------

using namespace ecs;

// ----------------------------------------------------------------------------

Core::Core(engine::Engine *pEngine)
{	
	mpEntityPool = pEngine->alloc<ObjectManager<Entity, ECS_MAX_ENTITY_COUNT>>([](Entity *obj) { return obj->getId(); });

	// TODO: Dispatch event to register all component types
	{
		bool didRegister;

		didRegister = RegisterComponent(mpComponentTypeRegistry, ComponentTransform);
		if (!didRegister)
		{
			// TODO: Could not register the transform type
		}
	}

	// Iterate through all component types and create managers for each using the memory manager
	uSize const typeCount = mpComponentTypeRegistry->getItemCount();
	for (uSize iCompType = 0; iCompType < typeCount; ++iCompType)
	{
		auto type = mpComponentTypeRegistry->at(iCompType);
		mpaComponentPools[iCompType] = nullptr;
		if (type.has_value())
		{
			mpaComponentPools[iCompType] = type.value().mfAllocateManager(pEngine);
		}
	}
	memset(mpaComponentPools + typeCount, 0, sizeof(void*) * (ECS_MAX_UNIQUE_COMPONENT_TYPES - typeCount));



}

Core::~Core()
{
}

void Core::updateTick(f32 const & deltaTime)
{
}

Entity* const Core::createEntity()
{
	return this->mpEntityPool->create();
}
