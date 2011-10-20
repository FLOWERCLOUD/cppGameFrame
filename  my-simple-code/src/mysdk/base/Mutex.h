
#ifndef MYSDK_BASE_MUTEX_H
#define	MYSDK_BASE_MUTEX_H

#include <mysdk/base/Common.h>

#include <pthread.h>

namespace mysdk
{
	class MutexLock
	{
	public:
		MutexLock()
		{
			pthread_mutex_init(&mutex_, NULL);
		}
		~MutexLock()
		{
			pthread_mutex_destroy(&mutex_);
		}
		void lock()
		{
			pthread_mutex_lock(&mutex_);
		}
		void unlock()
		{
			pthread_mutex_unlock(&mutex_);
		}
		pthread_mutex_t* getPthreadMutex()
		{
			return &mutex_;
		}
	private:
		pthread_mutex_t mutex_;
	private:
		DISALLOW_COPY_AND_ASSIGN(MutexLock);
	};

	class MutexLockGuard
	{
	public:
		explicit MutexLockGuard(MutexLock& mutex):
			mutex_(mutex)
		{
			mutex_.lock();
		}
		~MutexLockGuard()
		{
			mutex_.unlock();
		}
	private:
		MutexLock& mutex_;
	private:
		DISALLOW_COPY_AND_ASSIGN(MutexLockGuard);
	};
}

// Prevent misuse like:
// MutexLockGuard(mutex_);
// A tempory object doesn't hold the lock for long!
#define MutexLockGuard(x) error "Missing guard object name"

#endif
