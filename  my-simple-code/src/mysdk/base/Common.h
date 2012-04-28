#ifndef MYSDK_BASE_COMMON_H
#define MYSDK_BASE_COMMON_H

#include <assert.h>
#include<cstddef>

#include <stdio.h>
#include <stdint.h>

namespace mysdk {

#undef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
  TypeName(const TypeName&);                           \
  void operator=(const TypeName&)

#undef LIBMYSDK_EXPORT
 #define LIBMYSDK_EXPORT

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#undef MYSDK_LONGLONG
#undef MYSQK_ULONGLONG
#undef MYSDK_LL_FORMAT

#define MYSDK_LONGLONG(x) x##LL
#define MYSDK_ULONGLONG(x) x##ULL
#define MYSDK_LL_FORMAT "l"  // As in "%lld". Note that "q" is poor form also.


static const int32 kint32max = 0x7FFFFFFF;
static const int32 kint32min = -kint32max - 1;
static const int64 kint64max = MYSDK_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64 kint64min = -kint64max - 1;
static const uint32 kuint32max = 0xFFFFFFFFu;
static const uint64 kuint64max = MYSDK_ULONGLONG(0xFFFFFFFFFFFFFFFF);

// ===================================================================
// from google3/base/scoped_ptr.h

namespace internal {

//  This is an implementation designed to match the anticipated future TR2
//  implementation of the scoped_ptr class, and its closely-related brethren,
//  scoped_array, scoped_ptr_malloc, and make_scoped_ptr.

template <class C> class scoped_ptr;
template <class C> class scoped_array;

// A scoped_ptr<T> is like a T*, except that the destructor of scoped_ptr<T>
// automatically deletes the pointer it holds (if any).
// That is, scoped_ptr<T> owns the T object that it points to.
// Like a T*, a scoped_ptr<T> may hold either NULL or a pointer to a T object.
//
// The size of a scoped_ptr is small:
// sizeof(scoped_ptr<C>) == sizeof(C*)
template <class C>
class scoped_ptr {
 public:

  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_ptr.
  // The input parameter must be allocated with new.
  explicit scoped_ptr(C* p = NULL) : ptr_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(C) };
    delete ptr_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != ptr_) {
      enum { type_must_be_complete = sizeof(C) };
      delete ptr_;
      ptr_ = p;
    }
  }

  // Accessors to get the owned object.
  // operator* and operator-> will assert() if there is no current object.
  C& operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }
  C* operator->() const  {
    assert(ptr_ != NULL);
    return ptr_;
  }
  C* get() const { return ptr_; }

  // Comparison operators.
  // These return whether two scoped_ptr refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C* p) const { return ptr_ == p; }
  bool operator!=(C* p) const { return ptr_ != p; }

  // Swap two scoped pointers.
  void swap(scoped_ptr& p2) {
    C* tmp = ptr_;
    ptr_ = p2.ptr_;
    p2.ptr_ = tmp;
  }

  // Release a pointer.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = ptr_;
    ptr_ = NULL;
    return retVal;
  }

 private:
  C* ptr_;

  // Forbid comparison of scoped_ptr types.  If C2 != C, it totally doesn't
  // make sense, and if C2 == C, it still doesn't make sense because you should
  // never have the same object owned by two different scoped_ptrs.
  template <class C2> bool operator==(scoped_ptr<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

  // Disallow evil constructors
  scoped_ptr(const scoped_ptr&);
  void operator=(const scoped_ptr&);
};

