//file: Type_Traits.h
#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

namespace TKF
{

template <typename _Tp>
struct rmv_const {
    typedef _Tp type;
};

template <typename _Tp>
struct rmv_const<_Tp const> {
    typedef _Tp type;
};

template <typename _Tp>
struct rmv_volatile {
    typedef _Tp type;
};

template <typename _Tp>
struct rmv_volatile<_Tp volatile> {
    typedef _Tp type;
};

template <typename _Tp>
struct rmv_cv {
    typedef typename 
    rmv_const<typename rmv_volatile<_Tp>::type>::type
    type;
};

template <typename _Tp>
struct rmv_reference {
    typedef _Tp type;
};

template <typename _Tp>
struct rmv_reference<_Tp&> {
    typedef _Tp type;
};

template <typename _Tp>
struct rmv_reference<_Tp&&> {
    typedef _Tp type;
};

template <typename _Tp>
constexpr typename rmv_reference<_Tp>::type&&
move(_Tp&& _t) noexcept{
    return static_cast<typename rmv_reference<_Tp>::type&&>(_t);
}

template <typename _Tp>
_Tp&& forward(typename rmv_reference<_Tp>::type& _t) noexcept {
    return static_cast<_Tp&&>(_t);
}

template <typename _Tp>
_Tp&& forward(typename rmv_reference<_Tp>::type&& _t) noexcept {
    return static_cast<_Tp&&>(_t);
}

template <typename _Tp>
struct add_const {
    typedef _Tp const type;
};

template <typename T, T v>
struct int_constant {
    static const T value = v;
    typedef T value_type;
    typedef int_constant<T, v> type;
};

typedef int_constant<bool, true> true_type;
typedef int_constant<bool, false> false_type;

template <typename>
struct _is_integral_helper : public false_type{};

template <>
struct _is_integral_helper<bool> : public true_type{};

template <>
struct _is_integral_helper<char> : public true_type{};

template <>
struct _is_integral_helper<long long> : public true_type{};

template <>
struct _is_integral_helper<int> : public true_type{};

template <typename _Tp>
struct is_integral : public _is_integral_helper<typename rmv_cv<_Tp>::type>::type{};

template <typename _Tp>
struct is_const : public false_type{};

template <typename _Tp>
struct is_const<_Tp const> : public true_type{};

template <typename _Tp>
struct is_volatile : public false_type{};

template <typename _Tp>
struct is_volatile<_Tp volatile> : public true_type{};

}

#endif //!TYPE_TRAITS_H