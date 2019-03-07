#ifndef TE_ECS_CORE_HPP
#define TE_ECS_CORE_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

#include <cassert>

// Engine ---------------------------------------------------------------------
#include "ecs/Entity.hpp"
#include "ecs/component/ComponentTypeRegistry.hpp"
#include "types/real.h"

NS_ENGINE
class Engine;
NS_END

// ----------------------------------------------------------------------------
NS_ECS

template <class TObject, uSize TMaxCount>
class ObjectManager;

/**
* Central structure which tracks all entities, components, and systems.
*/
class TEMPORTALENGINE_API Core
{
private:

	ObjectManager<Entity, ECS_MAX_ENTITY_COUNT> *mpEntityPool;

	ComponentTypeRegistry mpComponentTypeRegistry[1];
	void* mpaComponentPools[ECS_MAX_UNIQUE_COMPONENT_TYPES];

public:
	Core(engine::Engine *pEngine);
	~Core();

	void updateTick(f32 const &deltaTime);

	Entity* const createEntity();

	template <class TComponent>
	TComponent* createComponent()
	{
		assert(TComponent::Identification.has_value());
		auto pool = (ObjectManager<TComponent, ECS_MAX_ENTITY_COUNT>*)mpaComponentPools[TComponent::Identification.value()];
		return pool->create();
	}

	template <class TComponent>
	void destroyComponent(TComponent* &obj)
	{
		assert(TComponent::Identification.has_value());
		auto pool = (ObjectManager<TComponent, ECS_MAX_ENTITY_COUNT>*)mpaComponentPools[TComponent::Identification.value()];
		pool->destroy(obj);
	}
	
};

NS_END
// ----------------------------------------------------------------------------

#endif