//file: Fibonacci Heap.h
#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include"Allocator.h"
#include"Utility.h"
#include"Iterator.h"
#include<cmath>

namespace TKF {

#define phi 1.61803
#define logphi(n) log2(n) / log2(phi)
#define D(n) (unsigned int)(logphi(n)) + 1

template <typename T, bool> struct FIBH_value_traits_;
template <typename T> struct FIBH_value_traits;
template <typename T> struct FIBH_node_traits;
template <typename T> struct FIBH_traits;

template <typename T> struct FIBH_node_base;
template <typename T> struct FIBH_node;

typedef bool FIBH_mark_type;
static const FIBH_mark_type marked = true;
static const FIBH_mark_type unmarked = false;

template <typename T, bool>
struct FIBH_value_traits_ {
    typedef T key_type;
    typedef T value_type;
    typedef T info_type;

    template <typename _Tp>
    static const key_type& get_key(_Tp const& key) {
        return key;
    }

    template <typename _Tp>
    static void change_key (_Tp& old_key, _Tp const& new_key) {
        old_key = new_key;
    }

    template <typename _Tp>
    static const value_type&  get_value(_Tp const& value) {
        return value;
    }
};

template <typename T>
struct FIBH_value_traits_<T, true> {
    typedef typename TKF::rmv_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                            info_type;
    typedef T                                                  value_type;

    template <typename _Tp>
    static const key_type& get_key(_Tp const& value) {
        return value.first;
    }

    template <typename _Tp>
    static void change_key (_Tp& old_value, _Tp const& new_value) {
        old_value.first = new_value.first;
    }

    template <typename _Tp>
    static const value_type&  get_value(_Tp const& value) {
        return value;
    }
};

template <typename T>
struct FIBH_value_traits {
    static constexpr bool is_map = TKF::is_pair<T>::value;

    typedef FIBH_value_traits_<T, is_map>           value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::info_type   info_type;
    typedef typename value_traits_type::value_type  value_type;

    template <typename _Tp>
    static const key_type& get_key(_Tp const& value) {
        return value_traits_type::get_key(value);
    }

    template <typename _Tp>
    static void change_key (_Tp& old_key, _Tp const& new_key) {
        value_traits_type::change_key(old_key, new_key);
    }

    template <typename _Tp>
    static const value_type& get_value(_Tp const& value) {
        return value_traits_type::get_value(value);
    }
};

template <typename T>
struct FIBH_node_traits {
    typedef FIBH_mark_type                     mark_type;
    typedef unsigned int                       degree_type;

    typedef FIBH_value_traits<T>               value_traits;
    typedef typename value_traits::key_type    key_type;
    typedef typename value_traits::value_type  value_type;
    typedef typename value_traits::info_type   info_type;

    typedef FIBH_node_base<T>*                 base_ptr;
    typedef FIBH_node<T>*                      node_ptr;
};

template <typename T>
struct FIBH_node_base {
    typedef FIBH_node_traits<T>                node_traits;

    typedef typename node_traits::mark_type    mark_type;
    typedef typename node_traits::degree_type  degree_type;
    typedef typename node_traits::key_type     key_type;
    typedef typename node_traits::value_type   value_type;
    typedef typename node_traits::info_type    info_type;
    typedef typename node_traits::base_ptr     base_ptr;
    typedef typename node_traits::node_ptr     node_ptr; 

    mark_type   mark;
    degree_type degree;
    base_ptr    parent;
    base_ptr    left;
    base_ptr    right;
    base_ptr    child;


    base_ptr get_base_ptr() {
        return &*this;
    }

    node_ptr get_node_ptr() {
        return reinterpret_cast<node_ptr>(&*this);
    }
};

template <typename T>
struct FIBH_node : public FIBH_node_base<T> {
    typedef FIBH_node_base<T>* base_ptr;
    typedef FIBH_node<T>*      node_ptr;

    T value;

