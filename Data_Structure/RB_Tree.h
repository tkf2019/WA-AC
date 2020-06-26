#ifndef RB_TREE_H
#define RB_TREE_H

#include"Iterator.h"
#include"Allocator.h"
#include"Type_Traits.h"
#include"Utility.h"

namespace TKF {

template <typename T, bool> struct RBT_value_traits_;
template <typename T> struct RBT_value_traits;
template <typename T> struct RBT_node_traits;
template <typename T> struct RBT_traits;

template <typename T> struct RBT_node_base;
template <typename T> struct RBT_node;

template <typename T> struct RBT_iterator_base;
template <typename T> struct RBT_iterator;

template <typename T, typename COMP> class RBT;

typedef bool RBT_color_type;
static constexpr RBT_color_type RBT_color_red = true;
static constexpr RBT_color_type RBT_color_black = false;

typedef bool RBT_insert_type;
static constexpr RBT_insert_type RBT_left_insert = true;
static constexpr RBT_insert_type RBT_right_insert = false;

template <typename T, bool>
struct RBT_value_traits_ {
    typedef T key_type;
    typedef T value_type;
    typedef T map_type;

    template <typename _Tp>
    static const key_type& get_key(_Tp const& key) {
        return key;
    }

    template <typename _Tp>
    static const value_type& get_value(_Tp const& value) {
        return value;
    }
};

template <typename T>
struct RBT_value_traits_<T, true> {
    typedef typename TKF::rmv_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                            map_type;
    typedef T                                                  value_type;

    template <typename _Tp>
    static const key_type& get_key(_Tp const& value) {
        return value.first;
    }

    template <typename _Tp>
    static const value_type&  get_value(_Tp const& value) {
        return value;
    }
};

template <typename T>
struct RBT_value_traits {
    static constexpr bool is_map = TKF::is_pair<T>::value;

    typedef RBT_value_traits_<T, is_map>            value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::value_type  value_type;
    typedef typename value_traits_type::map_type    map_type;

    template <typename _Tp>
    static const key_type& get_key(_Tp const& value) {
        return value_traits_type::get_key(value);
    }

    template <typename _Tp>
    static const value_type& get_value(_Tp const& value) {
        return value_traits_type::get_value(value);
    }
};


template <typename T>
struct RBT_node_traits {
    typedef RBT_color_type                      color_type;

    typedef RBT_value_traits<T>                 value_traits;
    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::value_type   value_type;
    typedef typename value_traits::map_type     map_type;

    typedef RBT_node_base<T>*                   base_ptr;
    typedef RBT_node<T>*                        node_ptr;
};

template <typename T>
struct RBT_node_base {
    typedef RBT_color_type      color_type;
    typedef RBT_node_base<T>*   base_ptr;
    typedef RBT_node<T>*        node_ptr;

    base_ptr    parent;
    base_ptr    left;
    base_ptr    right;
    color_type  color;

    base_ptr get_base_ptr() {
        return &*this;
    }

    node_ptr get_node_ptr() {
        return reinterpret_cast<node_ptr>(&*this);
    }
};

template <typename T>
struct RBT_node : public RBT_node_base<T> {
    typedef RBT_node_base<T>*   base_ptr;
    typedef RBT_node<T>*        node_ptr;

    T value;

    base_ptr get_base_ptr() {
        return static_cast<base_ptr>(&*this);
    }

    node_ptr get_node_ptr() {
        return &*this;
    }
};

template <typename T>
struct RBT_traits {
    typedef RBT_value_traits<T>                 value_traits;

    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::value_type   value_type;
    typedef typename value_traits::map_type     map_type;

    typedef value_type*                         pointer;
    typedef value_type&                         reference;
    typedef const pointer                       const_pointer;
    typedef const reference                     const_reference;

    typedef RBT_node_base<T>                    base_type;
    typedef RBT_node<T>                         node_type;
    typedef RBT_node_base<T>*                   base_ptr;
    typedef RBT_node<T>*                        node_ptr;
};

template <typename T>
struct RBT_iterator_base 
    : public TKF::iterator<TKF::_bidirectional_iterator, T>{
    typedef typename RBT_traits<T>::base_ptr base_ptr;

    base_ptr ptr;

    RBT_iterator_base() : ptr(nullptr) {}

    base_ptr RBT_minimum (base_ptr ptr) {
        base_ptr link = ptr;
        while(link->left != nullptr) {
            link = link->left;
        }   
        return link;
    }

    base_ptr RBT_maximum (base_ptr ptr) {
        base_ptr link = ptr;
        while(link->right != nullptr) {
            link = link->right;
        }   
        return link;
    }

    void increase() {
        if(ptr->right != nullptr) {
            ptr = RBT_minimum(ptr->right);
        } 
        else {
            auto y = ptr->parent;
            while (y->right == ptr) {
                ptr = y;
                y = y->parent;
            } 
            if(ptr->right != y){
                ptr = y;
            }
        }
    }

    void decrease() {
        if (ptr->left != nullptr) {
            ptr = RBT_maximum(ptr->left);
        } 
        else {
            auto y = ptr->parent;
            while (y->left == ptr) {
                ptr = y;
                y = y->parent;
            }
            if(ptr->left != y){
                ptr = y;
            }
        }
    }
};

template <typename T>
struct RBT_iterator : public RBT_iterator_base<T> {
    typedef RBT_traits<T>       traits;
    
