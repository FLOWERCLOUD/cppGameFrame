
#ifndef MYSDK_BASE_ATOMICPTR_H_
#define MYSDK_BASE_ATOMICPTR_H_

#define MYSDK_ATOMIC_PTR_X86

#if defined MYSDK_ATOMIC_PTR_MUTEX
#include <mysdk/base/Mutex.h>
#endif

namespace mysdk
{
    //  This class encapsulates several atomic operations on pointers.
    template <typename T>
    class AtomicPtrT
    {
    public:
        //  Initialise atomic pointer
        inline AtomicPtrT ()
        {
            ptr = NULL;
        }

        //  Destroy atomic pointer
        inline ~AtomicPtrT ()
        {
        }

        //  Set value of atomic pointer in a non-threadsafe way
        //  Use this function only when you are sure that at most one
        //  thread is accessing the pointer at the moment.
        inline void set (T *ptr_)
        {
            this->ptr = ptr_;
        }

        //  Perform atomic 'exchange pointers' operation. Pointer is set
        //  to the 'val' value. Old value is returned.
        inline T *xchg (T *val_)
        {
#if defined MYSDK_ATOMIC_PTR_X86
            T *old;
            __asm__ volatile (
                "lock; xchg %0, %2"
                : "=r" (old), "=m" (ptr)
                : "m" (ptr), "0" (val_));
            return old;
#elif defined MYSDK_ATOMIC_PTR_MUTEX
            sync.lock ();
            T *old = (T*) ptr;
            ptr = val_;
            sync.unlock ();
            return old;
#endif
        }

        //  Perform atomic 'compare and swap' operation on the pointer.
        //  The pointer is compared to 'cmp' argument and if they are
        //  equal, its value is set to 'val'. Old value of the pointer
        //  is returned.
        inline T *cas (T *cmp_, T *val_)
        {
#if defined MYSDK_ATOMIC_PTR_X86
            T *old;
            __asm__ volatile (
                "lock; cmpxchg %2, %3"
                : "=a" (old), "=m" (ptr)
                : "r" (val_), "m" (ptr), "0" (cmp_)
                : "cc");
            return old;
#elif defined MYSDK_ATOMIC_PTR_MUTEX
            sync.lock ();
            T *old = (T*) ptr;
            if (ptr == cmp_)
                ptr = val_;
            sync.unlock ();
            return old;
#endif
        }

    private:

        volatile T *ptr;
#if defined  MYSDK_ATOMIC_PTR_MUTEX
        MutexLock sync;
#endif

        AtomicPtrT (const AtomicPtrT&);
        const AtomicPtrT &operator =(const AtomicPtrT&);
    };

}

//  Remove macros local to this file.
#if defined MYSDK_ATOMIC_PTR_X86
#undef MYSDK_ATOMIC_PTR_X86
#endif
#if defined MYSDK_ATOMIC_PTR_MUTEX
#undef MYSDK_ATOMIC_PTR_MUTEX
#endif

#endif

