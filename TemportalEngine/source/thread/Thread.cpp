#include "thread/Thread.hpp"
#include <thread>

Thread::Thread()
	: mpThreadHandle(nullptr)
	, mFunctorDelegate([&]() { return false; })
	, mOnCompleteDelegate(std::nullopt)
{
}

Thread::Thread(std::string name, logging::LogSystem * pLogSystem)
	: Thread()
{
	mpName = name;
	mLogger = logging::Logger(mpName.c_str(), pLogSystem);
}

Thread::~Thread()
{
	if (this->mpThreadHandle != nullptr)
	{
		this->join();
	}
}

void Thread::setFunctor(DelegateRun functor, bool bIterative)
{
	mFunctorDelegate = functor;
	this->mIsFunctorIterative = bIterative;
}

void Thread::setOnComplete(DelegateOnComplete onComplete)
{
	mOnCompleteDelegate = onComplete;
}

bool Thread::isValid() const
{
	return this->mpThreadHandle != nullptr;
}

void Thread::start()
{
	this->bIsActive = true;
	mpThreadHandle = new std::thread(std::bind(&Thread::run, this));
}

void Thread::run()
{
	mLogger.log(logging::ECategory::LOGINFO, "Starting thread %s", mpName.c_str());
	if (this->mIsFunctorIterative) while (mFunctorDelegate());
	else this->mFunctorDelegate();
	mLogger.log(logging::ECategory::LOGINFO, "Stopping thread %s", mpName.c_str());
	if (mOnCompleteDelegate.has_value())
	{
		(mOnCompleteDelegate.value())();
	}
	this->bIsActive = false;
}

bool Thread::isActive()
{
	return this->bIsActive;
}

void Thread::join()
{
	std::thread *pThread = (std::thread *)this->mpThreadHandle;
	pThread->join();

	delete pThread;
	this->mpThreadHandle = nullptr;
}
