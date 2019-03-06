#ifndef TE_ECS_COMPONENT_TYPE_HPP
#define TE_ECS_COMPONENT_TYPE_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// ----------------------------------------------------------------------------
NS_ECS

typedef i32 ComponentTypeIndex;

// ----------------------------------------------------------------------------
// TODO: Use optional<uSize> instead of a signed integer for type index
#define GENERATE_COMPONENT_IDENTIFICATION(humanReadableTypeIdentifier, type) \
private: \
enum { __GENERATED_COMPONENT_TYPE_ID = humanReadableTypeIdentifier }; \
public: \
	static ComponentTypeIndex IndexIdentifier; \
	static constexpr ui32 const getHumanReadableTypeIdentifier() {return __GENERATED_COMPONENT_TYPE_ID;} \

// ----------------------------------------------------------------------------

struct ComponentType
{
	typedef void* (*DelegateCreateManager)();

	uSize sizeInBytes;
	DelegateCreateManager mpfCreateManager;

};

NS_END
// ----------------------------------------------------------------------------

#endif