#ifndef MAP_H
#define MAP_H

#include"RB_Tree.h"
#include"Utility.h"

namespace TKF {

template <typename T>
struct less {
    bool operator () (T const& lhs, T const& rhs) const {
        return lhs <= rhs;
    }
};

template <typename KEY, typename T, typename COMP = less<KEY> >
class map {
public:
    typedef KEY                     key_type;
    typedef T                       map_type;
    typedef TKF::pair<KEY, T>       value_type;
    typedef COMP                    key_compare;
    
    class value_compare {
        friend class map<KEY, T, COMP>;
    private:
        COMP _comp;
        value_compare(COMP comp) : _comp(comp) {}
    public:
        bool operator () (value_type const& lhs, value_type const& rhs) {
            return _comp(lhs.first, rhs.first);
        }
    };
    
private:
    typedef TKF::RBT<value_type, key_compare> base_type;
    base_type _tree;

public:
    typedef typename base_type::node_type            node_type;
    typedef typename base_type::node_ptr             node_ptr;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;
    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::iterator             iterator;
    typedef typename base_type::reverse_iterator     reverse_iterator;

public:
    map() : _tree() {}

    template <typename ITER>
    map(ITER first, ITER second) : _tree() {
        _tree.unique_insert(first, second);
    }
    
    map(map const& rhs) : _tree(rhs._tree) {}

    map(map&& rhs) : _tree(TKF::move(rhs._tree)) {}

    map& operator = (map const& rhs) {
        _tree = rhs._tree;
        return *this;
    }

    map& operator = (map&& rhs) {
        _tree = TKF::move(rhs._tree);
        return *this;
        
    }

    ~map() = default;
    
    //API
    key_compare key_comp() const { 
        return _tree.key_comp();
    }

    value_compare value_comp() const {
        return value_compare(_tree.key_comp());
    }

    allocator_type get_allocator() const {
        return _tree.get_allocator();
    }

    //Iterator
    iterator begin() const noexcept {
        return _tree.begin();
    }

    iterator end() const noexcept {
        return _tree.end();
    }

    iterator rbegin() const noexcept {
        return _tree.rbegin();
    }

    iterator rend() const noexcept {
        return _tree.rend();
    }

    bool empty() const noexcept {
        return _tree.empty();
    }

    size_type size() const noexcept {
        return _tree.size();
    }

    size_type max_size() const noexcept {
        return _tree.max_size();
    }


    map_type& at (key_type const& key) {
        iterator iter = _tree.lower_bound(key);
        THROW_OUT_OF_RANGE_IF(iter == end() || !key_comp()(iter->first, key)
            ,"map<KEY, T> has no such element");
        return iter->second;
    }

    map_type& operator [] (key_type& key) {
        iterator iter = _tree.lower_bound(key);
        if (iter == end() || !key_comp()(iter->first, key)) {
            std::cout << "ok";
            iter = emplace_hint(iter, key, T{});
        }
        return iter->second;
    }

    map_type& operator [] (key_type&& key) {
        iterator iter = _tree.lower_bound(key);
        if (iter == end() || !key_comp()(iter->first, key)) {
            iter = emplace_hint(iter, TKF::move(key), T{});
        }
        return iter->second;
    }
    
    template <typename ...Args>
    iterator emplace (Args&&... args) {
        return _tree.unique_emplace(TKF::forward<Args>(args)...);
    }

    template <typename ...Args>
    iterator emplace_hint (iterator hint, Args&&... args) {
        return _tree.unique_emplace_hint(hint, TKF::forward<Args>(args)...);
    }

    TKF::pair<iterator, bool> insert (value_type const& value) {
        return _tree.unique_insert(value);
    }

    TKF::pair<iterator, bool> insert (value_type&& value) {
        return _tree.unique_insert(TKF::move(value));
    }

    iterator insert (iterator hint, value_type const& value) {
        return _tree.unique_insert(hint, value);
    }

    iterator insert (iterator hint, value_type&& value) {
        return _tree.unique_insert(hint, TKF::move(value));
    }

    template <typename ITER>
    void insert (ITER const& first, ITER const& last) {
        _tree.unique_insert(first, last);
    }

    void erase (iterator iter) {
        _tree.erase(iter);
    }

    size_type erase(key_type const& key) {
        return _tree.unique_erase(key);
    }

    void erase (iterator first, iterator last) {
        _tree.erase(first, last);
    }

    void clear () {
        _tree.clear();
    }

    iterator find (key_type const& key) {
        return _tree.find(key);
    }

    iterator lower_bound (key_type const& key) {
        return _tree.lower_bound(key);
    }

