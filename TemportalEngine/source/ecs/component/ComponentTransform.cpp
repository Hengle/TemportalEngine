#include "ecs/component/ComponentTransform.hpp"

using namespace ecs;

ComponentTypeIndex ComponentTransform::IndexIdentifier = -1;

ComponentTransform::ComponentTransform()
{
	mPosition = math::Vector3::ZERO;
	mRotation = math::QuaternionIdentity;
	mScale = { 1, 1, 1 };
}
