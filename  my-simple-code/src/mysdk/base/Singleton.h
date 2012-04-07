
#ifndef MYSDK_BASE_SINGLETON_H
#define MYSDK_BASE_SINGLETON_H

#include <mysdk/base/Common.h>

#include <pthread.h>
#include <stdlib.h> // atexit

namespace mysdk
{
template<typename T>
class Singleton
{
public:
	static T& instance()
	{
		pthread_once(&ponce_, &Singleton::init);
		return *value_;
	}

private:
	  Singleton();
	  ~Singleton();

	  static void init()
	  {
		  value_ = new T();
		  ::atexit(destroy);
	  }

	  static void destroy()
	  {
		  delete value_;
	  }

private:
	  static pthread_once_t ponce_;
	  static T*             value_;
 private:
	  DISALLOW_COPY_AND_ASSIGN(Singleton);
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}
#endif

