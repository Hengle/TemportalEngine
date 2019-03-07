#ifndef TE_ECS_COMPONENT_HPP
#define TE_ECS_COMPONENT_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Parents --------------------------------------------------------------------
#include "registry/RegistryItem.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "utility/Guid.hpp"
#include "ecs/component/ComponentType.hpp"

// ----------------------------------------------------------------------------
NS_ECS

/**
* Generic/base class for all components.
*/
struct TEMPORTALENGINE_API Component
{

	/** The unique identifier for an instance of this component. */
	utility::Guid mId;

	/** The unique identifier of the entity instance this component is attached to. */
	utility::Guid mIdEntity;

};

NS_END
// ----------------------------------------------------------------------------

#endif