#ifndef TE_MEMORY_POOL_HPP
#define TE_MEMORY_POOL_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"

//-----------------------------------------------------------------------------
NS_MEMORY

template <typename TObject, uSize ObjectMaxCount>
class ObjectPool
{

private:
	struct Node
	{
		ui8 mHasPrevious;
		uSize mIndexPrevious;

		ui8 mHasNext;
		uSize mIndexNext;
		
		ui8 mInUse;
	};
	
	uSize mObjectCount;
	
	Node mpNodes[ObjectMaxCount];
	TObject mpObjects[ObjectMaxCount];

private:

	void reset()
	{
		mObjectCount = 0;
		memset(mpObjects, 0, ObjectMaxCount * sizeof(TObject));

		for (uSize i = 0; i < ObjectMaxCount; i++)
		{
			Node &header = mpNodes[i];

			//header = 0;

			/*header.inUse = 0;
			header.mIndexPrevious = i - 1;
			header.prev = (a3_mem_Pool_ObjectHeader*)prevLocation;
			if (header->prev != 0)
				header->prev->next = header;
			header->next = 0;

			void* objectLocation = header + 1;

			prevLocation = headerLocation;
			headerLocation += sizeof(a3_mem_Pool_ObjectHeader) + pPool->mSizePerObject;
			header = (a3_mem_Pool_ObjectHeader*)headerLocation;*/
		}

	}

public:

	ObjectPool()
	{
		reset();
	}

	

};

NS_END
//-----------------------------------------------------------------------------

#endif