    base_ptr get_base_ptr() {
        return static_cast<base_ptr>(&*this);
    }

    node_ptr get_node_ptr() {
        return &*this;
    }
};

template <typename T>
struct FIBH_traits {
    typedef FIBH_value_traits<T>                value_traits;

    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::value_type   value_type;
    typedef typename value_traits::info_type     info_type;

    typedef value_type*                         pointer;
    typedef value_type&                         reference;
    typedef const pointer                       const_pointer;
    typedef const reference                     const_reference;

    typedef FIBH_node_base<T>                   base_type;
    typedef FIBH_node<T>                        node_type;
    typedef FIBH_node_base<T>*                  base_ptr;
    typedef FIBH_node<T>*                       node_ptr;
};

template <typename T, typename COMP>
class FIBHeap {
public:
    typedef FIBH_traits<T>                      heap_traits;
    typedef FIBH_value_traits<T>                value_traits;

    typedef typename heap_traits::base_type     base_type;
    typedef typename heap_traits::node_type     node_type;
    typedef typename heap_traits::base_ptr      base_ptr;
    typedef typename heap_traits::node_ptr      node_ptr;
    typedef typename heap_traits::key_type      key_type;
    typedef typename heap_traits::value_type    value_type;

    typedef COMP    key_compare;

    typedef TKF::allocator<T>                   data_allocator;
    typedef TKF::allocator<base_type>           base_allocator;
    typedef TKF::allocator<node_type>           node_allocator;

    typedef typename data_allocator::pointer        pointer;
    typedef typename data_allocator::const_pointer  const_pointer;
    typedef typename data_allocator::reference      reference;
    typedef typename data_allocator::const_reference const_reference;
    typedef typename data_allocator::size_type      size_type;
    typedef typename data_allocator::difference_type difference_type;

    data_allocator get_allocator() const { return node_allocator(); }
    key_compare key_comp() const { return _comp; }

protected:
    base_ptr _top;
    size_type _num;
    key_compare _comp;

public:
    FIBHeap() {
        _init();
    }
    
    FIBHeap(FIBHeap const& rhs) {
        _init();
        if (rhs._num != 0)
            _top = _copy(rhs.top());
        _num = rhs._num;
        _comp = rhs._num;
    }

    FIBHeap(FIBHeap&& rhs) {
        _top = TKF::move(rhs._top);
        _num = rhs._num;
        _comp = rhs._comp;
        rhs._top = nullptr;
        rhs._num = 0;
    }

    FIBHeap& operator = (FIBHeap const& rhs) {
        if (this != &rhs) {
            clear();
            if (rhs._num != 0)
                _top = _copy(rhs.top());
            _num = rhs._num;
            _comp = rhs._num;
        }
        return *this;
    }

    FIBHeap& operator = (FIBHeap&& rhs) {
        if (this != &rhs) {
            clear();
            _top = TKF::move(rhs._top);
            _num = rhs._num;
            _comp = rhs._num;
            rhs._top = nullptr;
            rhs._num = 0;
        }
        return *this;
    }

    ~FIBHeap() { }

    base_ptr& top() const {
        return _top->child;
    }

    bool empty() const noexcept { 
        return _num == 0; 
    }

    size_type size() const noexcept { 
        return _num; 
    }

    size_type max_size() const noexcept { 
        return static_cast<size_type>(-1); 
    }

    template <typename ...Args>
    node_ptr emplace(Args&& ...args);

    node_ptr insert(value_type const& value);
    
    node_ptr insert(value_type&& value) {
        return emplace(TKF::move(value));
    }

    node_ptr extract();

    node_ptr decrease(base_ptr ptr, key_type key);

    void erase(base_ptr ptr);

    void clear();
private:
    void _init() {
        _top = base_allocator::allocate(1);
        _top->mark = unmarked;
        _top->parent = nullptr;
        _top->child = nullptr;
        _top->left = nullptr;
        _top->right = nullptr;
        _top->degree = 0;
        _num = 0;
    }

