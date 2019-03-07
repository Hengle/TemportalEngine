#include "ecs/component/ComponentTransform.hpp"

using namespace ecs;

RegistryIdentifier ComponentTransform::Identification = std::nullopt;

ComponentTransform::ComponentTransform()
{
	mPosition = math::Vector3::ZERO;
	mRotation = math::QuaternionIdentity;
	mScale = { 1.0f, 1.0f, 1.0f };
}
