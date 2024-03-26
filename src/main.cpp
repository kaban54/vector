#include "vector.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <iterator>
#include<algorithm>
#include <cmath>


using myvector::Vector;


void Modify(int& x) {
    static int m = 100;
    x += (m -= 2);
}

void Print(int x) {
    std::cout << x << '\n';
}


int main() {
    
    Vector<int> vec1(8);

    std::for_each(vec1.begin(), vec1.end(), Modify);
    std::for_each(vec1.begin(), vec1.end(), Print);


    std::sort(vec1.begin(), vec1.end());
    std::cout << "\n\n";

    for (auto elem : vec1) {
        std::cout << elem << "\n";
    }


    Vector<int> vec2(8);
    

    std::copy(vec1.begin(), vec1.end(), vec2.begin());

    std::cout << "\n\n";
    std::for_each(vec2.begin(), vec2.end(), Print);


    double x = 123.666;

    Vector<double> d1(10);
    d1[0] = 0.666;
    d1[1] = 1.666;
    d1[2] = 2.1;
    d1[3] = 3.32;
    d1[4] = 4.666;
    d1[5] = 5.666;
    d1[6] = 6.123;

    Vector<double> d2(10);

    if (d1[0] == 0.002) std::cout << "E\n";

    std::copy_if(d1.begin(), d1.end(), d2.begin(), 
                 [x](double arg) {std::cout << "[ " << arg << "\t" << (int)arg << "\t" << arg - (int)arg << " ]\n";
                                  return fabs((arg - (int)arg) - 0.666) < 0.001;});


    std::cout << "\n\n";
    std::for_each(d2.begin(), d2.end(), [](double y) {std::cout << y << "\n";});

    return 0;
}