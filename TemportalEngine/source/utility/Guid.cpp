#include "utility/Guid.hpp"

//#include <stdio.h>
#include <cassert>

#ifdef _WIN32
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#include "Windows.h"
#include "Rpc.h"
#endif

using namespace utility;

constexpr utility::Guid Guid::fromString(unsigned char stringOut[GUID_STRING_LENGTH])
{
	GuidData data;
#ifdef _WIN32
	UUID uuid;
	UuidFromStringA(stringOut, &uuid);
	data = *((GuidData*)(&uuid));
#else
	assert(false);
#endif
	Guid guid = utility::Guid(data);
	return guid;
}

constexpr int Guid::compare(Guid const &a, Guid const &b)
{
#ifdef _WIN32
	UUID* uuidA = (UUID*)(&a.mData);
	UUID* uuidB = (UUID*)(&b.mData);
	RPC_STATUS errors = 0;
	return UuidCompare(uuidA, uuidB, &errors);
#else
	assert(false);
#endif
	assert(false);
	return 0;
}

constexpr Guid::Guid(GuidData const &data)
	: mData(data)
{
}

constexpr Guid::Guid()
	: mData({})
{
}

constexpr Guid::Guid(utility::Guid const &other)
	: mData(other.mData)
{
}

// 8-4-4-4-12
constexpr bool Guid::toString(unsigned char stringOut[GUID_STRING_LENGTH]) const
{
	GuidData data = mData;
#ifdef _WIN32
	UuidToStringA((UUID*)(&data), &stringOut);
	return true;
#else
	assert(false);
#endif
	return false;
}

constexpr bool const Guid::isValid() const
{
	return *this != Guid();
}

constexpr bool const Guid::operator<(Guid const &other) const
{
	return Guid::compare(*this, other) < 0;
}

constexpr bool const Guid::operator==(Guid const &other) const
{
	return Guid::compare(*this, other) == 0;
}

constexpr bool const Guid::operator!=(Guid const &other) const
{
	return !(*this == other);
}

constexpr bool const Guid::operator>(Guid const &other) const
{
	return Guid::compare(*this, other) > 0;
}

size_t const Guid::hash() const
{
#ifdef _WIN32
	UUID* uuid = (UUID*)(&mData);
	RPC_STATUS errors;
	return UuidHash(uuid, &errors);
#else
	assert(false);
	return 0;
#endif
}
