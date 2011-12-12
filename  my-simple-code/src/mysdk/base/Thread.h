
#ifndef MYSDK_BASE_THREAD_H
#define MYSDK_BASE_THREAD_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Atomic.h>

#include <pthread.h>

#include <tr1/functional>
#include <string>

namespace mysdk
{
class Thread
{
public:
  typedef std::tr1::function<void ()> ThreadFunc;

  explicit Thread(const ThreadFunc&, const std::string& name = std::string());
  ~Thread();

  void start();
  void join();

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const std::string& name() const { return name_; }

  static int numCreated() { return numCreated_.get(); }

 private:
  static void* startThread(void* thread);
  void runInThread();

  bool       started_;
  pthread_t  pthreadId_;
  pid_t      tid_;
  ThreadFunc func_;
  std::string     name_;

  static AtomicInt32 numCreated_;
};

namespace CurrentThread
{
  pid_t tid();
  const char* name();
  bool isMainThread();
}

}
#endif
