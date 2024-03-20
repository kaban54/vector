#include "vector.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <iterator>

int main() {
    std::vector<int> a({1, 2});

    a.insert(a.begin(), 0);
    std::cout << a[0] << a[1] << a[2];

    auto r = std::make_reverse_iterator(a.end());

    std::cout << '\n' << *r;

    return 0;
}