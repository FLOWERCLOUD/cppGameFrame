
#ifndef MYSDK_BASE_ATOMIC_H
#define MYSDK_BASE_ATOMIC_H

#include <mysdk/base/Common.h>

namespace mysdk {

template<typename T>
class AtomicIntegerT {
public:
	AtomicIntegerT():
		value_(0)
	{
	}

	T get()
	{
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	T getAndAdd(T x)
	{
		return __sync_fetch_and_add(&value_, x);
	}

	T addAndGet(T x)
	{
		return getAndAdd(x) + x;
	}

	T incrementAndGet()
	{
		return addAndGet(1);
	}

	T decrementAndGet()
	{
		return addAndGet(-1);
	}

	void add(T x)
	{
		getAndAdd(x);
	}

	void increment()
	{
		incrementAndGet();
	}

	void decrement()
	{
		decrementAndGet();
	}

	T getAndSet(T newValue)
	{
		return __sync_lock_test_and_set(&value_, newValue);
	}
private:
	volatile T value_;
private:
	DISALLOW_COPY_AND_ASSIGN(AtomicIntegerT);
};

typedef AtomicIntegerT<int32> AtomicInt32;
typedef AtomicIntegerT<int64> AtomicInt64;
}

#endif
