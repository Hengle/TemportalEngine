#ifndef TE_ECS_COMPONENT_TYPEREGISTRY_HPP
#define TE_ECS_COMPONENT_TYPEREGISTRY_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "ecs/EcsBudget.h"
#include "ecs/component/ComponentType.hpp"

// Macros ---------------------------------------------------------------------

#define RegisterComponent(pRegistry, type, createManager) \
pRegistry->registerType(type::getHumanReadableTypeIdentifier(), type::IndexIdentifier, { sizeof(type), createManager });

// ----------------------------------------------------------------------------
NS_ECS

class TEMPORTALENGINE_API ComponentTypeRegistry
{

private:

	uSize mComponentTypeCount;
	ComponentType mpaComponentTypes[ECS_MAX_UNIQUE_COMPONENT_TYPES];

public:
	ComponentTypeRegistry();

	bool const registerType(ui32 const &humanReadableTypeIdentifier, ComponentTypeIndex &typeIndex, ComponentType const &type);

	uSize const getTypeCount() const;

	ComponentType const & at(uSize const &i) const;

};

NS_END
// ----------------------------------------------------------------------------

#endif