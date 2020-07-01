#include<iostream>
#include<string>
#include"Map.h"
#include"Fibonacci_Heap.h"

using namespace std;
typedef TKF::pair<int, string> Pair;
typedef TKF::map<int, string> Map;
typedef TKF::multimap<int, string> multiMap;
int main(){
    Pair Pair_1(1, "cat");
    Pair Pair_2(1, "dog");
    Pair Pair_3(2, "fish");
    Pair Pair_4(4, "bird");
    Pair Pair_5(4, "mouse");

    multiMap m;
    m.insert(Pair_5);
    m.insert(Pair_3);
    m.insert(Pair_2);
    m.insert(Pair_1);
    m.insert(Pair_4);
    m.erase(m.begin(), m.begin() + 2);
    for (auto i = m.begin(); i != m.end(); i++) {
        cout << *i << endl;
    }

    cout << endl;
    Map m2;
    m2.insert(Pair_5);
    m2.insert(Pair_3);
    m2.insert(Pair_2);
    m2.insert(Pair_1);
    m2.insert(Pair_4);
    m2[3] = "snake";
    for (auto i = m2.begin(); i != m2.end(); i++) {
        cout << *i << endl;
    }

    cout << endl;
    TKF::FIBHeap<int, TKF::less<int>> f;
    f.insert(1);
    f.insert(4);
    f.insert(3);
    f.insert(1);
    f.insert(6);
    f.extract();
    f.extract();
    f.extract();
    f.extract();
    f.extract();

    cout << f.empty();
    return 0;
}