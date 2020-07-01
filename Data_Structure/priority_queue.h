//file: priority_queue.h
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include"Fibonacci_Heap.h"

namespace TKF {

template <typename T, typename COMP = TKF::less<T>, 
    typename HEAP = TKF::FIBHeap<T, COMP> >
class priority_queue {
protected:
    HEAP heap;
    COMP comp;

public:
    typedef typename HEAP::value_type      value_type;
    typedef typename HEAP::reference       reference;
    typedef typename HEAP::const_reference const_reference;
    typedef typename HEAP::size_type       size_type;

    typedef COMP                           value_compare;

public:
    priority_queue(): heap(), comp() {}

    explicit
    priority_queue(COMP const& c, HEAP const& h) 
        : heap(h), comp(c) {}

    explicit
    priority_queue(COMP const& c, HEAP&& h = HEAP()) 
        : heap(TKF::move(h)), comp(c) {}
    
    bool empty() const noexcept {
        return heap.empty();
    }

    size_type size() const noexcept {
        return heap.size();
    }

    const_reference top() const {
        return heap.top()->get_node_ptr()->value;
    }

    void push(value_type const& value) {
        heap.insert(value);
    }

    void push(value_type&& value) {
        heap.insert(TKF::move(value));
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        heap.emplace(TKF::forward<Args>(args)...);
    }

    void pop() {
        heap.extract();
    }
};

}

#endif //!PRIORITY_QUEUE_H