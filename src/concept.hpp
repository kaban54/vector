#include <type_traits>
#include <iterator>

namespace myconcept {

template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

template <typename T>
concept NotTriviallyCopyable = !TriviallyCopyable<T>;

template <typename Iter>
concept InputIterator = (std::is_convertible_v<typename std::iterator_traits<Iter>::iterator_category,
                                               std::input_iterator_tag>);

} // namespace myconcept