#include "ecs/component/ComponentTypeRegistry.hpp"

using namespace ecs;

ComponentTypeRegistry::ComponentTypeRegistry()
	: mComponentTypeCount(0)
{
}

bool const ComponentTypeRegistry::registerType(ui32 const & humanReadableTypeIdentifier, ComponentTypeIndex & typeIndex, ComponentType const & type)
{
	if (typeIndex >= 0)
	{
		// TODO: Log error, duplicate registration
		return false;
	}

	// TODO: Check if the HRTI has already been used to register something

	// Save the component type information
	typeIndex = mComponentTypeCount++;
	mpaComponentTypes[typeIndex] = type;
	return true;
}

uSize const ComponentTypeRegistry::getTypeCount() const
{
	return mComponentTypeCount;
}

ComponentType const & ComponentTypeRegistry::at(uSize const &i) const
{
	return mpaComponentTypes[i];
}