    typedef typename traits::pointer     pointer;
    typedef typename traits::reference   reference;
    typedef typename traits::base_ptr    base_ptr;
    typedef typename traits::node_ptr    node_ptr;

    typedef RBT_iterator<T>     iterator;
    typedef iterator            self;

    using RBT_iterator_base<T>::ptr;

    RBT_iterator() = default;
    RBT_iterator(base_ptr x) { 
        ptr = x; 
    }
    RBT_iterator(node_ptr x) { 
        ptr = x; 
    }
    RBT_iterator(iterator const& rhs) { 
        ptr = rhs.ptr; 
    }
    RBT_iterator(iterator&& rhs) { 
        ptr = TKF::move(rhs.ptr); 
    }

    self& operator = (iterator const& rhs) {
        ptr = rhs.ptr;
        return *this;
    }

    self& operator = (iterator&& rhs) {
        ptr = TKF::move(rhs.ptr);
        return *this;
    }

    reference operator *() const { 
        return ptr->get_node_ptr()->value; 
    }

    pointer operator ->() const { 
        return &(operator*()); 
    }

    self& operator ++ () {
        this->increase();
        return *this;
    }

    self operator ++ (int) {
        self tmp(*this);
        this->increase();
        return tmp;
    }

    self& operator -- () {
        this->decrease();
        return *this;
    }

    self operator -- (int) {
        self tmp(*this);
        this->decrease();
        return tmp;
    }

    self& operator += (ptrdiff_t n) {
        while (n--) {
            this->increase();
        }
        return *this;
    }

    self operator + (ptrdiff_t n) const {
        self tmp(*this);
        while (n--) {
            tmp.increase();
        }
        return tmp; 
    }

    self& operator -= (ptrdiff_t n) {
        while (n--) {
            this->decrease();
        }
        return *this;
    }

    self operator - (ptrdiff_t n) const {
        self tmp(*this);
        while (n--) {
            tmp.decrease();
        }
        return tmp; 
    }

    bool operator == (iterator const& rhs) {
        return rhs.ptr == ptr;
    }

    bool operator != (iterator const& rhs) {
        return rhs.ptr != ptr;
    }
};

template <typename T, typename COMP>
class RBT {
public:
    typedef RBT_traits<T>                           tree_traits;
    typedef RBT_value_traits<T>                     value_traits;

    typedef typename tree_traits::base_type         base_type;
    typedef typename tree_traits::node_type         node_type;
    typedef typename tree_traits::base_ptr          base_ptr;
    typedef typename tree_traits::node_ptr          node_ptr;
    typedef typename tree_traits::key_type          key_type;
    typedef typename tree_traits::value_type        value_type;

    typedef COMP                                    key_compare;

    typedef TKF::allocator<T>                       allocator_type;
    typedef TKF::allocator<base_type>               base_allocator;
    typedef TKF::allocator<node_type>               node_allocator;

    typedef typename allocator_type::pointer        pointer;
    typedef typename allocator_type::const_pointer  const_pointer;
    typedef typename allocator_type::reference      reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type      size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef RBT_iterator<T>                         iterator;
    typedef TKF::reverse_iterator<iterator>         reverse_iterator;
        
    allocator_type get_allocator() const { return node_allocator(); }
    key_compare key_comp() const { return _comp; }

private:
    base_ptr _head;
    size_type _num;
    key_compare _comp;

public:
    RBT() { 
        _init(); 
    }

    base_ptr& root() const noexcept { 
        return _head->parent; 
    }
    base_ptr& min() const noexcept { 
        return _head->right; 
    }
    base_ptr& max() const noexcept { 
        return _head->left; 
    }

    RBT(RBT const& rhs) {
        _init();
        if(rhs._num != 0) {
            _head->parent = _copy(rhs.root(), _head);
            _head->right = _minimum(root());
            _head->left = _maximum(root());
        }
        _num = rhs._num;
        _comp = rhs._comp;
    }

