#include "vector.hpp"
#include <memory>
#include <vector>

int main() {
    std::vector<int> a({1, 2, 3});

    a[2];
    a.at(1);
    return 0;
}