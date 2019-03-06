#include "ecs/Entity.hpp"

using namespace ecs;

Entity::Entity()
{

}

void Entity::initialize(utility::Guid const & id)
{
	mId = id;
}

void Entity::markForDestruction()
{
	mIsPendingDestruction = true;
}

utility::Guid const Entity::getId() const
{
	return mId;
}

bool const Entity::isPendingDestruction() const
{
	return mIsPendingDestruction;
}