    RBT(RBT&& rhs) noexcept {
        _head = TKF::move(rhs._head);
        _num = rhs._num;
        _comp = rhs._comp;
        rhs._reset();
    }

    RBT& operator = (RBT const& rhs) {
        if(this != &rhs) {
            clear();
            if(rhs._num != 0) {
                _head->parent = _copy(rhs.root(), _head);
                _head->right = _minimum(root());
                _head->left = _maximum(root());
            }
            _num = rhs._num;
            _comp = rhs._comp;
        }
        return *this;
    }
    
    RBT& operator = (RBT&& rhs) {
        if(this != &rhs) {
            clear();
            _head = TKF::move(rhs._head);
            _num = rhs._num;
            _comp = rhs._comp;
            rhs._reset();
        }
        return *this;
    }

    ~RBT() { clear(); }
    
    friend bool operator == (RBT const& lhs, RBT const& rhs) {
        if (lhs._num != rhs._num) {
            return false;
        }
        else {
            auto iter = rhs.begin();
            for (auto i = lhs.begin(); i != lhs.end(); ++i) {
                if (*iter != *i) {
                    return false;
                }
                ++iter;
            }
            return true;
        }
    }
    friend bool operator < (RBT const& lhs, RBT const& rhs) {
        auto iter = rhs.begin();
        for (auto i = lhs.begin(); i != lhs.end(); ++i) {
            if (iter == rhs.end()) {
                break;
            } 
            if (*i < *iter) {
                return true;
            }
            else if (*i > *iter) {
                return false;
            }
            ++iter;
        }
        return (iter != rhs.end());
    }
    

public:
    //iterator 
    iterator begin() const noexcept {
        return min();
    }

    iterator end() const noexcept {
        return _head;
    }

    reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
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

    //emplace
    template <typename ...Args>
    iterator multi_emplace (Args&& ...args);

    template <typename ...Args>
    TKF::pair<iterator, bool> unique_emplace (Args&& ...args);

    template <typename ...Args>
    iterator multi_emplace_hint(iterator hint, Args&& ...args);

    template <typename ...Args>
    iterator unique_emplace_hint(iterator hint, Args&& ...args);

    //insert
    iterator multi_insert(value_type const& value);

    iterator multi_insert(value_type&& value) {
        return multi_emplace(TKF::move(value));
    }

    iterator multi_insert(iterator hint, value_type const& value) {
        return multi_emplace_hint(hint, value);
    }

    iterator multi_insert(iterator hint, value_type&& value) {
        return multi_emplace_hint(hint, TKF::move(value));
    }

    template <typename ITER>
    void multi_insert(ITER const& first, ITER const& last);

    TKF::pair<iterator, bool> unique_insert(value_type const& value);
    
    TKF::pair<iterator, bool> unique_insert(value_type&& value) {
        unique_insert(TKF::move(value));
    }

    iterator unique_insert(iterator hint, value_type const& value) {
        return unique_emplace_hint(hint, value);
    }

    iterator unique_insert(iterator hint, value_type&& value) {
        return unique_emplace_hint(hint, TKF::move(value));
    }

    template <typename ITER>
    void unique_insert(ITER const& first, ITER const& last);

    //erase
    iterator erase(iterator hint);

    size_type multi_erase(key_type const& key);
    size_type unique_erase(key_type const& key);

    void erase(iterator first, iterator last);

    void clear();

    //find
    iterator find(key_type const& key) const;
    iterator lower_bound(key_type const& key) const;
    iterator upper_bound(key_type const& key) const;

private:
    //helpers: untouchable and invisible for users
    void _init() {
        _head = base_allocator::allocate(1);
        _head->color = RBT_color_black;
        _head->parent = nullptr; //root() = nullptr;
        _head->left = _head; //max() = _head;
        _head->right = _head; //min() = _head;
        _num = 0;
    }

    void _reset() {
        _num = 0;
        _head = nullptr;
    }

    void _left_rotate(base_ptr ptr) noexcept;
    void _right_rotate(base_ptr ptr) noexcept;

    base_ptr _minimum(base_ptr const& ptr) noexcept;
    base_ptr _maximum(base_ptr const& ptr) noexcept;

    template <typename ...Args>
    node_ptr _create(Args&&... args);
    node_ptr _clone(base_ptr ptr);
    void _destroy(node_ptr ptr);

    TKF::pair<base_ptr, bool> _multi_insert_pos(key_type const& key);
    TKF::pair<TKF::pair<base_ptr, bool>, bool> _unique_insert_pos(key_type const& key);

