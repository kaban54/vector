#include "vector.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <random>


using myvector::Vector;


// void TestMemberfuncs() {
//     Vector<int> vec1;    
// }

void Fill(Vector<int>& vec, size_t num) {
    for (; num > 0; --num) {
        vec.push_back(std::rand() % 1000);
    }
}

void Modify(int& x) {
    static int m = 100;
    x += (m -= 2);
}

void Print(int x) {
    std::cout << x << '\t';
}

void TestForEach() {
    Vector<int> vec1(8);
    std::for_each(vec1.begin(), vec1.end(), Modify);
    std::for_each(vec1.begin(), vec1.end(), Print);
    std::cout << '\n';
}

void TestSort() {
    Vector<int> vec;
    Fill(vec, 6);
    std::cout << "\nBefore sorting:\n";
    std::for_each(vec.begin(), vec.end(), Print);
    std::sort(vec.begin(), vec.end());
    std::cout << "\nAfter sorting:\n";
    std::for_each(vec.begin(), vec.end(), Print);
    std::cout << '\n';
}

void TestForAuto() {
    Vector<int> vec;
    Fill(vec, 6);
    for (auto elem : vec) {
        std::cout << elem << "\t";
    }
    std::cout << '\n';
}

void TestCopy() {
    Vector<int> vec1;
    Fill(vec1, 6);
    Vector<int> vec2(6);
    std::cout << "Vec1:\n";
    std::for_each(vec1.begin(), vec1.end(), Print);
    std::cout << "\n";
    std::copy(vec1.begin(), vec1.end(), vec2.begin());
    std::cout << "Copy:\n";
    std::for_each(vec2.begin(), vec2.end(), Print);
    std::cout << "\n";
}

void TestCopyIf() {
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

    std::copy_if(d1.begin(), d1.end(), d2.begin(), 
                 [x](double arg) {return fabs((arg - (int)arg) - 0.666) < 0.001;});


    std::cout << "\n";
    std::for_each(d2.begin(), d2.end(), [](double y) {std::cout << y << "\t";});
    std::cout << "\n";
}

void TestInitList() {
    Vector<double> d({2.3, 2112, 1.32, -0.22});

    std::cout << "Initializer list test:\n";
    std::for_each(d.begin(), d.end(), [](double y) {std::cout << y << "\t";});
    std::cout << "\n";
}

int main() {
    TestForEach();
    TestSort();
    TestCopy();
    TestCopyIf();
    TestForAuto();
    TestInitList();

    return 0;
}