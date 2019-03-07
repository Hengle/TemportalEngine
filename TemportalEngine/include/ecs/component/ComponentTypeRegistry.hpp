#ifndef TE_ECS_COMPONENT_TYPEREGISTRY_HPP
#define TE_ECS_COMPONENT_TYPEREGISTRY_HPP
#pragma warning(push)
#pragma warning(disable:4251) // disable STL warnings in dll

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "ecs/EcsBudget.h"
#include "ecs/component/ComponentType.hpp"
#include "registry/Registry.hpp"

// Macros ---------------------------------------------------------------------

#define RegisterComponent(pRegistry, type) \
	pRegistry->registerType(type::Identification, { \
		[](engine::Engine *pEngine) \
		{ \
			return pEngine->alloc<ecs::ObjectManager<type, ECS_MAX_ENTITY_COUNT>>([](type *comp) { return comp->mId; }); \
		} \
	});

// ----------------------------------------------------------------------------
NS_ECS

class TEMPORTALENGINE_API ComponentTypeRegistry : public utility::Registry<ComponentType, ECS_MAX_UNIQUE_COMPONENT_TYPES>
{

public:
	ComponentTypeRegistry();

};

NS_END
// ----------------------------------------------------------------------------

#pragma warning(pop)
#endif