
#ifndef MYSDK_BASE_CONDITIION_H
#define MYSDK_BASE_CONDITION_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Mutex.h>

#include <pthread.h>


namespace mysdk
{

class Condition
{
 public:
  explicit Condition(MutexLock& mutex) : mutex_(mutex)
  {
    pthread_cond_init(&pcond_, NULL);
  }

  ~Condition()
  {
    pthread_cond_destroy(&pcond_);
  }

  void wait()
  {
    pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
  }

  void notify()
  {
    pthread_cond_signal(&pcond_);
  }

  void notifyAll()
  {
    pthread_cond_broadcast(&pcond_);
  }

 private:
  MutexLock& mutex_;
  pthread_cond_t pcond_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Condition);
};

}
#endif
