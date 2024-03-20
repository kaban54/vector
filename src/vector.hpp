#include <memory>
#include <iterator>
#include <type_traits>
#include "forward.hpp"

namespace myvector {

using myforward::forward;


template<typename T, typename Allocator = std::allocator<T>>
class Vector {
    public:

    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<allocator_type>::pointer;
    using const_pointer = std::allocator_traits<allocator_type>::const_pointer;

    private:

    template<bool Const>
    struct VecIter : public std::iterator<std::contiguous_iterator_tag, value_type> {
        using ptr_t = typename std::conditional_t<Const, const_pointer, pointer>;
        using ref_t = typename std::conditional_t<Const, const_reference, reference>;

        constexpr VecIter() noexcept: ptr_(nullptr) {}

        constexpr VecIter(pointer ptr) noexcept: ptr_(ptr) {}

        constexpr VecIter(const VecIter& other) noexcept: ptr_(other.ptr_) {}

        constexpr VecIter& operator=(const VecIter& other) noexcept {
            ptr_ = other.ptr_;
        }

        constexpr ref_t operator*() const noexcept {
            return *ptr_;
        }

        constexpr ptr_t operator->() const noexcept {
            return ptr_;
        }

        constexpr ref_t operator[](size_type idx) noexcept {
            return ptr_[idx];
        }

        constexpr VecIter& operator++() noexcept {
            ++ptr_;
            return *this;
        }

        constexpr VecIter operator++(int) noexcept {
            return VecIter(ptr_++);
        }

        constexpr VecIter& operator--() noexcept {
            --ptr_;
            return *this;
        }

        constexpr VecIter operator--(int) noexcept {
            return VecIter(ptr_--);
        }

        constexpr VecIter operator+(int n) const noexcept {
            return VecIter(ptr_ + n);
        }

        constexpr VecIter operator-(int n) const noexcept {
            return VecIter(ptr_ - n);
        }

        constexpr VecIter& operator+=(int n) noexcept {
            ptr_ += n;
            return *this;
        }

        constexpr VecIter& operator-=(int n) noexcept {
            ptr_ -= n;
            return *this;
        }

        constexpr difference_type operator-(const VecIter& other) const noexcept {
            return ptr_ - other.ptr_;
        }

        constexpr bool operator==(const VecIter& other) const noexcept {
            return ptr_ == other.ptr_;
        }

        constexpr bool operator!=(const VecIter& other) const noexcept {
            return ptr_ != other.ptr_;
        }
        
        constexpr bool operator<=(const VecIter& other) const noexcept {
            return ptr_ <= other.ptr_;
        }
        
        constexpr bool operator>=(const VecIter& other) const noexcept {
            return ptr_ >= other.ptr_;
        }

        constexpr bool operator<(const VecIter& other) const noexcept {
            return ptr_ < other.ptr_;
        }
        
        constexpr bool operator>(const VecIter& other) const noexcept {
            return ptr_ > other.ptr_;
        }

        pointer ptr_;
    };

    public:

    using iterator = VecIter<false>;
    using const_iterator = VecIter<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr Vector() noexcept(noexcept(allocator_type())):
        allocator_(),
        sz_(0),
        cp_(0),
        data_(nullptr) {}

    constexpr explicit Vector(const allocator_type& alloc) noexcept :
        allocator_(alloc),
        sz_(0),
        cp_(0),
        data_(nullptr) {}

    constexpr Vector(size_type count, const T& value, const allocator_type& alloc = allocator_type()):
        allocator_(alloc),
        sz_(count),
        cp_(count),
        data_(nullptr)
        {
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            Fill(allocator_, begin(), begin() + sz_, value);
        }

    constexpr explicit Vector(size_type count, const allocator_type& alloc = allocator_type()):
        allocator_(alloc),
        sz_(count),
        cp_(count),
        data_(nullptr)
        {
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            Fill(allocator_, begin(), begin() + sz_);
        }

    // template<class InputIt>
    // constexpr Vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type());

    constexpr Vector(const Vector& other):
        allocator_(std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.allocator_)),
        sz_(other.sz_),
        cp_(other.sz_),
        data_(nullptr)
        {
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            Copy(allocator_, other.cbegin(), other.cend(), begin());
        }

    constexpr Vector(const Vector& other, const allocator_type& alloc):
        allocator_(alloc),
        sz_(other.sz_),
        cp_(other.sz_),
        data_(nullptr)
        {
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            Copy(allocator_, other.cbegin(), other.cend(), begin());
        }

    constexpr Vector(Vector&& other) noexcept:
        allocator_(std::move(other.allocator_)),
        sz_(other.sz_),
        cp_(other.cp_),
        data_(other.data_)
        {
            other.cp_ = 0;
            other.sz_ = 0;
            other.data_ = nullptr;
        }

