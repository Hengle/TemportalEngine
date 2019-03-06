#ifndef TE_MEMORY_POOL_HPP
#define TE_MEMORY_POOL_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Engine ---------------------------------------------------------------------
#include "types/integer.h"
#include "memory/Pool.h"

//-----------------------------------------------------------------------------
NS_MEMORY

template <typename TObject, uSize ObjectMaxCount>
class ObjectPool
{

private:
	struct Node
	{
		uSize mIndexPrevious;
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

		char* headerLocation = (char*)a3_mem_pool_getFirstHeader(pPool);
		char* prevLocation = 0;
		a3_mem_Pool_ObjectHeader* header = (a3_mem_Pool_ObjectHeader*)headerLocation;

		pPool->mpHeaderFreeHead = header;
		pPool->mpHeaderUsedHead = 0;
		for (uSize i = 0; i < ObjectMaxCount; i++)
		{
			header->inUse = 0;
			header->prev = (a3_mem_Pool_ObjectHeader*)prevLocation;
			if (header->prev != 0)
				header->prev->next = header;
			header->next = 0;

			void* objectLocation = header + 1;
			memset(objectLocation, 0, pPool->mSizePerObject);

			prevLocation = headerLocation;
			headerLocation += sizeof(a3_mem_Pool_ObjectHeader) + pPool->mSizePerObject;
			header = (a3_mem_Pool_ObjectHeader*)headerLocation;
		}

	}

public:

	ObjectPool()
	{
		reset(pPool);
	}

	

};

extern a3_mem_Pool* a3_mem_pool_initialize(void* poolLocation, void* contentStart, ui32 const maxCount, ui32 const sizePerObject);
extern void* a3_mem_pool_alloc(a3_mem_Pool *pPool);
extern void a3_mem_pool_dealloc(a3_mem_Pool *pPool, void* object);
inline extern void* a3_mem_pool_getObjectAtIndex(a3_mem_Pool *pPool, ui32 i);
extern ui8 a3_mem_pool_getIteratorNext(a3_mem_Pool const *pPool, void** obj, ui32 *i);

NS_END
//-----------------------------------------------------------------------------

#endif