// scoped_array<C> is like scoped_ptr<C>, except that the caller must allocate
// with new [] and the destructor deletes objects with delete [].
//
// As with scoped_ptr<C>, a scoped_array<C> either points to an object
// or is NULL.  A scoped_array<C> owns the object that it points to.
//
// Size: sizeof(scoped_array<C>) == sizeof(C*)
template <class C>
class scoped_array {
 public:

  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_array.
  // The input parameter must be allocated with new [].
  explicit scoped_array(C* p = NULL) : array_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_array() {
    enum { type_must_be_complete = sizeof(C) };
    delete[] array_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != array_) {
      enum { type_must_be_complete = sizeof(C) };
      delete[] array_;
      array_ = p;
    }
  }

  // Get one element of the current object.
  // Will assert() if there is no current object, or index i is negative.
  C& operator[](std::ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  // Get a pointer to the zeroth element of the current object.
  // If there is no current object, return NULL.
  C* get() const {
    return array_;
  }

  // Comparison operators.
  // These return whether two scoped_array refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C* p) const { return array_ == p; }
  bool operator!=(C* p) const { return array_ != p; }

  // Swap two scoped arrays.
  void swap(scoped_array& p2) {
    C* tmp = array_;
    array_ = p2.array_;
    p2.array_ = tmp;
  }

  // Release an array.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = array_;
    array_ = NULL;
    return retVal;
  }

 private:
  C* array_;

  // Forbid comparison of different scoped_array types.
  template <class C2> bool operator==(scoped_array<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_array<C2> const& p2) const;

  // Disallow evil constructors
  scoped_array(const scoped_array&);
  void operator=(const scoped_array&);
};

}  // namespace internal

// We made these internal so that they would show up as such in the docs,
// but we don't want to stick "internal::" in front of them everywhere.
using internal::scoped_ptr;
using internal::scoped_array;

// Taken from google-protobuf stubs/common.h
//
// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.

//
// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template<typename To, typename From>
inline To implicit_cast(From const &f) {
  return f;
}

// ===================================================================
// emulates google3/base/callback.h

// Abstract interface for a callback.  When calling an RPC, you must provide
// a Closure to call when the procedure completes.  See the Service interface
// in service.h.
//
// To automatically construct a Closure which calls a particular function or
// method with a particular set of parameters, use the NewCallback() function.
// Example:
//   void FooDone(const FooResponse* response) {
//     ...
//   }
//
//   void CallFoo() {
//     ...
//     // When done, call FooDone() and pass it a pointer to the response.
//     Closure* callback = NewCallback(&FooDone, response);
//     // Make the call.
//     service->Foo(controller, request, response, callback);
//   }
//
// Example that calls a method:
//   class Handler {
//    public:
//     ...
//
//     void FooDone(const FooResponse* response) {
//       ...
//     }
//
//     void CallFoo() {
//       ...
//       // When done, call FooDone() and pass it a pointer to the response.
//       Closure* callback = NewCallback(this, &Handler::FooDone, response);
//       // Make the call.
//       service->Foo(controller, request, response, callback);
//     }
//   };
//
// Currently NewCallback() supports binding zero, one, or two arguments.
//
// Callbacks created with NewCallback() automatically delete themselves when
// executed.  They should be used when a callback is to be called exactly
// once (usually the case with RPC callbacks).  If a callback may be called
// a different number of times (including zero), create it with
// NewPermanentCallback() instead.  You are then responsible for deleting the
// callback (using the "delete" keyword as normal).
//
// Note that NewCallback() is a bit touchy regarding argument types.  Generally,
// the values you provide for the parameter bindings must exactly match the
// types accepted by the callback function.  For example:
//   void Foo(string s);
//   NewCallback(&Foo, "foo");          // WON'T WORK:  const char* != string
//   NewCallback(&Foo, string("foo"));  // WORKS
// Also note that the arguments cannot be references:
//   void Foo(const string& s);
//   string my_str;
//   NewCallback(&Foo, my_str);  // WON'T WORK:  Can't use referecnes.
// However, correctly-typed pointers will work just fine.
class  Closure {
 public:
  Closure() {}
  virtual ~Closure(){}

  virtual void Run() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Closure);
};

namespace internal {

class FunctionClosure0 : public Closure {
 public:
  typedef void (*FunctionType)();

  FunctionClosure0(FunctionType function, bool self_deleting)
    : function_(function), self_deleting_(self_deleting) {}
  ~FunctionClosure0() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_();
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
};

template <typename Class>
class MethodClosure0 : public Closure {
 public:
  typedef void (Class::*MethodType)();