    iterator upper_bound (key_type const& key) {
        return _tree.upper_bound(key);
    }

    friend bool operator == (map const& lhs, map const& rhs) {
        return (lhs._tree == rhs._tree);
    }

    friend bool operator != (map const& lhs, map const& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator < (map const& lhs, map const& rhs) {
        return lhs._tree < rhs._tree;
    }

    friend bool operator > (map const& lhs, map const& rhs) {
        return rhs < lhs;
    }

    friend bool operator >= (map const& lhs, map const& rhs) {
        return !(lhs < rhs);
    }

    friend bool operator <= (map const& lhs, map const& rhs) {
        return !(rhs < lhs);
    }
    
};

template <typename KEY, typename T, typename COMP = less<KEY> >
class multimap {
public:
    typedef KEY                     key_type;
    typedef T                       map_type;
    typedef TKF::pair<KEY, T>       value_type;
    typedef COMP                    key_compare;
    
    class value_compare {
        friend class map<KEY, T, COMP>;
    private:
        COMP _comp;
        value_compare(COMP comp) : _comp(comp) {}
    public:
        bool operator () (value_type const& lhs, value_type const& rhs) {
            return _comp(lhs.first, rhs.first);
        }
    };
    
private:
    typedef TKF::RBT<value_type, key_compare> base_type;
    base_type _tree;

public:
    typedef typename base_type::node_type            node_type;
    typedef typename base_type::node_ptr             node_ptr;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;
    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::iterator             iterator;
    typedef typename base_type::reverse_iterator     reverse_iterator;

public:
    multimap() : _tree() {}

    template <typename ITER>
    multimap(ITER first, ITER second) : _tree() {
        _tree.unique_insert(first, second);
    }
    
    multimap(multimap const& rhs) : _tree(rhs._tree) {}

    multimap(multimap&& rhs) : _tree(TKF::move(rhs._tree)) {}

    multimap& operator = (multimap const& rhs) {
        _tree = rhs._tree;
        return *this;
    }

    multimap& operator = (multimap&& rhs) {
        _tree = TKF::move(rhs._tree);
        return *this;
        
    }

    //API
    key_compare key_comp() const { 
        return _tree.key_comp();
    }

    value_compare value_comp() const {
        return value_compare(_tree.key_comp());
    }

    allocator_type get_allocator() const {
        return _tree.get_allocator();
    }

    //Iterator
    iterator begin() const noexcept {
        return _tree.begin();
    }

    iterator end() const noexcept {
        return _tree.end();
    }

    iterator rbegin() const noexcept {
        return _tree.rbegin();
    }

    iterator rend() const noexcept {
        return _tree.rend();
    }

    bool empty() const noexcept {
        return _tree.empty();
    }

    size_type size() const noexcept {
        return _tree.size();
    }

    size_type max_size() const noexcept {
        return _tree.max_size();
    }
    
    iterator insert (value_type const& value) {
        return _tree.multi_insert(value);
    }

    iterator insert (value_type&& value) {
        return _tree.multi_insert(TKF::move(value));
    }

    iterator insert (iterator hint, value_type const& value) {
        return _tree.multi_insert(hint, value);
    }

    iterator insert (iterator hint, value_type&& value) {
        return _tree.multi_insert(hint, TKF::move(value));
    }

    template <typename ITER>
    void insert (ITER const& first, ITER const& last) {
        _tree.multi_insert(first, last);
    }

    void erase (iterator iter) {
        _tree.erase(iter);
    }

    size_type erase(key_type const& key) {
        return _tree.multi_erase(key);
    }

    void erase (iterator first, iterator last) {
        _tree.erase(first, last);
    }

    void clear () {
        _tree.clear();
    }

    iterator find (key_type const& key) {
        return _tree.find(key);
    }

    iterator lower_bound (key_type const& key) {
        return _tree.lower_bound(key);
    }

    iterator upper_bound (key_type const& key) {
        return _tree.upper_bound(key);
    }

    friend bool operator == (multimap const& lhs, multimap const& rhs) {
        return (lhs._tree == rhs._tree);
    }

    friend bool operator != (multimap const& lhs, multimap const& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator < (multimap const& lhs, multimap const& rhs) {
        return lhs._tree < rhs._tree;
    }

    friend bool operator > (multimap const& lhs, multimap const& rhs) {
        return rhs < lhs;
    }

    friend bool operator >= (multimap const& lhs, multimap const& rhs) {
        return !(lhs < rhs);
    }

    friend bool operator <= (multimap const& lhs, multimap const& rhs) {
        return !(rhs < lhs);
    }
    
};


}

#endif //!MAP_H