    void _insert_fix(base_ptr ptr); 
    iterator _insert_value_at (base_ptr ptr, value_type const& value, RBT_insert_type insert) {
            return _insert_node_at(ptr, _create(value), insert);
    }
    iterator _insert_node_at(base_ptr ptr, node_ptr node, RBT_insert_type);

    iterator _multi_insert_hint(iterator hint, key_type key, value_type const& value){
        return _multi_insert_hint(hint, key, _create(value));
    }
    iterator _multi_insert_hint(iterator hint, key_type key, node_ptr node);
    iterator _unique_insert_hint(iterator hint, key_type key, value_type const& value) {
        return _unique_insert_hint(hint, key, _create(value));
    }
    iterator _unique_insert_hint(iterator hint, key_type key, node_ptr node);

    base_ptr _copy(base_ptr const& from, base_ptr ptr) const;
    void _erase_from(base_ptr from);
    void _transplant(base_ptr u, base_ptr v); 
    void _delete_fix(base_ptr ptr);
    void _erase(base_ptr ptr);
};

template <typename T, typename COMP>
bool operator == (RBT<T, COMP> const& lhs, RBT<T, COMP> const& rhs) {
    return lhs == rhs;
}

template <typename T, typename COMP>
bool operator != (RBT<T, COMP> const& lhs, RBT<T, COMP> const& rhs) {
    return !(lhs == rhs);
}
    
template <typename T, typename COMP>
bool operator >= (RBT<T, COMP> const& lhs, RBT<T, COMP> const& rhs) {
    return !(lhs < rhs);
}

template <typename T, typename COMP>
bool operator > (RBT<T, COMP> const& lhs, RBT<T, COMP> const& rhs) {
        return rhs < lhs;
}

template <typename T, typename COMP>
bool operator <= (RBT<T, COMP> const& lhs, RBT<T, COMP> const& rhs) {
        return !(rhs < lhs);
}

template <typename T, typename COMP>
template <typename ...Args>
typename RBT<T, COMP>::node_ptr
RBT<T, COMP>::_create (Args&&... args) {
    auto tmp = node_allocator::allocate(1);
    try {
        allocator_type::construct(&tmp->value, TKF::forward<Args>(args)...);
        tmp->left = nullptr;
        tmp->right = nullptr;
        tmp->parent = nullptr;
        tmp->color = RBT_color_red;
    }
    catch (...) {
        node_allocator::deallocate(tmp);
        throw;
    }
    return tmp;
}

template <typename T, typename COMP>
typename RBT<T, COMP>::node_ptr
RBT<T, COMP>::_clone (base_ptr ptr) {
    node_ptr tmp = _create(ptr->get_node_ptr()->value);
    tmp->color = ptr->color;
    tmp->left = nullptr;
    tmp->right = nullptr;
    return tmp;
}

template <typename T, typename COMP>
void RBT<T, COMP>::_destroy (node_ptr ptr) {
    allocator_type::destroy(&ptr->value);
    node_allocator::deallocate(ptr);
}

template <typename T, typename COMP>
typename RBT<T, COMP>::base_ptr
RBT<T, COMP>::_minimum (base_ptr const& ptr) noexcept {
    base_ptr link = ptr;
    while (link->left != nullptr) {
        link = link->left;
    }   
    return link;
}   

template <typename T, typename COMP>
typename RBT<T, COMP>::base_ptr
RBT<T, COMP>::_maximum (base_ptr const& ptr) noexcept {
    base_ptr link = ptr;
    while (link->right != nullptr) {
        link = link->right;
    }
    return link;
}

template <typename T, typename COMP>
template <typename ...Args>
typename RBT<T, COMP>::iterator 
RBT<T, COMP>::multi_emplace (Args&& ...args) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1
        , "RBT<T, COMP> size is out of range");
    node_ptr ptr = _create(TKF::forward<Args>(args)...);
    auto res =_multi_insert_pos(value_traits::
        get_key(ptr->get_node_ptr()->value));
    return _insert_node_at(res.first, ptr, res.second);
}

template <typename T, typename COMP>
template <typename ...Args>
TKF::pair<typename RBT<T, COMP>::iterator, bool>
RBT<T, COMP>::unique_emplace (Args&& ...args) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1
        , "RBT<T, COMP> size out of range");
    node_ptr ptr = _create(TKF::forward<Args>(args)...);
    auto res = _unique_insert_pos(value_traits::
        get_key(ptr->get_node_ptr()->value));
    if (!res.second) {
        _destroy(ptr);
        return TKF::make_pair(iterator(res.first.first), false);
    }
    return TKF::make_pair(_insert_node_at(res.first.first, ptr, res.first.second), true);
}

