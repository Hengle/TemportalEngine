#ifndef TE_ECS_COMPONENT_TYPEREGISTRY_HPP
#define TE_ECS_COMPONENT_TYPEREGISTRY_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "ecs/EcsBudget.h"
#include "ecs/component/ComponentType.hpp"

// Macros ---------------------------------------------------------------------

#define RegisterComponent(pRegistry, type) \
pRegistry->registerType<type>(type::getHumanReadableTypeIdentifier(), type::IndexIdentifier);

// ----------------------------------------------------------------------------
NS_ECS

class TEMPORTALENGINE_API ComponentTypeRegistry
{

private:

	/*
	TODO: Create a TypeInfo structure which stores:
	- the size of a component?
	- a way to create the component manager / object pool
	*/
	struct TypeInfo
	{
		uSize sizeInBytes;
	};

	TypeInfo maTypeInfo[ECS_MAX_UNIQUE_COMPONENT_TYPES];

	/*
	TODO: Member function to register a component type
	- takes the data type of the component
	- takes a human readable type identifier (4 char?)
	- outputs the numerical/index type identifier
	- returns success flag boolean
	- fails if the numerical index passed is already populated
	- populates an array of TypeInfo using the numerical type index
	*/

public:
	ComponentTypeRegistry();

	template <class TComponent>
	bool registerType(ui32 const &humanReadableTypeIdentifier, ComponentTypeIndex &typeIndex)
	{
		// TODO: Log that the component failed to register
		return false;
	}

};

NS_END
// ----------------------------------------------------------------------------

#endif