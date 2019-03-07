#include "ecs/system/System.hpp"
#include <cassert>
#include "ecs/Core.hpp"
#include "ecs/ObjectManager.hpp"
#include "ecs/component/Component.hpp"

using namespace ecs;

System::System(
	std::array<TypeInclude, ECS_MAX_UNIQUE_COMPONENT_TYPES> const includes,
	std::array<TypeExclude, ECS_MAX_UNIQUE_COMPONENT_TYPES> const excludes
)
	: maTypesToInclude(includes)
	, maTypesToExclude(excludes)
{

}

System::System()
	: System({}, {})
{

}

bool const System::reset()
{
	return maComponentSets.empty();
}

void System::retrieveComponentData(Core *pCore)
{
	uSize componentSetCount = 0;

	Core::EntityManager *pMngEntity = pCore->getEntityManager();
	Entity *pCurrentEntity = 0;
	uSize iEntity = 0;
	TypeExclude typeToExclude;
	TypeInclude typeToInclude;

	// ****TO-DO: I dont actually /want/ to iterate over all entities
	while (pMngEntity->getNext(pCurrentEntity, iEntity))
	{
		if (pCurrentEntity->isPendingDestruction()) continue;

		// Check for entities with excluded components
		bool hasAnyExcludedComponents = false;
		for (auto it = maTypesToExclude.begin(); it != maTypesToExclude.end(); ++it)
		{
			typeToExclude = *it;
			if (pCurrentEntity->hasComponent(typeToExclude.id))
			{
				hasAnyExcludedComponents = true;
				break;
			}
		}
		if (!hasAnyExcludedComponents)
		{
			bool hasAllComponents = true;
			uSize iType = 0;
			for (auto it = maTypesToInclude.begin(); it != maTypesToInclude.end(); ++it, ++iType)
			{
				typeToInclude = *it;
				if (pCurrentEntity->hasComponent(typeToInclude.id))
				{
					maComponentSets[componentSetCount].maComponentIds[iType] = pCurrentEntity->getComponentId(typeToInclude.id);
				}
				else
				{
					hasAllComponents = false;
					break;
				}
			}
			if (hasAllComponents)
				componentSetCount++;
		}
	}
}

void System::operator()(Core *pCore, f32 const &deltaTime)
{
	this->retrieveComponentData(pCore);
	this->execute(pCore, deltaTime);
}

void System::execute(Core *pCore, f32 const & deltaTime)
{
	for (auto it = maComponentSets.begin(); it != maComponentSets.end(); ++it)
	{
		this->updateComponents(pCore, (*it).maComponentIds);
	}
}
