//file: Iterator.h
#ifndef ITERATOR_H
#define ITERATOR_H

#include<cstddef>
#include"Type_Traits.h"

namespace TKF {

//only read
struct _input_iterator {};
//only write
struct _output_iterator {};
//move forward
struct _forward_iterator : public _input_iterator {};
//move bidirectionally
struct _bidirectional_iterator : public _forward_iterator {};
//move at random
struct _random_iterator : public _bidirectional_iterator {};

template <typename T, typename U>
struct input_iterator {
    typedef _input_iterator iterator_category;
    typedef T               value_type;
    typedef U               difference_type;
    typedef T*              pointer;
    typedef T&              reference;
};

template <typename T, typename U>
struct output_iterator {
    typedef _output_iterator iterator_category;
    typedef void             value_type;
    typedef void             difference_type;
    typedef void             pointer;
    typedef void             reference;
};

template <typename T, typename U>
struct forward_iterator {
    typedef _forward_iterator   iterator_category;
    typedef T                   value_type;
    typedef U                   difference_type;
    typedef T*                  pointer;
    typedef T&                  reference;
};

template <typename T, typename U>
struct bidirectional_iterator {
    typedef _bidirectional_iterator iterator_category;
    typedef T                       value_type;
    typedef U                       difference_type;
    typedef T*                      pointer;
    typedef T&                      reference;
};

template <typename T, typename U>
struct random_iterator {
    typedef _random_iterator    iterator_category;
    typedef T                   value_type;
    typedef U                   difference_type;
    typedef T*                  pointer;
    typedef T&                  reference;
};

template <typename C, typename T, typename U = ptrdiff_t,
    typename P = T*, typename R = T&>
struct iterator {
    typedef C iterator_category;
    typedef T value_type;
    typedef U difference_type;
    typedef P pointer;
    typedef R reference;
};

template <typename ITER>
struct iterator_traits {
    typedef typename ITER::iterator_category    iterator_category;
    typedef typename ITER::value_type           value_type;
    typedef typename ITER::difference_type      difference_type;
    typedef typename ITER::pointer              pointer;
    typedef typename ITER::reference            reference;
};

template <typename T>
struct iterator_traits<T*> {
    typedef _random_iterator    iterator_category;
    typedef T                   value_type;
    typedef ptrdiff_t           difference_type;
    typedef T*                  pointer;
    typedef T&                  reference;
};

template <typename T>
struct iterator_traits<const T*> {
    typedef _random_iterator    iterator_category;
    typedef T                   value_type;
    typedef ptrdiff_t           difference_type;
    typedef const T*                  pointer;
    typedef const T&                  reference;
};

template <typename ITER>
inline typename iterator_traits<ITER>::iterator_category
    iterator_category(ITER const&) {
        typedef typename iterator_traits<ITER>::iterator_category category;
        return category();
}

template <typename ITER>
inline typename iterator_traits<ITER>::difference_type*
    difference_type(ITER const&) {
        typedef typename iterator_traits<ITER>::difference_type type;
        return static_cast<type*>(0);
}

template <typename ITER>
inline typename iterator_traits<ITER>::value_type*
    value_type(ITER const&) {
        typedef typename iterator_traits<ITER>::value_type type;
        return static_cast<type*>(0);
}

template <typename INPUTITER>
inline typename iterator_traits<INPUTITER>::difference_type 
    _distance(INPUTITER first, INPUTITER last, _input_iterator) {
        typename iterator_traits<INPUTITER>::difference_type n = 0;
        while(first ^ last) {
            ++first;
            ++n;
        }
        return n;
}

template <typename RANDOMITER>
inline typename iterator_traits<RANDOMITER>::difference_type
    _distance(RANDOMITER first, RANDOMITER last, _random_iterator) {
        return last - first;
}

template <typename ITER>
inline typename iterator_traits<ITER>::difference_type
    distance(ITER first, ITER last) {
        typedef typename iterator_traits<ITER>::iterator_category category;
        return _distance(first, last, category());
}

template <typename INPUTITER, typename U>
inline void _advance(INPUTITER& iter, U n, _input_iterator) {
    while(n--)
        ++iter;
}

template <typename BIITER, typename U>
inline void _advance(BIITER& iter, U n, _bidirectional_iterator) {
    if (n >= 0) {
        while (n--) {
            ++iter;
        } 
    }    
    else {
        while (n++) {
            --iter;
        }
    }
        
}

template <typename RANDOMITER, typename U>
inline void _advance(RANDOMITER& iter, U n, _random_iterator) {
    iter += n;
}

template <typename ITER, typename U>
inline void advance(ITER& iter, U n) {
    _advance(iter, n, iterator_category(iter));
}

template <typename ITER>
class reverse_iterator {
private:
    ITER current;

public:
    typedef iterator_traits<ITER>                       iterator_traits;
    typedef typename iterator_traits::iterator_category iterator_category;
    typedef typename iterator_traits::value_type        value_type;
    typedef typename iterator_traits::difference_type   difference_type;
    typedef typename iterator_traits::pointer           pointer;
    typedef typename iterator_traits::reference         reference;

    typedef ITER                                        iterator_type;
    typedef reverse_iterator<ITER>                      self;

public:
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type iter) : current(iter) {}
    reverse_iterator(self const& rhs) : current(rhs.current) {}

public:
    iterator_type base() const {
        return current;
    }    
    reference operator *() const {
        auto tmp = current;
        return *(--tmp);
    }
    pointer operator ->() const{
        return &(operator*());
    }

    self& operator ++ () {
        --current;
        return *this;
    }

    self operator ++ (int) {
        self tmp(current);
        --current;
        return tmp;
    }

    self& operator -- () {
        ++current;
        return *this;
    }

    self operator -- (int) {
        self tmp(current);
        ++current;
        return tmp;
    }

    self& operator += (difference_type n) {
        current -= n;
        return *this;
    }

    self operator + (difference_type n) const {
        return self(current - n);
    }

    self& operator -= (difference_type n) {
        current += n;
        return *this;
    }

    self operator - (difference_type n) const {
        return self(current + n);
    }
};

template <typename ITER>
typename reverse_iterator<ITER>::difference_type 
    operator - (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
        return rhs.base() - lhs.base();
}

template <typename ITER>
bool operator == (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
    return lhs.base() == rhs.base();    
}

template <typename ITER>
bool operator != (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
    return lhs.base() != rhs.base() ;    
}

template <typename ITER>
bool operator < (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
    return lhs.base() < rhs.base();    
}

template <typename ITER>
bool operator >= (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
    return !(lhs.base() < rhs.base());    
}

template <typename ITER>
bool operator > (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
    return lhs.base() > rhs.base();    
}

template <typename ITER>
bool operator <= (reverse_iterator<ITER> const& lhs, reverse_iterator<ITER> const rhs) {
    return !(lhs.base() > rhs.base());    
}

}


#endif // !ITERATOR_H