template <typename T, typename COMP>
template <typename ...Args>
typename RBT<T, COMP>::iterator 
RBT<T, COMP>::multi_emplace_hint (iterator hint, Args&&... args) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1
        , "RBT<T, COMP> size out of range");
    node_ptr ptr = _create(TKF::forward<Args>(args)...);
    key_type key = value_traits::get_key(ptr->get_node_ptr()->value);
    if (_num == 0) {
        return _insert_node_at(hint.ptr, ptr, RBT_left_insert);
    }
    if (hint == min()) {
        if(_comp(key, value_traits::get_key(*hint))) {
            return _insert_node_at(hint.ptr, ptr, RBT_left_insert);
        }
        auto res = _multi_insert_pos(key);
        return _insert_node_at(res.first, ptr, res.second);
    }
    else if (hint == max()) {
        if(!_comp(key, value_traits::get_key(*hint))) {
            return _insert_node_at(hint.ptr, ptr, RBT_right_insert);
        }
        auto res = _multi_insert_pos(key);
        return _insert_node_at(res.first, ptr, res.second);
    }
    return _multi_insert_hint(hint, key, ptr);
}

template <typename T, typename COMP>
template <typename ...Args>
typename RBT<T, COMP>::iterator
RBT<T, COMP>::unique_emplace_hint (iterator hint, Args&&... args) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1
        , "RBT<T, COMP> size out of range");
    node_ptr ptr = _create(TKF::forward<Args>(args)...);
    key_type key = value_traits::get_key(ptr->get_node_ptr()->value);
    if(_num == 0) {
        return _insert_node_at(hint.ptr, ptr, RBT_left_insert);
    }
    if(hint == min()) {
        if (_comp(key, value_traits::get_key(*hint))
            && key != value_traits::get_key(*hint)) {
                return _insert_node_at(hint.ptr, ptr, RBT_left_insert);
            }
        auto res = _unique_insert_pos(key);
        if(!res.second) {
            _destroy(ptr);
            return res.first.first;
        }
        return _insert_node_at(res.first.first, ptr, res.first.second);
    }
    else if (hint == max()) {
        if (!_comp(key, value_traits::get_key(*hint))) {
            return _insert_node_at(hint.ptr, ptr, RBT_left_insert);
        }
        auto res = _unique_insert_pos(key);
        if(!res.second) {
            _destroy(ptr);
            return iterator(res.first.first);
        }
        return _insert_node_at(res.first.first, ptr, res.first.second);
    }
    return _unique_insert_hint(hint, key, ptr);
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator
RBT<T, COMP>::multi_insert (value_type const& value) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1
        , "RBT<T, COMP> size out of range");
    auto res = _multi_insert_pos(value_traits::get_key(value));
    return _insert_value_at(res.first, value, res.second);
}

template <typename T, typename COMP>
TKF::pair<typename RBT<T, COMP>::iterator, bool>
RBT<T, COMP>::unique_insert (value_type const& value) {
    THROW_OUT_OF_RANGE_IF(_num > max_size() - 1
        , "RBT<T, COMP> size out of range");
    auto res = _unique_insert_pos(value_traits::get_key(value));
    if (!res.second) {
        return TKF::make_pair(iterator(res.first.first), false);
    }
    return TKF::make_pair(_insert_value_at(res.first.first, value, res.first.second), true);
}

template <typename T, typename COMP>
template <typename ITER>
void RBT<T, COMP>::multi_insert (ITER const& first, ITER const& last) {
    ITER ptr = first;
    difference_type n = TKF::distance(first, last);
    THROW_OUT_OF_RANGE_IF(_num > max_size() - n
        , "RBT<T, COMP> size is out of range");
    while (ptr != last) {
        multi_insert(end(), *ptr);
        ++ptr;
    }
}

