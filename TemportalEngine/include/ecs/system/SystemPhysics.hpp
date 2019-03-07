#ifndef TE_ECS_SYSTEM_PHYSICS_HPP
#define TE_ECS_SYSTEM_PHYSICS_HPP
#pragma warning(push)
#pragma warning(disable:4251) // disable STL warnings in dll

// Parents --------------------------------------------------------------------
#include "ecs/system/System.hpp"

// ----------------------------------------------------------------------------
NS_ECS

class TEMPORTALENGINE_API SystemPhysics : public System
{
public:
	SystemPhysics();

	void updateComponents(Core *pCore, ComponentIds const &ids) override;

};

NS_END
// ----------------------------------------------------------------------------

#pragma warning(pop)
#endif