#pragma once

#include "TemportalEnginePCH.hpp"

template<class TSignature>
class ExecuteDelegate
{
	typedef std::function<TSignature> TBinding;

public:

	void bind(TBinding binding)
	{
		this->mBinding = binding;
	}

	template <typename... TArgs>
	void execute(TArgs... args)
	{
		if (this->mBinding)
		{
			this->mBinding(args...);
		}
	}

	void clear()
	{
		this->mBinding = nullptr;
	}

private:
	TBinding mBinding;

};

template <class TSignature>
class BroadcastDelegate
{
	typedef std::function<TSignature> TBinding;
	typedef std::weak_ptr<void> TKey;

	struct Pair
	{
		TKey key;
		TBinding binding;
	};

public:
	BroadcastDelegate() = default;
	
	void bind(TKey key, TBinding binding)
	{
		assert(!this->hasBindingFor(key));
		this->mBindings.push_back({ key, binding });
	}

	void unbind(TKey key)
	{
		assert(this->hasBindingFor(key));
		this->mBindings.erase(this->find(key));
	}

	// 2O(n) to remove all bindings which are expired and then execute all non-expired bindings
	template <typename... TArgs>
	void broadcast(TArgs... args)
	{
		this->mBindings.erase(std::remove_if(
			this->mBindings.begin(), this->mBindings.end(),
			[](Pair const &pair) { return pair.key.expired(); }
		), this->mBindings.end());
		for (auto& pair : this->mBindings)
		{
			pair.binding(args...);
		}
	}

private:
	/**
	 * A map keyed by object pointers to the binding they have added.
	 */
	std::vector<Pair> mBindings;

	// O(n) search for a pair with the key
	auto find(TKey key)
	{
		for (auto it = this->mBindings.begin(); it != this->mBindings.end(); ++it)
		{
			// The keys are equivalent if neither is "before" (i.e. less than) the other
			if (!it->key.owner_before(key) && !key.owner_before(it->key))
			{
				return it;
			}
		}
		return this->mBindings.end();
	}

	// O(n) search for a pair with the key
	bool hasBindingFor(TKey key)
	{
		return this->find(key) != this->mBindings.end();
	}

};

template <typename TEventType, class TSignature>
class KeyedBroadcastDelegate
{
	typedef std::function<TSignature> TBinding;
	typedef std::weak_ptr<void> TKey;

	struct Pair
	{
		TKey key;
		TBinding binding;
	};

public:
	KeyedBroadcastDelegate() = default;

	void bind(TEventType evt, TKey key, TBinding binding)
	{
		assert(!this->hasBindingFor(evt, key));
		Pair pair = { key, binding };
		this->mBindings.insert(std::make_pair(evt, pair));
	}

	void unbind(TEventType evt, TKey key)
	{
		assert(this->hasBindingFor(evt, key));
		this->mBindings.erase(this->find(evt, key));
	}

	void unbindExpired(TEventType evt)
	{
		auto range = this->mBindings.equal_range(evt);
		for (auto it = range.first; it != range.second;)
		{
			if (it->second.key.expired())
			{
				it = this->mBindings.erase(it);
				continue;
			}
			++it;
		}
	}

	// O(n) for a given event to remove all bindings which are expired and execute all non-expired bindings (maps allow in-iteration erasure)
	template <typename... TArgs>
	void broadcast(TEventType evt, TArgs... args)
	{
		auto range = this->mBindings.equal_range(evt);
		for (auto it = range.first; it != range.second;)
		{
			if (it->second.key.expired())
			{
				it = this->mBindings.erase(it);
				continue;
			}
			it->second.binding(args...);
			++it;
		}
	}

private:
	std::multimap<TEventType, Pair> mBindings;

	auto find(TEventType evt, TKey key)
	{
		auto range = this->mBindings.equal_range(evt);
		for (auto it = range.first; it != range.second; ++it)
		{
			auto pairKey = it->second.key;
			// The keys are equivalent if neither is "before" (i.e. less than) the other
			if (!pairKey.owner_before(key) && !key.owner_before(pairKey))
			{
				return it;
			}
		}
		return this->mBindings.end();
	}

	bool hasBindingFor(TEventType evt, TKey key)
	{
		return this->find(evt, key) != this->mBindings.end();
	}

};