    constexpr Vector(Vector&& other, const allocator_type& alloc):
        allocator_(alloc),
        sz_(other.sz_),
        cp_(other.cp_),
        data_(nullptr)
        {
            if (allocator_ == other.allocator_) {
                data_ = other.data_;
                other.cp_ = 0;
                other.sz_ = 0;
                other.data_ = nullptr;
            }
            else {
                data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
                Move(allocator_, other.begin(), other.end(), begin());
            }
        }

    // constexpr Vector( std::initializer_list<T> init, const allocator_type& alloc = allocator_type());
    
    ~Vector() {
        clear();
        if (data_ != nullptr) {
            std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
        }
    }


    constexpr Vector& operator=(const Vector& other) {
        allocator_type old_allocator = allocator_;
        if (std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value) {
            allocator_ = other.allocator_;
        }
        if (allocator_ != old_allocator || other.sz_ > cp_) {
            Destroy(old_allocator, begin(), end());
            if (data_ != nullptr) {
                std::allocator_traits<allocator_type>::deallocate(old_allocator, data_, cp_);
            }
            cp_ = other.sz_;
            sz_ = other.sz_;
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            Copy(allocator_, other.cbegin(), other.cend(), begin());
        }
        else {
            size_type minsz = sz_ < other.sz_ ? sz_ : other.sz_;
            for (size_type idx = 0; idx < minsz; ++idx) {
                data_[idx] = other.data_[idx];
            }
            if (sz_ < other.sz_) Copy(allocator_, other.cbegin() + sz_, other.cend(), end());
            else Destroy(allocator_, begin() + other.sz_, end());
        
            sz_ = other.sz_;
        }

        return *this;
    }

    constexpr Vector& operator=(Vector&& other)
    noexcept(std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value /* TODO */ ) {
        allocator_type old_allocator = allocator_;
        if (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value) {
            allocator_ = other.allocator_;
        }
        if (allocator_ == other.allocator_ || cp_ < other.sz_) {
            Destroy(old_allocator, begin(), end());
            if (data_ != nullptr) {
                std::allocator_traits<allocator_type>::deallocate(old_allocator, data_, cp_);
            }

            if (allocator_ == other.allocator_) {
                sz_ = other.sz_;
                cp_ = other.cp_;
                data_ = other.data_;
                other.sz_ = 0;
                other.cp_ = 0;
                other.data_ = nullptr;
            }
            else {
                data_ = std::allocator_traits<allocator_type>::allocate(allocator_, other.sz_);
                Move(allocator_, other.begin(), other.end(), begin());
            }
        }
        else {
            size_type minsz = sz_ < other.sz_ ? sz_ : other.sz_;
            for (size_type idx = 0; idx < minsz; ++idx) {
                data_[idx] = std::move(other.data_[idx]);
            }
            if (sz_ < other.sz_) Move(allocator_, other.begin() + sz_, other.end(), begin() + sz_);
            else Destroy(allocator_, begin() + other.sz_, end());
        
            sz_ = other.sz_;
        }

        return *this;
    }


    constexpr allocator_type get_allocator() const noexcept {
        return allocator_;
    }

    constexpr reference at(size_type pos) {
        if (pos >= sz_) throw std::out_of_range();
        return data_[pos];
    }

    constexpr const_reference at(size_type pos) const {
        if (pos >= sz_) throw std::out_of_range();
        return data_[pos];
    }

    constexpr reference operator[](size_type pos) noexcept {
        return data_[pos];
    }

    constexpr const_reference operator[](size_type pos) const noexcept {
        return data_[pos];
    }

    constexpr reference front(size_type pos) noexcept {
        return data_[0];
    }

    constexpr const_reference front(size_type pos) const noexcept {
        return data_[0];
    }

    constexpr reference back(size_type pos) noexcept {
        return data_[sz_ - 1];
    }

    constexpr const_reference back(size_type pos) const noexcept {
        return data_[sz_ - 1];
    }

    constexpr pointer data() noexcept {
        return data_;
    }
    
    constexpr const_pointer data() const noexcept {
        return data_;
    }


    constexpr bool empty() const noexcept {
        return sz_ == 0;
    }

    constexpr size_type size() const noexcept {
        return sz_;
    }

    constexpr size_type max_size() const noexcept {
        return std::allocator_traits<allocator_type>::max_size(allocator_);
    }

