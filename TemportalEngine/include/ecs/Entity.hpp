#ifndef TE_ECS_ENTITY_HPP
#define TE_ECS_ENTITY_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "utility/Guid.hpp"
#include "types/integer.h"
#include "registry/RegistryItem.hpp"

// ----------------------------------------------------------------------------
NS_ECS

/**
* Container for linking components together.
*/
class TEMPORTALENGINE_API Entity
{

	// Properties -------------------------------------------------------------
private:

	/** The unique identifier per instance. */
	utility::Guid mId;

	// TODO: This needs the max component type count. Use constexpr function?
	/** The id of each attached component. Limited to 1 component of each type. */
	utility::Guid mIdComponents[1];

	/** If this entity has been marked for destruction. */
	bool mIsPendingDestruction;

	// Operations -------------------------------------------------------------
private:
	
	/** Initialize the entity with a unique identifier. */
	void initialize(utility::Guid const &id);

	/** Mark this entity for destruction by ECS. */
	void markForDestruction();

public:

	/** Create a null-entity (invalid) */
	Entity();

	utility::Guid const getId() const;

	/** Returns true if this entity has been marked for destruction. */
	bool const isPendingDestruction() const;
	
	// TODO: Move to cpp
	bool const hasComponent(RegistryIdentifier componentType) const { return mIdComponents[componentType.value()].isValid(); }

	// TODO: Move to cpp
	utility::Guid const& getComponentId(RegistryIdentifier const& componentType) const { return mIdComponents[componentType.value()]; }

	// TODO: Move to cpp
	bool const operator==(Entity const *other) const { return mId == other->getId(); }

};

NS_END
// ----------------------------------------------------------------------------

#endif