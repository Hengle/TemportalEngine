#ifndef TE_UTILITY_GUID_HPP
#define TE_UTILITY_GUID_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Libraries ------------------------------------------------------------------
# include <type_traits>

// ----------------------------------------------------------------------------
NS_UTILITY

#define GUID_STRING_LENGTH 36

class Guid
{

private:

	struct GuidData
	{
		unsigned long  data1;
		unsigned short data2;
		unsigned short data3;
		unsigned char  data4[8];
	};

	GuidData const mData;

	constexpr Guid(GuidData const &data);

public:
	static constexpr Guid fromString(unsigned char stringOut[GUID_STRING_LENGTH]);
	static constexpr int compare(Guid const &a, Guid const &b);

	constexpr Guid();

	constexpr Guid(Guid const &other);

	// 8-4-4-4-12
	constexpr bool toString(unsigned char stringOut[GUID_STRING_LENGTH]) const;

	constexpr bool const isValid() const;

	constexpr bool const operator<(Guid const &other) const;
	
	constexpr bool const operator==(Guid const &other) const;

	constexpr bool const operator!=(Guid const &other) const;

	constexpr bool const operator>(Guid const &other) const;

	size_t const hash() const;

};

NS_END
// ----------------------------------------------------------------------------

namespace std {
	template<>
	struct hash<utility::Guid>
	{
		inline size_t operator()(utility::Guid const &guid) const
		{
			return guid.hash();
		}
	};
}

#endif