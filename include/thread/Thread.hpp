#ifndef INCLUDE_THREAD_H_
#define INCLUDE_THREAD_H_

#include <thread>
#include "logging/Logger.hpp"

template <typename... TArgs>
class Thread
{
public:
	typedef void (*DelegateUpdate)(TArgs...);

private:
	char const* mpName;
	std::thread mpHandle[1];
	logging::Logger mLogger;
	DelegateUpdate mpDelegateUpdate;

	void updateInternal(TArgs... args)
	{
		mLogger.log(logging::ECategory::INFO, "Starting thread");
		(*mpDelegateUpdate)(args...);
		mLogger.log(logging::ECategory::INFO, "Stopping thread");
	}

	static void updateInternalStatic(Thread<TArgs...> *pThread, TArgs... args)
	{
		pThread->updateInternal(args...);
	}

public:
	Thread() {}
	Thread(char const* name, logging::LogSystem *pLogSystem, DelegateUpdate update)
		: mpName(name)
		, mpDelegateUpdate(update)
	{
		mLogger = logging::Logger(mpName, pLogSystem);
	}

	void start(TArgs... args)
	{
		*mpHandle = std::thread(&updateInternalStatic, this, args...);
	}

	void join()
	{
		this->mpHandle->join();
	}

};

#endif  // INCLUDE_THREAD_H_