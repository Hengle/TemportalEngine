#include "ecs/system/SystemPhysics.hpp"

#include "ecs/Core.hpp"
#include "ecs/component/ComponentTransform.hpp"

using namespace ecs;

SystemPhysics::SystemPhysics()
	: System(
		{ {ComponentTransform::Identification, true} },
		{}
	)
{}

void SystemPhysics::updateComponents(Core *pCore, ComponentIds const & ids)
{
	// TODO: getting a component should return a constant component if it has not been marked as writable
	std::optional<ComponentTransform*> transformOpt = pCore->getComponent<ComponentTransform>(ids[0]);
	assert(transformOpt.has_value());

	ComponentTransform* transform = transformOpt.value();

	// TODO: Mark transform as writing

	// TODO: Do physics

	// TODO: Unlock the transform for writing

}
