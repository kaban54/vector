#include <type_traits>
#include <iterator>

namespace myconcept {

template<bool B, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> {
    using type = T;
};

template<bool Cond, typename T = void>
using enable_if_t = typename enable_if<Cond, T>::type;



template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

template <typename T>
concept NotTriviallyCopyable = !TriviallyCopyable<T>;

template <typename Iter>
concept InputIterator = (std::is_convertible_v<typename std::iterator_traits<Iter>::iterator_category,
                                               std::input_iterator_tag>);

} // namespace myconcept