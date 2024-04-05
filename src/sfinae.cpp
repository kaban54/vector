#include "vector.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

using myvector::Vector;

struct TriviallyCopyable {
    int a;
    char* c;
};

struct NotTriviallyCopyable {
    int data;
    char* addr;

    NotTriviallyCopyable(): data (0), addr ((char*)this) {}
    NotTriviallyCopyable(const NotTriviallyCopyable& other): data (other.data), addr ((char*)this) {}
    NotTriviallyCopyable(NotTriviallyCopyable&& other): data (other.data), addr ((char*)this) {}

    NotTriviallyCopyable& operator=(const NotTriviallyCopyable& other) {
        data = other.data;
        return *this;
    }

    NotTriviallyCopyable& operator=(NotTriviallyCopyable&& other) {
        data = other.data;
        return *this;
    }
};

template <typename T>
void Test1(const std::string& name) {
    Vector<T> v;
    auto start = std::chrono::steady_clock::now();

    for (int k = 0; k < 10; ++k) {
        for (int i = 0; i < 10000000; ++i) {
            v.emplace_back();
        }
        v.clear();
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << name << ": " << duration.count() << " s\n";
}


int main() {
    Test1<TriviallyCopyable>("TriviallyCopyable");
    Test1<NotTriviallyCopyable>("NotTriviallyCopyable");
    return 0;
}