    constexpr void reserve(size_type new_cap) {
        if (new_cap <= cp_) return;

        pointer new_data = std::allocator_traits<allocator_type>::allocate(allocator_, new_cap);
        Move(allocator_, begin(), end(), iterator(new_data));
        clear();
        if (data_ != nullptr) {
            std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
        }

        data_ = new_data;
        cp_ = new_cap;
    }

    constexpr size_type capacity() const noexcept {
        return cp_;
    }

    constexpr void shrink_to_fit() {
        if (sz_ == cp_) return;

        if (sz_ == 0) {
            std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
            cp_ = 0;
            data_ = nullptr;
            return;
        }

        pointer new_data = std::allocator_traits<allocator_type>::allocate(allocator_, sz_);
        Move(allocator_, begin(), end(), iterator(new_data));
        clear();
        if (data_ != nullptr) {
            std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
        }

        data_ = new_data;
        cp_ = sz_;
    }



    constexpr iterator begin() noexcept {
        return iterator(data_);
    }

    constexpr const_iterator begin() const noexcept {
        return const_iterator(data_);
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(data_);
    }

    constexpr iterator end() noexcept {
        return iterator(data_ + sz_);
    }

    constexpr const_iterator end() const noexcept {
        return const_iterator(data_ + sz_);
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(data_ + sz_);
    }

    constexpr reverse_iterator rbegin() noexcept {
        return std::make_reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    constexpr reverse_iterator rend() noexcept {
        return std::make_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }


    constexpr void clear() noexcept {
        Destroy(allocator_, begin(), end());
        sz_ = 0;
    }

    constexpr iterator insert(const_iterator position, const_reference val) {
        difference_type dif = position - cbegin();
        if (sz_ == cp_) {
            reserve(cp_ == 0 ? 2 : cp_ * 2);
        }
        iterator pos = begin() + dif;

        if (pos != end()) {
            std::allocator_traits<allocator_type>::construct(allocator_, end().ptr_, std::move(back()));
            for (auto it = end() - 1; it != pos, --it) {
                *it = std::move(*(it - 1));
            }
        }
        std::allocator_traits<allocator_type>::construct(allocator_, pos().ptr_, val);
        sz_++;
        return pos;
    }

    constexpr iterator insert(const_iterator position, T&& val) {
        difference_type dif = position - cbegin();
        if (sz_ == cp_) {
            reserve(cp_ == 0 ? 2 : cp_ * 2);
        }
        iterator pos = begin() + dif;

        if (pos != end()) {
            std::allocator_traits<allocator_type>::construct(allocator_, end().ptr_, std::move(back()));
            for (auto it = end() - 1; it != pos, --it) {
                *it = std::move(*(it - 1));
            }
        }
        std::allocator_traits<allocator_type>::construct(allocator_, pos().ptr_, std::move(val));
        sz_++;
        return pos;
    }

    constexpr iterator insert(const_iterator position, size_type count, const_reference val) {
        difference_type dif = position - cbegin();
        if (sz_ + count > cp_) {
            size_type new_cap = (cp_ == 0 ? 2 : cp_ * 2);
            if (sz_ + count > new_cap) new_cap = sz_ + count;
            reserve(new_cap);
        }
        iterator pos = begin() + dif;

        if (pos != end()) {
            std::allocator_traits<allocator_type>::construct(allocator_, (end() + count - 1).ptr_, std::move(back()));
            for (auto it = end() + count - 2; it != pos + count - 1, --it) {
                *it = std::move(*(it - count));
            }
        }
        std::allocator_traits<allocator_type>::construct(allocator_, pos().ptr_, val);
        sz_ += count;
        return pos;
    }


    private:

    allocator_type allocator_;
    size_type sz_;
    size_type cp_;
    pointer data_;

    static void Copy(allocator_type allocator, const_iterator src,
                     const_iterator src_end, iterator dst) {
        for(;src != src_end; ++src) {
            std::allocator_traits<allocator_type>::construct(allocator, (dst++).ptr_, *src);
        }
    }

    static void Move(allocator_type allocator, iterator src, iterator src_end, iterator dst) {
        for(;src != src_end; ++src) {
            std::allocator_traits<allocator_type>::construct(allocator, (dst++).ptr_, std::move(*src));
        }
    }

    static void Destroy(allocator_type allocator, iterator it, iterator end_it) noexcept {
        for(;it != end_it; ++it) {
            std::allocator_traits<allocator_type>::destroy(allocator, it.ptr_);
        }
    }

    template<typename... Args>
    static void Fill(allocator_type allocator, iterator it, iterator end_it, Args&&... args) {
        for(;it != end_it; ++it) {
            std::allocator_traits<allocator_type>::construct(allocator, it.ptr_, forward<Args>(args)...);
        }
    }
};

// TODO: bool specialization


} // namespace myvector
