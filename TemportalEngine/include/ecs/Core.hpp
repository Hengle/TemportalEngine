#ifndef TE_ECS_CORE_HPP
#define TE_ECS_CORE_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

#include <cassert>
#include <optional>

// Engine ---------------------------------------------------------------------
#include "ecs/Entity.hpp"
#include "ecs/component/ComponentTypeRegistry.hpp"
#include "types/real.h"
#include "ecs/ObjectManager.hpp"

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
public:
	typedef ObjectManager<Entity, ECS_MAX_ENTITY_COUNT> EntityManager;

private:

	EntityManager *mpEntityPool;

	ComponentTypeRegistry mpComponentTypeRegistry[1];
	void* mpaComponentPools[ECS_MAX_UNIQUE_COMPONENT_TYPES];

public:
	Core(engine::Engine *pEngine);
	~Core();

	void updateTick(f32 const &deltaTime);

	Entity* const createEntity();
	EntityManager* getEntityManager() const;

	template <class TComponent>
	std::optional<TComponent*> getComponent(utility::Guid const &id) const
	{
		assert(TComponent::Identification.has_value());
		ObjectManager<TComponent, ECS_MAX_ENTITY_COUNT>* manager =
			(ObjectManager<TComponent, ECS_MAX_ENTITY_COUNT>*)mpaComponentPools[TComponent::Identification.value()];
		return manager->getObjectWithId(id);
	}

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