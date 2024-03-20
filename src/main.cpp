#include "vector.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <iterator>

using myvector::Vector;

int main() {
    std::vector<int> a;

    Vector<int> v (5, 5);

    std::cout << v[0] << v[1] << v[2] << '\n';

    *(v.begin()) = 0;
    v.insert(v.cbegin() + 1, 123);

    std::cout << v[0] << v[1] << v[2] << '\n';
    std::cout << *(v.cbegin());

    return 0;
}