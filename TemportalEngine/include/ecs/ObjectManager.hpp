#ifndef TE_ECS_OBJECTMANAGER_HPP
#define TE_ECS_OBJECTMANAGER_HPP
#pragma warning(push)
#pragma warning(disable:4251) // disable STL warnings in dll

// PCH ------------------------------------------------------------------------
#include "TemportalEnginePCH.hpp"

// Libraries ------------------------------------------------------------------
#include <functional>

// Engine ---------------------------------------------------------------------
#include "memory/ObjectPool.hpp"
#include "thread/MutexLock.hpp"
#include "utility/Guid.hpp"

// ----------------------------------------------------------------------------
NS_ECS

template <class TObject, uSize TMaxCount>
class TEMPORTALENGINE_API ObjectManager
{

private:
	memory::ObjectPool<TObject, TMaxCount> mpPool[1];
	thread::MutexLock mpMutex[1];
	std::function<utility::Guid(TObject*)> const mGetId;

public:

	ObjectManager() {}
	ObjectManager(std::function<utility::Guid(TObject*)> const getId)
		: mGetId(getId)
	{}

	template <typename... TArgs>
	TObject* const create(TArgs... args)
	{
		this->mpMutex->lock();
		auto object = this->mpPool->alloc();
		this->mpMutex->unlock();

		if (object != nullptr)
		{
			new (object) TObject(args...);
		}
		
		return object;
	}

	void destroy(TObject* &ptr)
	{
		this->mpMutex->lock();
		this->mpPool->dealloc(ptr);
		this->mpMutex->unlock();
	}

	std::optional<TObject*> const getObjectWithId(utility::Guid const &id)
	{
		if (!id.isValid()) return std::nullopt;

		TObject *pObj;
		uSize index;
		while (mpPool->getNext(pObj, index))
		{
			if (id == mGetId(pObj)) return pObj;
		}

		return std::nullopt;
	}

	bool const getNext(TObject* &obj, uSize &index)
	{
		return mpPool->getNext(obj, index);
	}

};

NS_END
// ----------------------------------------------------------------------------

#pragma warning(pop)
#endif