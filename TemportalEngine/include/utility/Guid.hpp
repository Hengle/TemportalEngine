#ifndef TE_UTILITY_GUID_HPP
#define TE_UTILITY_GUID_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Libraries ------------------------------------------------------------------
# include <type_traits>

// ----------------------------------------------------------------------------
NS_UTILITY

#define GUID_STRING_LENGTH 36

class TEMPORTALENGINE_API Guid
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
	static Guid fromString(unsigned char stringOut[GUID_STRING_LENGTH]);
	static int compare(Guid const &a, Guid const &b);

	constexpr Guid();

	constexpr Guid(Guid const &other);

	// 8-4-4-4-12
	bool toString(unsigned char stringOut[GUID_STRING_LENGTH]) const;

	bool const isValid() const;

	bool const operator<(Guid const &other) const;
	
	bool const operator==(Guid const &other) const;

	bool const operator!=(Guid const &other) const;

	bool const operator>(Guid const &other) const;

	void operator=(Guid const &other);

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