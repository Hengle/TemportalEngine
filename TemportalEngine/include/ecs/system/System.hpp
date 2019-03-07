#ifndef TE_ECS_SYSTEM_HPP
#define TE_ECS_SYSTEM_HPP
#pragma warning(push)
#pragma warning(disable:4251) // disable STL warnings in dll

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Parents --------------------------------------------------------------------
#include "registry/RegistryItem.hpp"

// Libraries ------------------------------------------------------------------
#include <array>

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "types/real.h"
#include "utility/Guid.hpp"
#include "ecs/EcsBudget.h"

// ----------------------------------------------------------------------------
NS_ECS

class Core;

//template <typename... TComponentTypes>
class TEMPORTALENGINE_API System
{

protected:
	typedef std::array<utility::Guid, ECS_MAX_UNIQUE_COMPONENT_TYPES> ComponentIds;

private:
	
	utility::Guid mId;

	// Component Types --------------------------------------------------------
	
	struct TypeInclude
	{
		RegistryIdentifier id;
		bool isWriteable;
	};
	struct TypeExclude
	{
		RegistryIdentifier id;
	};

	std::array<TypeInclude, ECS_MAX_UNIQUE_COMPONENT_TYPES> const maTypesToInclude;
	std::array<TypeExclude, ECS_MAX_UNIQUE_COMPONENT_TYPES> const maTypesToExclude;
	
	// Component Data ---------------------------------------------------------

	struct ComponentSet
	{
		ComponentIds maComponentIds;
	};

	std::array<ComponentSet, ECS_MAX_ENTITY_COUNT> maComponentSets;
	
public:
	System();
	System(
		std::array<TypeInclude, ECS_MAX_UNIQUE_COMPONENT_TYPES> const includes,
		std::array<TypeExclude, ECS_MAX_UNIQUE_COMPONENT_TYPES> const excludes
	);

	bool const reset();
	void retrieveComponentData(Core *pCore);

	void operator()(Core *pCore, f32 const &deltaTime);

	void execute(Core *pCore, f32 const &deltaTime);

	virtual void updateComponents(Core *pCore, ComponentIds const &ids) = 0;

};

NS_END
// ----------------------------------------------------------------------------

#pragma warning(pop)
#endif