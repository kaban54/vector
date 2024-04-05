#include <type_traits>
#include <iterator>

namespace myconcept {

template <bool B, typename T = void>
struct enable_if {};
 
template <typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool Cond, typename T = void>
using enable_if_t = typename enable_if<Cond, T>::type;


template <typename T, T val>
struct integral_constant {
    static constexpr T value = val;
    using value_type = T;
    using type = integral_constant<T, val>;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;


template <typename T, typename U>
struct is_same : std::false_type {};
 
template <typename T>
struct is_same<T, T> : std::true_type {};

template <typename T, typename U>
using is_same_v = is_same<T, U>::value;


template <typename T>
struct is_arithmetic : integral_constant<bool, std::is_integral<T>::value ||
                                               std::is_floating_point<T>::value> {};
template <typename T>
using is_arithmetic_v = is_arithmetic::value;


// template <typename T>
// struct is_enum : integral_constant<bool, > {};
// template <typename T>
// using is_enum_v = is_enum::value;



template<typename T>
struct is_scalar : integral_constant<bool, is_arithmetic<T>::value || 
                                           std::is_enum<T>::value || 
                                           std::is_pointer<T>::value ||
                                           std::is_member_pointer<T>::value || 
                                           std::is_null_pointer<T>::value> {};
template <typename T>
using is_scalar_v = is_scalar::value;


template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

template <typename T>
concept NotTriviallyCopyable = !TriviallyCopyable<T>;

template <typename Iter>
concept InputIterator = (std::is_convertible_v<typename std::iterator_traits<Iter>::iterator_category,
                                               std::input_iterator_tag>);

} // namespace myconcept