    template <typename ...Args>
    node_ptr _create(Args&&... args);
    node_ptr _clone(base_ptr ptr);
    base_ptr _copy(base_ptr from);

    void _destroy(node_ptr ptr);

    base_ptr _insert_list(base_ptr ptr, base_ptr head);

    void _consolidate();

    void _cut_list(base_ptr ptr, base_ptr head);
    void _cascading_cut(base_ptr ptr);

};

template <typename T, typename COMP>
template <typename ...Args>
typename FIBHeap<T, COMP>::node_ptr 
FIBHeap<T, COMP>::_create (Args&&... args) {
    auto tmp = node_allocator::allocate(1);
    try {
        data_allocator::construct(&tmp->value, TKF::forward<Args>(args)...);
        tmp->parent = nullptr;
        tmp->child = nullptr;
        tmp->left = tmp;
        tmp->right = tmp;
        tmp->mark = unmarked;
        tmp->degree = 0;
    }
    catch (...) {
        node_allocator::deallocate(tmp);
        throw;
    }
    return tmp;
}

template <typename T, typename COMP>
typename FIBHeap<T, COMP>::node_ptr
FIBHeap<T, COMP>::_clone (base_ptr ptr) {
    node_ptr tmp = _create(ptr->get_node_ptr()->value);
    tmp->mark = ptr->mark;
    return tmp;
}

template <typename T, typename COMP>
void FIBHeap<T, COMP>::_destroy (node_ptr ptr) {
    data_allocator::destroy(&ptr->value);
    node_allocator::deallocate(ptr);
}

template <typename T, typename COMP>
template <typename ...Args>
typename FIBHeap<T, COMP>::node_ptr
FIBHeap<T, COMP>::emplace (Args&&... args) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1,
        "FIBHeap<T, COMP> size is out of range");
    node_ptr ptr = _create(TKF::forward<Args>(args)...);
    _insert_list(ptr, _top);
    if (_comp(value_traits::get_key(ptr->value), 
        value_traits::get_key(top()->get_node_ptr()->value))) {
            _top->child = ptr;
        }
    ++_num;
    return ptr;
}

template <typename T, typename COMP>
typename FIBHeap<T, COMP>::node_ptr
FIBHeap<T, COMP>::insert (value_type const& value) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1,
        "FIBHeap<T, COMP> size is out of range");
    node_ptr ptr = _create(value);
    _insert_list(ptr, _top);
    if (_comp(value_traits::get_key(value), 
        value_traits::get_key(top()->get_node_ptr()->value))) {
            _top->child = ptr;
        }
    ++_num;
    return ptr;
}

template <typename T, typename COMP>
typename FIBHeap<T, COMP>::base_ptr
FIBHeap<T, COMP>::_insert_list (base_ptr ptr, base_ptr head) {
    if (head->child == nullptr) {
        head->child = ptr;
        ptr->left = ptr;
        ptr->right = ptr;
    }
    else {
        auto x = head->child;
        if (_comp(value_traits::get_key(ptr->get_node_ptr()->value),
            value_traits::get_key(x->get_node_ptr()->value))) {
                x->left->right = ptr;
                ptr->left = x->left;
                x->left = ptr;
                ptr->right = x;
            }
        else {
            x->right->left = ptr;
            ptr->right = x->right;
            x->right = ptr;
            ptr->left = x;
        }
    }
    ptr->parent = head;
    ++head->degree;
    return ptr;
}

