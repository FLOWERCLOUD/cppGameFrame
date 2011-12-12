
#ifndef MYSDK_BASE_BLOCKINGQUEUE_H
#define MYSDK_BASE_BLOCKINGQUEUE_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Condition.h>
#include <mysdk/base/Mutex.h>

#include <deque>
#include <assert.h>

namespace mysdk
{

template<typename T>
class BlockingQueue
{
 public:
  BlockingQueue()
    : mutex_(),
      cond_(mutex_),
      queue_()
  {
  }

  void put(const T& x)
  {
    MutexLockGuard lock(mutex_);
    queue_.push_back(x);
    cond_.notify();
  }

  T take()
  {
    MutexLockGuard lock(mutex_);
    while (queue_.empty())
    {
      cond_.wait();
    }
    assert(!queue_.empty());
    T front(queue_.front());
    queue_.pop_front();
    return front;
  }

  size_t size() const
  {
    MutexLockGuard lock(mutex_);
    return queue_.size();
  }

 private:
  mutable MutexLock mutex_;
  Condition         cond_;
  std::deque<T>     queue_;

 private:
  DISALLOW_COPY_AND_ASSIGN(BlockingQueue);
};

}

#endif
