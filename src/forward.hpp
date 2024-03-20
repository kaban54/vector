#include <type_traits>
namespace myforward {

template<typename T>
T&& forward(typename std::remove_reference_t<T>& arg) {
    return static_cast<T&&>(arg);
}

template<typename T>
T&& forward(typename std::remove_reference_t<T>&& arg) {
    return static_cast<T&&>(arg);
}

} // namespace myforward