template <typename T, typename COMP>
typename FIBHeap<T, COMP>::node_ptr
FIBHeap<T, COMP>::extract() {
    base_ptr x = top()->child, z;
    node_ptr y = top()->get_node_ptr();
    if (x != nullptr) {
        x->parent = nullptr;
        for (int i = 1; i <= y->degree; ++i) {
            z = x->right;
            z->left = x->left;
            z->left->right = z;
            _insert_list(x, _top);
            x = z;
        }
        y->degree = 0;
        y->child = nullptr;
    }
    y->left->right = y->right;
    y->right->left = y->left;
    if (y == y->right) {
        _top->child = nullptr;
        _top->degree = 0;
    }
    else {
        _top->child = y->right;
        y->right = y;
        y->left = y;
        y->parent = nullptr;
        --_top->degree;
        _consolidate();
    }
    --_num;
    return y;
}

template <typename T, typename COMP>
void FIBHeap<T, COMP>::_consolidate() {
    auto n = D(_num);
    auto aux_array = TKF::allocator<base_ptr>::allocate(n);
    for (int i = 0; i < n; i++) {
        aux_array[i] = nullptr;
    }
    base_ptr x, y = top(), z;
    for (int i = 1; i <= _top->degree; ++i) {
        size_type d = y->degree;
        x = y->right;
        while (aux_array[d] != nullptr && aux_array[d] != y) {
            z = aux_array[d];
            if (!_comp(value_traits::get_key(y->get_node_ptr()->value),
                value_traits::get_key(z->get_node_ptr()->value))) {
                    auto tmp = z;
                    z = y;
                    y = tmp;
                }
            z->left->right = z->right;
            z->right->left = z->left;
            _insert_list(z, y);
            z->mark = unmarked;
            aux_array[d] = nullptr;
            ++d;
        }
        aux_array[d] = y;
        if (i == _top->degree) break;
        y = x;
    }
    _top->child = y;
    _top->degree = 1;
    x = y;
    while (y->right != x) {
        y = y->right;
        if (_comp(value_traits::get_key(y->get_node_ptr()->value),
            value_traits::get_key(top()->get_node_ptr()->value))) {
                _top->child = y;
            }
        ++_top->degree;
    }
    _top->child->parent = _top;
}

template <typename T, typename COMP>
void FIBHeap<T, COMP>::_cut_list (base_ptr ptr, base_ptr head) {
    if (ptr->right == ptr) {
        head->child = nullptr;
    }
    else {
        if (head->child == ptr) {
            head->child = ptr->right;
        }
        ptr->right->left = ptr->left;
        ptr->left->right = ptr->right;
    }
    _insert_list(ptr, _top);
    ptr->mark = unmarked;
    --head->degree;
}

template <typename T, typename COMP>
void FIBHeap<T, COMP>::_cascading_cut (base_ptr ptr) {
    auto z = ptr->parent;
    if (z != nullptr && z != _top) {
        if (ptr->mark == unmarked) {
            ptr->mark = marked;
        }
        else {
            _cut_list(ptr, z);
            _cascading_cut(z);
        }
    }
}

template <typename T, typename COMP>
typename FIBHeap<T, COMP>::node_ptr
FIBHeap<T, COMP>::decrease (base_ptr ptr, key_type key) {
    THROW_OUT_OF_RANGE_IF(!_comp(key, value_traits::
        get_key(ptr->get_node_ptr()->value)),
        "FIBHeap<T, COMP>::decrease: key is larger than before");
    value_traits::change_key(ptr->get_node_ptr()->value, key);
    auto y = ptr->parent;
    if (y != nullptr && y != _top && 
        _comp(key, value_traits::get_key(y->get_node_ptr()->value))) {
            _cut_list(ptr, y);
            _cascading_cut(y);
        }
    if (_comp(key, value_traits::get_key(top()->get_node_ptr()->value))) {
        ptr->parent = _top;
        _top->child = ptr;
    }
    return ptr->get_node_ptr();
}

template <typename T, typename COMP>
void FIBHeap<T, COMP>::erase (base_ptr ptr) {
    decrease(ptr, value_traits::get_key(top()->get_node_ptr()->value));
    _top->child = ptr;
    _destroy(extract());
}

}

#endif //!FIBONACCI_HEAP_H