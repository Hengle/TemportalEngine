#ifndef TE_ECS_COMPONENT_HPP
#define TE_ECS_COMPONENT_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "utility/Guid.hpp"

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

//uSize ComponentTypeCount;
//uSize const ComponentSize[1];
//
//template <typename TComponent>
//constexpr bool const registerComponent(uSize &idOut)
//{
//	static_assert(
//		std::is_base_of<Component, TComponent>::value
//		"TComponent must derive from ecs::Component."
//	);
//
//	sizeof(TComponent);
//
//	return false;
//}

NS_END
// ----------------------------------------------------------------------------

#endif