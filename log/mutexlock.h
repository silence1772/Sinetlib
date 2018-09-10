#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include <pthread.h>

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

	void Lock()
	{
		pthread_mutex_lock(&mutex_);
	}

	void Unlock()
	{
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t *Get()
	{
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;
};


class MutexLockGuard
{
public:
	explicit MutexLockGuard(MutexLock &mutex_lock) : mutex_lock_(mutex_lock)
	{
		mutex_lock_.Lock();
	}

	~MutexLockGuard()
	{
		mutex_lock_.Unlock();
	}
private:
	MutexLock &mutex_lock_;
};

#endif // MUTEXLOCK_H