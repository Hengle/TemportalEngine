#ifndef TE_ECS_COMPONENT_TRANSFORM_HPP
#define TE_ECS_COMPONENT_TRANSFORM_HPP
#pragma warning(push)
#pragma warning(disable:4251) // disable STL warnings in dll

// Parents --------------------------------------------------------------------
#include "ecs/component/Component.hpp"

// Engine ---------------------------------------------------------------------
#include "math/Vector.hpp"

// ----------------------------------------------------------------------------
NS_ECS

/**
* Generic/base class for all components.
*/
struct TEMPORTALENGINE_API ComponentTransform : public Component
{
	// TPRS - Transform Position Rotation Scale
	GENERATE_IDENTIFICATION('TPRS')

public:

	math::Vector3 mPosition;
	math::Quaternion mRotation;
	math::Vector3 mScale;

	ComponentTransform();

};

NS_END
// ----------------------------------------------------------------------------

#pragma warning(pop)
#endif