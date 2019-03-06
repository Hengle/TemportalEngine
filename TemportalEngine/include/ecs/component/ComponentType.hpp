#ifndef TE_ECS_COMPONENT_TYPE_HPP
#define TE_ECS_COMPONENT_TYPE_HPP

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

#endif