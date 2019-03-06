#ifndef TE_ECS_ENTITY_HPP
#define TE_ECS_ENTITY_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "utility/Guid.hpp"

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

	/** Create a null-entity (invalid) */
	Entity();
	
	/** Initialize the entity with a unique identifier. */
	void initialize(utility::Guid const &id);

	/** Mark this entity for destruction by ECS. */
	void markForDestruction();

public:

	utility::Guid const getId() const;

	/** Returns true if this entity has been marked for destruction. */
	bool const isPendingDestruction() const;
	
};

NS_END
// ----------------------------------------------------------------------------

#endif