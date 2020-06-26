//file: Utility.h
#ifndef UTILITY_H
#define UTILITY_H

#include"Type_Traits.h"

namespace TKF {

template <typename T1, typename T2>
struct pair {
    typedef pair<T1, T2>    Pair;
    typedef Pair*           PairPtr;
    typedef T1              first_type;
    typedef T2              second_type;

    first_type first;
    second_type second;

    pair () = default;
    pair (first_type const& _first, second_type const& _second)
        : first(_first), second(_second) {} 

    pair (Pair const& rhs) {
        first = rhs.first;
        second = rhs.second;
    }
    pair (Pair&& rhs) {
        first = TKF::move(rhs.first);
        second = TKF::move(rhs.second);
    }
    pair& operator = (Pair const& rhs) {
        if(this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }
    pair& operator = (Pair&& rhs) {
        if(this != &rhs) {
            first = TKF::move(rhs.first);
            second = TKF::move(rhs.second);
        }
        return *this;
    }

    template <typename _T1, typename _T2>
    pair& operator = (pair<_T1, _T2> const& rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }

    template <typename _T1, typename _T2>
    pair& operator = (pair<_T1, _T2>&& rhs) {
        first = TKF::move(rhs.first);
        second = TKF::move(rhs.second);
        return *this;
    }

    ~pair() = default;

    void swap(pair& rhs) {
        if(this != &rhs) {
            first_type f = TKF::move(first);
            first = TKF::move(rhs.first);
            rhs.first = TKF::move(f);
            second_type s = TKF::move(second);
            second = TKF::move(rhs.second);
            rhs.second = TKF::move(s);
        }
    }

    bool operator == (Pair const& rhs) {
        return first == rhs.first && second == rhs.second;
    }

    bool operator != (Pair const& rhs) {
        return !(first == rhs.first && second == rhs.second);
    }

    bool operator < (Pair const& rhs) {
        return first < rhs.first || (first == rhs.first && second <rhs.second);
    }

    bool operator >= (Pair const& rhs) {
        return !(*this < rhs); 
    }

    bool operator > (Pair const& rhs) {
        return first > rhs.first || (first == rhs.first && second > rhs.second);
    }

    bool operator <= (Pair const& rhs) {
        return !(*this > rhs);
    }

    friend std::ostream& operator << (std::ostream& out, Pair const& rhs) {
        out << rhs.first << " " << rhs.second;
        return out;
    }
};

template <typename T1, typename T2>
pair<T1, T2> make_pair(T1 _first, T2 _second) {
    return pair<T1, T2>(_first, _second);
}

template <typename _Tp>
struct is_pair : public false_type{};

template <typename T1, typename T2>
struct is_pair<TKF::pair<T1, T2> > : public true_type{};

}

#endif //!UTILITY_H