#ifndef TE_MEMORY_POOL_HPP
#define TE_MEMORY_POOL_HPP

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Libraries ------------------------------------------------------------------
#include <optional>

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
		std::optional<uSize> mIndexPrevious;
		std::optional<uSize> mIndexNext;
		ui8 mInUse;
	};

	uSize mObjectCount;
	
	std::optional<uSize> mIndexHeadFree;
	std::optional<uSize> mIndexHeadUsed;
	Node mpNodes[ObjectMaxCount];
	TObject mpObjects[ObjectMaxCount];

private:

	void reset()
	{
		mObjectCount = 0;
		memset(mpObjects, 0, ObjectMaxCount * sizeof(TObject));
		
		mIndexHeadFree = 0;
		mIndexHeadUsed = std::nullopt;

		std::optional<uSize> iPrev = std::nullopt;
		for (uSize i = 0; i < ObjectMaxCount; i++)
		{
			Node &header = mpNodes[i];

			header.mInUse = false;
			header.mIndexPrevious = iPrev;
			header.mIndexNext = std::nullopt;

			if (iPrev.has_value())
				mpNodes[iPrev.value()].mIndexNext = i;

			iPrev = i;
		}

	}

public:

	ObjectPool()
	{
		reset();
	}

	TObject* alloc()
	{
		if (mObjectCount >= ObjectMaxCount) return nullptr;

		uSize index = mIndexHeadFree.value();
		Node *header = &mpNodes[index];

		mIndexHeadFree = header->mIndexNext;
		if (mIndexHeadFree.has_value())
			mpNodes[mIndexHeadFree.value()].mIndexPrevious = std::nullopt;

		// Mark as in use
		header->mInUse = true;
		header->mIndexPrevious = std::nullopt;
		header->mIndexNext = std::nullopt;

		std::optional<uSize> prevIndex = std::nullopt;
		// the next node in linear memory that is used
		std::optional<uSize> nextLinearIndexInUsed = mIndexHeadUsed;
		while (nextLinearIndexInUsed.has_value() && nextLinearIndexInUsed.value() < index)
		{
			prevIndex = nextLinearIndexInUsed;
			nextLinearIndexInUsed = mpNodes[nextLinearIndexInUsed.value()].mIndexNext;
		}

		header->mIndexNext = nextLinearIndexInUsed;
		header->mIndexPrevious = prevIndex;
		if (nextLinearIndexInUsed.has_value())
			mpNodes[nextLinearIndexInUsed.value()].mIndexPrevious = index;
		if (prevIndex.has_value())
			mpNodes[prevIndex.value()].mIndexNext = index;
		if (nextLinearIndexInUsed == mIndexHeadUsed)
			mIndexHeadUsed = index;

		mObjectCount++;

		return &mpObjects[index];
	}

	void dealloc(TObject* &ptr)
	{
		uSize index = ptr - mpObjects;

		Node *header = &mpNodes[index];

		if (header->mIndexPrevious.has_value())
			mpNodes[header->mIndexPrevious.value()].mIndexNext = header->mIndexNext;
		else
			mIndexHeadUsed = header->mIndexNext;

		if (header->mIndexNext.has_value())
			mpNodes[header->mIndexNext.value()].mIndexPrevious = header->mIndexPrevious;

		// Mark as free
		header->mInUse = false;
		header->mIndexPrevious = std::nullopt;
		header->mIndexNext = std::nullopt;

		std::optional<uSize> prev = std::nullopt;
		std::optional<uSize> next = mIndexHeadFree;
		while (next.has_value() && next.value() < index)
		{
			prev = next;
			next = mpNodes[next.value()].mIndexNext;
		}

		header->mIndexNext = next;
		if (next.has_value())
			mpNodes[next.value()].mIndexPrevious = index;
		if (next == mIndexHeadFree)
			mIndexHeadFree = index;
		
		header->mIndexPrevious = prev;
		if (prev.has_value())
			mpNodes[prev.value()].mIndexNext = index;

		mObjectCount--;

		// Clear memory for given object
		memset(ptr, 0, sizeof(TObject));
		ptr = nullptr;
	}

	bool getNext(TObject* &obj, uSize &index)
	{
		// there is nothing allocated
		if (mObjectCount == ObjectMaxCount)
		{
			return false;
		}
		// This is the first iteration (object is null)
		else if (*obj == nullptr)
		{
			// Get the first object
			*obj = mpObjects[mIndexHeadUsed];
			// It is the first index
			*i = 0;
			return true;
		}
		// This is some in-between iteration (next index is less than the current count)
		else if (*i + 1 < mObjectCount)
		{
			// get the current object header
			Node *header = &mpNodes[*i];
			// And get the next active object
			// this should always be valid because we are not at the end of the list
			*obj = &mpObjects[header->mIndexNext.value()];
			(*i)++;
			return true;
		}
		// This is the last iteration
		else
		{
			// there are no more active objects
			*obj = 0;
			// we are at the max count
			*i = ObjectMaxCount;
			return false;
		}
	}

};

NS_END
//-----------------------------------------------------------------------------

#endif