  MethodClosure0(Class* object, MethodType method, bool self_deleting)
    : object_(object), method_(method), self_deleting_(self_deleting) {}
  ~MethodClosure0() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)();
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
};

template <typename Arg1>
class FunctionClosure1 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1);

  FunctionClosure1(FunctionType function, bool self_deleting,
                   Arg1 arg1)
    : function_(function), self_deleting_(self_deleting),
      arg1_(arg1) {}
  ~FunctionClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Class, typename Arg1>
class MethodClosure1 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1);

  MethodClosure1(Class* object, MethodType method, bool self_deleting,
                 Arg1 arg1)
    : object_(object), method_(method), self_deleting_(self_deleting),
      arg1_(arg1) {}
  ~MethodClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Arg1, typename Arg2>
class FunctionClosure2 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

  FunctionClosure2(FunctionType function, bool self_deleting,
                   Arg1 arg1, Arg2 arg2)
    : function_(function), self_deleting_(self_deleting),
      arg1_(arg1), arg2_(arg2) {}
  ~FunctionClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template <typename Class, typename Arg1, typename Arg2>
class MethodClosure2 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

  MethodClosure2(Class* object, MethodType method, bool self_deleting,
                 Arg1 arg1, Arg2 arg2)
    : object_(object), method_(method), self_deleting_(self_deleting),
      arg1_(arg1), arg2_(arg2) {}
  ~MethodClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

}  // namespace internal

// See Closure.
inline Closure* NewCallback(void (*function)()) {
  return new internal::FunctionClosure0(function, true);
}

// See Closure.
inline Closure* NewPermanentCallback(void (*function)()) {
  return new internal::FunctionClosure0(function, false);
}

// See Closure.
template <typename Class>
inline Closure* NewCallback(Class* object, void (Class::*method)()) {
  return new internal::MethodClosure0<Class>(object, method, true);
}

// See Closure.
template <typename Class>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)()) {
  return new internal::MethodClosure0<Class>(object, method, false);
}

// See Closure.
template <typename Arg1>
inline Closure* NewCallback(void (*function)(Arg1),
                            Arg1 arg1) {
  return new internal::FunctionClosure1<Arg1>(function, true, arg1);
}

// See Closure.
template <typename Arg1>
inline Closure* NewPermanentCallback(void (*function)(Arg1),
                                     Arg1 arg1) {
  return new internal::FunctionClosure1<Arg1>(function, false, arg1);
}

// See Closure.
template <typename Class, typename Arg1>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1),
                            Arg1 arg1) {
  return new internal::MethodClosure1<Class, Arg1>(object, method, true, arg1);
}

// See Closure.
template <typename Class, typename Arg1>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1),
                                     Arg1 arg1) {
  return new internal::MethodClosure1<Class, Arg1>(object, method, false, arg1);
}

// See Closure.
template <typename Arg1, typename Arg2>
inline Closure* NewCallback(void (*function)(Arg1, Arg2),
                            Arg1 arg1, Arg2 arg2) {
  return new internal::FunctionClosure2<Arg1, Arg2>(
    function, true, arg1, arg2);
}

// See Closure.
template <typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2),
                                     Arg1 arg1, Arg2 arg2) {
  return new internal::FunctionClosure2<Arg1, Arg2>(
    function, false, arg1, arg2);
}

// See Closure.
template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2),
                            Arg1 arg1, Arg2 arg2) {
  return new internal::MethodClosure2<Class, Arg1, Arg2>(
    object, method, true, arg1, arg2);
}

// See Closure.
template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(
    Class* object, void (Class::*method)(Arg1, Arg2),
    Arg1 arg1, Arg2 arg2) {
  return new internal::MethodClosure2<Class, Arg1, Arg2>(
    object, method, false, arg1, arg2);
}

// A function which does nothing.  Useful for creating no-op callbacks, e.g.:
//   Closure* nothing = NewCallback(&DoNothing);
//void  DoNothing(){}
}


#endif
