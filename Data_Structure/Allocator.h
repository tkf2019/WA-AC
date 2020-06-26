//file: Allocator.h
#ifndef ALLOCATOR_T
#define ALLOCATOR_T

#include<new>
#include<string>
#include<cstdlib>
#include<cstddef>
#include<climits>
#include<iostream>
#include"Type_Traits.h"

namespace TKF
{

template <typename T>
inline T* _allocate(ptrdiff_t size, T*) {
    if (size == 0) {
        return nullptr;
    }
    return static_cast<T*>(::operator new(size * sizeof(T)));
}

template <typename T>
inline void _deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <typename T1>
inline void _construct (T1* p) {
    :: new ((void*)p)T1();
}

template <typename T1, typename T2>
inline void _construct (T1* p, T2 const& value) {
    ::new ((void*)p)T1(value);
}

template <typename T1, typename... Args>
inline void _construct (T1* p, Args&&... args) {
    ::new ((void*)p)T1(TKF::forward<Args>(args)...);
}

template <typename T>
inline void _destroy(T* ptr) {
    ptr->~T();
}

void THROW_OUT_OF_RANGE_IF(bool judge, std::string sentence) {
    if(!judge) return;
    std::cerr << sentence;
    exit(1);
}

template <typename T>
class allocator {
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    static pointer allocate(size_type N) {
        return _allocate((difference_type)N, (pointer)0);
    }

    static void construct (pointer p) {
        _construct(p);
    }

    static void construct(pointer p, T const& value) {
        _construct(p, value);
    }
    
    template <typename ...Args>
    static void construct(pointer p, Args&&... args) {
        _construct(p, TKF::forward<Args>(args)...);
    }

    static void deallocate(pointer p) {
        _deallocate(p);
    }

    static void destroy(pointer p) {
        _destroy(p);
    }
};

}

#endif