template <typename T, typename COMP>
template <typename ITER>
void RBT<T, COMP>::unique_insert (ITER const& first, ITER const& last) {
    ITER ptr = first;
    difference_type n = TKF::distance(first, last);
    THROW_OUT_OF_RANGE_IF(_num > max_size() - n
        , "RBT<T, COMP> size is out of range");
    while (ptr != last) {
        unique_insert(end(), *ptr);
        ++ptr;
    }
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator
RBT<T, COMP>::erase (iterator hint) {
    node_ptr ptr = hint.ptr->get_node_ptr();
    iterator next(ptr);
    ++next;
    _erase(hint.ptr);
    _destroy(ptr);
    --_num;
    return next;
}

template <typename T, typename COMP>
typename RBT<T, COMP>::size_type
RBT<T, COMP>::multi_erase (key_type const& key) {
    size_type n = 0;
    if (find(key) != end()) {
        iterator iter = begin(), tmp;
        while (iter != end()) {
            if(key == value_traits::
                get_key(iter.ptr->get_node_ptr()->value)) {
                    tmp = iter++;
                    erase(tmp);
                    ++n;
                    continue;
                }
            ++iter;
        }
    }
    return n;
}

template <typename T, typename COMP>
typename RBT<T, COMP>::size_type
RBT<T, COMP>::unique_erase (key_type const& key) {
    iterator target = find(key);
    if (target != end()) {
        erase(target);
        return 1;
    }
    return 0;
}

template <typename T, typename COMP>
void RBT<T, COMP>::erase (iterator first, iterator last) {
    if(first == begin() && last == end()) {
        clear();
    }
    else {
        iterator tmp;
        while (first != last) {
            tmp = first++;
            erase(tmp);
        }
    }
}

template <typename T, typename COMP>
void RBT<T, COMP>::clear () {
    if (_num != 0) {
        _erase_from(root());
        min() = _head;
        max() = _head;
        root() = nullptr;
        _num = 0;
    }
}
template <typename T, typename COMP>
typename RBT<T, COMP>::iterator 
RBT<T, COMP>::find (key_type const& key) const {
    base_ptr ptr = root();
    base_ptr link = _head;
    while (ptr != nullptr) {
        link = ptr;
        if (key == value_traits::get_key(link->get_node_ptr()->value)) {
            return iterator(link);
        }
        if(_comp(key, value_traits::get_key(link->get_node_ptr()->value))) {
            ptr = ptr->left;
        }
        else {
            ptr = ptr->right;
        }
    }
    return end();
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator 
RBT<T, COMP>::lower_bound (key_type const& key) const {
    base_ptr ptr = root();
    base_ptr link = _head;
    while (ptr != nullptr) {
        if(_comp(key, value_traits::get_key(ptr->get_node_ptr()->value))) {
            link = ptr;
            ptr = ptr->left;
        }
        else {
            ptr = ptr->right;
        }
    }
    return iterator(link);
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator 
RBT<T, COMP>::upper_bound (key_type const& key) const{
    base_ptr ptr = root();
    base_ptr link = _head;
    while (ptr != nullptr) {
        if(_comp(key, value_traits::get_key(ptr->get_node_ptr()->value))) {
            ptr = ptr->left;
        }
        else {
            link = ptr;
            ptr = ptr->right;
        }
    }
    return iterator(link);
}

template <typename T, typename COMP>
TKF::pair<typename RBT<T, COMP>::base_ptr, bool>
RBT<T, COMP>::_multi_insert_pos (key_type const& key) {
    base_ptr ptr = root();
    base_ptr link = _head;
    RBT_insert_type insert = RBT_left_insert;
    while (ptr != nullptr) {
        link = ptr;
        insert = _comp(key, value_traits::
            get_key(ptr->get_node_ptr()->value));
        ptr = (insert == RBT_left_insert) ? ptr->left : ptr->right;
    }
    return TKF::make_pair(link, insert);
}

template <typename T, typename COMP>
TKF::pair<TKF::pair<typename RBT<T, COMP>::base_ptr, bool>, bool>
RBT<T, COMP>::_unique_insert_pos (key_type const& key) {
    base_ptr ptr = root();
    base_ptr link = _head;
    RBT_insert_type insert = RBT_left_insert;
    while (ptr != nullptr) {
        link = ptr;
        insert = _comp(key, value_traits::
            get_key(ptr->get_node_ptr()->value));
        ptr = (insert == RBT_left_insert) ? ptr->left : ptr->right;
    }
    if (insert == RBT_left_insert) {
        if (link == _head || key != value_traits::
            get_key(link->get_node_ptr()->value)) {
                return TKF::make_pair(TKF::make_pair(link, insert), true);
            }
        return TKF::make_pair(TKF::make_pair(link, insert), false);
    }
    if (link == root() || key !=  value_traits::
        get_key(link->parent->get_node_ptr()->value)) {
            return TKF::make_pair(TKF::make_pair(link, insert), true);
        }
    return TKF::make_pair(TKF::make_pair(link, insert), false);
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator RBT<T, COMP>::
_insert_node_at (base_ptr ptr, node_ptr node, RBT_insert_type insert) {
    node->parent = ptr;
    base_ptr base = node->get_base_ptr();
    if (ptr == _head) {
        _head->parent = base;
        _head->right = base;
        _head->left = base;
    }
    else if (insert == RBT_left_insert) {
        ptr->left = base;
        if (min() == ptr) {
            _head->right = base;
        } 
    }
    else {
        ptr->right = base;
        if (max() == ptr) {
            _head->left = base;
        }
    }
    _insert_fix(base);
    ++_num;
    return iterator(node);
}

template <typename T, typename COMP>
void RBT<T, COMP>::_left_rotate(base_ptr ptr) noexcept {
    base_ptr x = ptr, y = ptr->right;
    if (y == nullptr) {
        return;
    }
    x->right = y->left;
    y->parent = x->parent;
    if(y->left != nullptr) {
        y->left->parent = x;
    }
    if (x == root()) {
        x->parent->parent = y;
    } 
    else if (x == x->parent->left) {
        x->parent->left = y;
    } 
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

template <typename T, typename COMP>
void RBT<T, COMP>::_right_rotate(base_ptr ptr) noexcept {
    base_ptr x = ptr, y = ptr->left;
    if (y == nullptr) {
        return;
    }
    x->left = y->right;
    y->parent = x->parent;
    if (y->right != nullptr) {
        y->right->parent = x;
    }
    if (x == root()) {
        x->parent->parent = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

template <typename T, typename COMP>
void RBT<T, COMP>::_insert_fix(base_ptr ptr){
    base_ptr x = ptr, y;
    while (x->parent->color == RBT_color_red) {
        if (x->parent == x->parent->parent->left) {
            y = x->parent->parent->right;
            if(y != nullptr) {
                if (y->color == RBT_color_red) {
                    x->parent->color = RBT_color_black;
                    x->parent->parent->color = RBT_color_red;
                    y->color = RBT_color_black;
                    x = x->parent->parent;
                }
            }
            else if (x == x->parent->right) {
                x = x->parent;
                _left_rotate(x);
            }
            if (x != root() && x->parent->parent != _head) {
                x->parent->color = RBT_color_black;
                x->parent->parent->color = RBT_color_red;
                _right_rotate(x->parent->parent);
            } 
        } 
        else {
            y = x->parent->parent->left;
            if(y != nullptr) {
                if (y->color == RBT_color_red) {
                    x->parent->color = RBT_color_black;
                    x->parent->parent->color = RBT_color_red;
                    y->color = RBT_color_black;
                    x = x->parent->parent;
                }
            }
            else if (x == x->parent->left) {
                x = x->parent;
                _right_rotate(x);
            }
            if (x != root() && x->parent->parent != _head) {
                x->parent->color = RBT_color_black;
                x->parent->parent->color = RBT_color_red;
                _left_rotate(x->parent->parent);
            }
        }
    }
    root()->color = RBT_color_black;
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator RBT<T, COMP>::
_multi_insert_hint (iterator hint, key_type key, node_ptr node) {
    base_ptr ptr = hint.ptr;
    iterator before = hint;
    --before;
    base_ptr bptr = before.ptr;
    if (!_comp(key, value_traits::get_key(*before)) 
        && !_comp(value_traits::get_key(*hint), key)) {
            if (bptr->right == nullptr) {
                return _insert_node_at(bptr, node, RBT_right_insert);
            }
            else if (ptr->left == nullptr) {
                return _insert_node_at(ptr, node, RBT_left_insert);
            }
        }
    auto pos = _multi_insert_pos(key);
    return _insert_node_at(pos.first, node, pos.second);
}

template <typename T, typename COMP>
typename RBT<T, COMP>::iterator RBT<T, COMP>::
_unique_insert_hint (iterator hint, key_type key, node_ptr node) {
    base_ptr ptr = hint.ptr;
    iterator before = hint;
    --before;
    base_ptr bptr = before.ptr;
    if (!_comp(key, value_traits::get_key(*before)) 
        && !_comp(value_traits::get_key(*hint), key)) {
            if (bptr->right == nullptr) {
                return _insert_node_at(bptr, node, RBT_right_insert);
            }
            else if (ptr->left == nullptr) {
                return _insert_node_at(ptr, node, RBT_left_insert);
            }
        }
    auto pos = _unique_insert_pos(key);
    if (!pos.second) {
        _destroy(node);
        return iterator(pos.first.first);
    }
    return _insert_node_at(pos.first.first, node, pos.first.second);
}

template <typename T, typename COMP>
typename RBT<T, COMP>::base_ptr 
RBT<T, COMP>::_copy (base_ptr const& from, base_ptr ptr) const {
    base_ptr link = from;
    base_ptr head = ptr;
    node_ptr _root = _clone(from);
    _root->parent = ptr;
    try {
        if (from->right != nullptr) {
            _root->right = _copy(from->right, _root);
        }
        head = _root;
        link = link->left;
        while (link != nullptr) {
            node_ptr tmp = _clone(link);
            head->left = tmp;
            tmp->parent = head;
            if(link->right != nullptr) {
                tmp->right = _copy(link->right, tmp);
            }
            head = tmp;
            link = link->left;
        }
    }
    catch (...) {
        _erase_from(_root);
        throw;
    }
    return _root;
}

template <typename T, typename COMP>
void RBT<T, COMP>::_erase_from (base_ptr from) {
    base_ptr ptr = from;
    while (ptr != nullptr) {
        _erase_from(ptr->right);
        base_ptr tmp = ptr->left;
        _destroy(ptr->get_node_ptr());
        ptr = tmp;
    }
    return;
}


template <typename T, typename COMP>
void RBT<T, COMP>::_transplant (base_ptr u, base_ptr v) {
    if (u->parent == _head) {
        root() = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != nullptr) {
        v->parent = u->parent;
    }
}

template <typename T, typename COMP>
void RBT<T, COMP>::_delete_fix (base_ptr ptr){
    base_ptr x = ptr, y;
    while (x != root() && x->color == RBT_color_black) {
        if (x == x->parent->left) {
            y = x->parent->right;
            if (y != nullptr) {
                if (y->color == RBT_color_red) {
                    x->parent->color = RBT_color_red;
                    y->color = RBT_color_black;
                    _left_rotate(x->parent);
                    y = x->parent->right;
                }
                if (y->left == nullptr && y->right != nullptr) {
                    if (y->right->color == RBT_color_black) {
                        y->color = RBT_color_red;
                        x = x->parent;
                    }
                }
                else if (y->left != nullptr && y->right != nullptr) {
                    if (y->left->color == RBT_color_black && y->right->color == RBT_color_black) {
                        y->color = RBT_color_red;
                        x = x->parent;
                    }
                    else if (y->right->color == RBT_color_black) {
                        y->color = RBT_color_red;
                        y->left->color = RBT_color_black;
                        _right_rotate(y);
                        y = x->parent->right;
                    }
                }
                y->color = x->parent->color;
                if (y->right != nullptr) {
                    y->right->color = RBT_color_black;
                }
            }
            if (x != root()) {
                x->parent->color = RBT_color_black;
                _left_rotate(x->parent);
                x = root();
            }
        }
        else {
            y = x->parent->left;
            if (y != nullptr) {
                if (y->color == RBT_color_red) {
                    x->parent->color = RBT_color_red;
                    y->color = RBT_color_black;
                    _right_rotate(x->parent);
                    y = x->parent->left;
                }
                if (y->right == nullptr && y->left != nullptr) {
                    if (y->left->color == RBT_color_black) {
                        y->color = RBT_color_red;
                        x = x->parent;
                    }
                }
                else if (y->right != nullptr && y->left != nullptr) {
                    if (y->right->color == RBT_color_black && y->left->color == RBT_color_black) {
                        y->color = RBT_color_red;
                        x = x->parent;
                    }
                    else if (y->left->color == RBT_color_black) {
                        y->color = RBT_color_red;
                        y->right->color = RBT_color_black;
                        _left_rotate(y);
                        y = x->parent->left;
                    }
                }
                y->color = x->parent->color;
                if (y->left != nullptr) {
                    y->left->color = RBT_color_black;
                }
            }
            if (x != root()) {
                x->parent->color = RBT_color_black;
                _left_rotate(x->parent);
                x = root();
            }
        }
    }
    x->color = RBT_color_black;
}

template <typename T, typename COMP>
void RBT<T, COMP>::_erase (base_ptr ptr) {
    base_ptr x, y = ptr;
    RBT_color_type origin = ptr->color;
    if (ptr->left == nullptr && ptr->right == nullptr) {
        if (ptr == min()) {
            min() = ptr->parent;
        }
        if (ptr == max()) {
            max() = ptr->parent;
        }
        _transplant(ptr, nullptr);
        return;
    }
    else if (ptr->left == nullptr) {
        x = ptr->right;
        _transplant(ptr, ptr->right);
        if (ptr == min()) {
            min() = ptr->right;
        }
    }
    else if (ptr->right == nullptr) {
        x = ptr->left;
        _transplant(ptr, ptr->left);
        if (ptr == max()) {
            max() = ptr->left;
        }
    }
    else {
        y = _minimum(ptr->right);
        x = y->right;
        origin = y->color;
        if (y->parent == ptr) {
            if (x != nullptr) {
                x->parent = y;
            }
        }
        else {
            _transplant(y, y->right);
            y->right = ptr->right;
            y->right->parent = y;
        }
        _transplant(ptr, y);
        y->left = ptr->left;
        y->left->parent = y;
        y->color = ptr->color;
    }
    if (origin == RBT_color_black) {
        _delete_fix(x);
    }
}

}

#endif // !RB_TREE_H