#include <memory>
#include <iterator>

namespace myvector {


template<typename T, typename Allocator = std::allocator<T>>
class Vector {
    public:

    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<Allocator>::pointer;
    using const_pointer = std::allocator_traits<Allocator>::const_pointer;
    using iterator = std::iterator<std::contiguous_iterator_tag, value_type, difference_type, pointer, reference>;
    using const_iterator = std::basic_const_iterator<iterator>;

    constexpr Vector() noexcept(noexcept(Allocator())):
        allocator_(),
        sz_(0),
        cp_(0),
        data_(nullptr) {}

    constexpr explicit Vector(const Allocator& alloc) noexcept :
        allocator_(alloc),
        sz_(0),
        cp_(0),
        data_(nullptr) {}

    constexpr Vector(size_type count, const T& value, const Allocator& alloc = Allocator()):
        allocator_(alloc),
        sz_(count),
        cp_(count),
        data_(nullptr)
        {
            data_ = std::allocator_traits<Allocator>::allocate(allocator_, cp_);
            for (size_type idx = 0; idx < sz_; ++idx) {
                std::allocator_traits<Allocator>::construct(allocator_, data_ + idx, value);
            }
        }

    constexpr explicit Vector(size_type count, const Allocator& alloc = Allocator()):
        allocator_(alloc),
        sz_(count),
        cp_(count),
        data_(nullptr)
        {
            data_ = std::allocator_traits<Allocator>::allocate(allocator_, cp_);
            for (size_type idx = 0; idx < sz_; ++idx) {
                std::allocator_traits<Allocator>::construct(allocator_, data_ + idx);
            }
        }

    // template< class InputIt >
    // constexpr vector( InputIt first, InputIt last,
    //               const Allocator& alloc = Allocator() );

    constexpr Vector(const vector& other):
        allocator_(alloc),
        sz_(count),
        cp_(count),
        data_(nullptr)
        {

        }

    constexpr Vector( const vector& other, const Allocator& alloc );

    constexpr Vector( vector&& other ) noexcept;

    constexpr Vector( vector&& other, const Allocator& alloc );

    constexpr vector( std::initializer_list<T> init, const Allocator& alloc = Allocator());
    
    ~Vector() {
        for (size_type idx = 0; idx < sz_; ++idx) {
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + idx);
        }
        if (data_ != nullptr) {
            std::allocator_traits<Allocator>::deallocate(allocator_, data, cp_);
        }
    }

    private:

    allocator_type allocator_;
    size_type sz_;
    size_type cp_;
    pointer data_;
};

// TODO: bool specialization


} //namespace myvector
