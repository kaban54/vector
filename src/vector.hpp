#include <cstring>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
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
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    private:

    template<bool Const>
    struct VecIter {
        using iterator_category = typename std::contiguous_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = typename std::allocator_traits<allocator_type>::pointer;
        using reference = value_type&;
        using ptr_t = typename std::conditional_t<Const, const_pointer, pointer>;
        using ref_t = typename std::conditional_t<Const, const_reference, reference>;

        constexpr VecIter() noexcept: ptr_(nullptr) {}

        constexpr VecIter(pointer ptr) noexcept: ptr_(ptr) {}

        constexpr VecIter(const VecIter& other) noexcept: ptr_(other.ptr_) {}

        constexpr VecIter& operator=(const VecIter& other) noexcept {
            ptr_ = other.ptr_;
            return *this;
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

    constexpr explicit Vector(const allocator_type& alloc) noexcept:
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
            Fill(allocator_, begin(), end());
        }

    template<typename InputIt, typename = std::_RequireInputIter<InputIt>>
    constexpr Vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type()):
        allocator_(alloc),
        sz_(std::distance(first, last)),
        cp_(sz_),
        data_(nullptr) {
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            iterator dst(data_);
            for(;first != last; ++first) {
                std::allocator_traits<allocator_type>::construct(allocator_, (dst++).ptr_, *first);
            }
        }

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

    constexpr Vector(std::initializer_list<T> init, const allocator_type& alloc = allocator_type()):
        allocator_(alloc),
        sz_(init.size()),
        cp_(init.size()),
        data_(nullptr)
        {
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            iterator dst(data_);
            for(const_pointer src = init.begin(); src != init.end(); ++src) {
                std::allocator_traits<allocator_type>::construct(allocator_, (dst++).ptr_, *src);
            }
        }
    
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
    noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value ||
             std::allocator_traits<Allocator>::is_always_equal::value) {
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

    constexpr Vector& operator=(std::initializer_list<T> ilist) {
        if (ilist.size() > cp_) {
            if (data_ != nullptr) {
                Destroy(allocator_, begin(), end());
                std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
            }
            cp_ = ilist.size();
            data_ = std::allocator_traits<allocator_type>::allocate(allocator_, cp_);
            sz_ = 0;
        }
        size_type minsz = sz_ < ilist.size() ? sz_ : ilist.size();
        const_pointer src = ilist.begin();
        iterator dst = begin();
        for (; dst < begin() + (int)minsz; ++dst) {
            *dst = *(src++);
        }
        if (sz_ < ilist.size()) {
            for(; src != ilist.end(); ++src) {
                std::allocator_traits<allocator_type>::construct(allocator_, (dst++).ptr_, *src);
            }
        } 
        else Destroy(allocator_, begin() + (int)ilist.size(), end());

        sz_ = ilist.size();
        return *this;
    }

    constexpr allocator_type get_allocator() const noexcept {
        return allocator_;
    }

    constexpr reference at(size_type pos) {
        if (pos >= sz_) throw std::out_of_range("Vector::at");
        return data_[pos];
    }

    constexpr const_reference at(size_type pos) const {
        if (pos >= sz_) throw std::out_of_range("Vector::at");
        return data_[pos];
    }

    constexpr reference operator[](size_type pos) noexcept {
        return data_[pos];
    }

    constexpr const_reference operator[](size_type pos) const noexcept {
        return data_[pos];
    }

    constexpr reference front() noexcept {
        return data_[0];
    }

    constexpr const_reference front() const noexcept {
        return data_[0];
    }

    constexpr reference back() noexcept {
        return data_[sz_ - 1];
    }

    constexpr const_reference back() const noexcept {
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
        Reserve<value_type>(new_cap);
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
        Destroy(allocator_, begin(), end());
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
        return emplace(position, val);
    }

    constexpr iterator insert(const_iterator position, T&& val) {
        return emplace(position, std::move(val));
    }

    constexpr iterator insert(const_iterator position, size_type count, const_reference val) {
        difference_type dif = position - cbegin();
        if (sz_ + count > cp_) {
            Reserve<value_type>( (sz_ + count > cp_ * 2) ? (sz_ + count) : (cp_ * 2) );
        }
        iterator pos = begin() + dif;

        if (pos != end()) {
            std::allocator_traits<allocator_type>::construct(allocator_, (end() + count - 1).ptr_, std::move(back()));
            for (auto it = end() + count - 2; it != pos + count - 1; --it) {
                *it = std::move(*(it - count));
            }
        }
        Fill(allocator_, pos, pos + count, val);
        sz_ += count;
        return pos;
    }

    template<typename... Args>
    constexpr iterator emplace(const_iterator position, Args&&... args) {difference_type dif = position - cbegin();
        if (sz_ == cp_) {
            Reserve<value_type>(cp_ == 0 ? 2 : cp_ * 2);
        }
        iterator pos = begin() + (int)dif;

        if (pos != end()) {
            MoveBeforeEmplace<value_type>(pos);
        }

        std::allocator_traits<allocator_type>::construct(allocator_, pos.ptr_, forward<Args>(args)...);
        sz_++;
        return pos;
    }

    constexpr iterator erase(const_iterator position) {
        iterator pos = position;
        MoveAssign(pos + 1, end(), pos);
        --sz_;
        std::allocator_traits<allocator_type>::destroy(allocator_, end());
        return pos;
    }

    constexpr void push_back(const_reference val) {
        emplace_back(val);
    }

    constexpr void push_back(T&& val) {
        emplace_back(std::move(val));
    }

    template<typename... Args>
    constexpr reference emplace_back(Args&&... args) {
        return *emplace(cend(), forward<Args>(args)...);
    }

    constexpr void pop_back() noexcept {
        --sz_;
        std::allocator_traits<allocator_type>::destroy(allocator_, end());
    }

    constexpr void resize(size_type count) {
        if (count > max_size()) throw std::length_error("Vector::resize");
        if (count < sz_) {
            Destroy(allocator_, begin() + count, end());
        }
        else {
            Reserve<value_type>(count);
            Fill(allocator_, end(), begin() + count);
        }
        sz_ = count;
    }

    constexpr void resize(size_type count, const_reference val) {
        if (count > max_size()) throw std::length_error("Vector::resize");
        if (count < sz_) {
            Destroy(allocator_, begin() + count, end());
        }
        else {
            Reserve<value_type>(count);
            Fill(allocator_, end(), begin() + count, val);
        }
        sz_ = count;
    }

    constexpr void swap(Vector& other)
    noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
             std::allocator_traits<Allocator>::is_always_equal::value) {
        if (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
           std::swap (allocator_, other.allocator_);
        }
        size_type buf = sz_;
        sz_ = other.sz_;
        other.sz_ = buf;
        buf = cp_;
        cp_ = other.cp_;
        other.cp_ = buf;
        pointer databuf = data_;
        data_ = other.data_;
        other.data_ = databuf;
    }

    private:

    allocator_type allocator_;
    size_type sz_;
    size_type cp_;
    pointer data_;

    static void Copy(allocator_type allocator, iterator src, iterator src_end, iterator dst) {
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

    static void MoveAssign(iterator src, iterator src_end, iterator dst) {
        for(;src != src_end; ++src) {
            *(dst++) = std::move(*src);
        }
    }

    template<typename U, std::enable_if_t<std::is_trivially_copyable_v<U>, bool> = true>
    constexpr void Reserve(size_type new_cap) {
        std::cout << "r1\n";
        if (new_cap <= cp_) return;
        if (new_cap >= max_size()) throw std::length_error("Vector::reserve");

        pointer new_data = std::allocator_traits<allocator_type>::allocate(allocator_, new_cap);

        std::memcpy(new_data, data_, sz_ * sizeof(value_type));

        std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
        data_ = new_data;
        cp_ = new_cap;
    }

    template<typename U, std::enable_if_t<!std::is_trivially_copyable_v<U>, bool> = true>
    constexpr void Reserve(size_type new_cap) {
        std::cout << "r2\n";
        if (new_cap <= cp_) return;
        if (new_cap >= max_size()) throw std::length_error("Vector::reserve");

        pointer new_data = std::allocator_traits<allocator_type>::allocate(allocator_, new_cap);

        if (std::is_nothrow_move_constructible_v<value_type> || 
            std::is_same_v<std::__is_copy_insertable<allocator_type>, std::false_type>) {
                Move(allocator_, begin(), end(), iterator(new_data));
        }
        else {
            iterator current(new_data);
            try {
                for(const_iterator it = cbegin(); it != cend(); ++it) {
                    std::allocator_traits<allocator_type>::construct(allocator_, current.ptr_, *it);
                    ++current;
                }
            }
            catch(...) {
                for(iterator it(new_data); it != current; ++it) {
                    std::allocator_traits<allocator_type>::destroy(allocator_, it.ptr_);
                }
                std::allocator_traits<allocator_type>::deallocate(allocator_, new_data, new_cap);
                throw;
            }
        }

        Destroy(allocator_, begin(), end());
        if (data_ != nullptr) {
            std::allocator_traits<allocator_type>::deallocate(allocator_, data_, cp_);
        }

        data_ = new_data;
        cp_ = new_cap;
    }

    template<typename U, typename... Args, std::enable_if_t<std::is_trivially_copyable_v<U>, bool> = true>
    void MoveBeforeEmplace(iterator pos) {
        std::cout << "empl1\n";
        std::memmove((pos + 1).ptr_, pos.ptr_, (end() - pos) * sizeof(value_type));
    }

    template<typename U, typename... Args, std::enable_if_t<!std::is_trivially_copyable_v<U>, bool> = true>
    void MoveBeforeEmplace(iterator pos) {
        std::cout << "empl2\n";
        std::allocator_traits<allocator_type>::construct(allocator_, end().ptr_, std::move(back()));
        for (iterator it = end() - 1; it != pos; --it) {
            *it = std::move(*(it - 1));
        }
    }
};

// TODO: bool specialization


} // namespace myvector
