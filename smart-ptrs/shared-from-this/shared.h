#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() noexcept {
        block_ = nullptr;
        observer_ = nullptr;
    }

    SharedPtr(std::nullptr_t) noexcept : SharedPtr() {
    }

    template <class Y>
    explicit SharedPtr(Y* ptr) noexcept {
        block_ = new BlockPtr<Y>(ptr);
        observer_ = ptr;
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            InitWeakThis(observer_);
        }
    }

    SharedPtr(const SharedPtr& other) noexcept {
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_->IncS();
        }
    }

    template <class Y>
    SharedPtr(const SharedPtr<Y>& other) noexcept {
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_->IncS();
        }
    }

    SharedPtr(SharedPtr&& other) noexcept {
        block_ = std::move(other.block_);
        observer_ = std::move(other.observer_);
        other.block_ = nullptr;
        other.observer_ = nullptr;
    }

    template <class Y>
    SharedPtr(SharedPtr<Y>&& other) noexcept {
        block_ = std::move(other.block_);
        observer_ = std::move(other.observer_);
        other.block_ = nullptr;
        other.observer_ = nullptr;
    }

    template <typename... Args>
    SharedPtr(int i, Args&&... args) {
        // i need 1st arg to avoid using this constructor when i need constructor (10) from
        // cpprefernece
        auto link = new BlockObject<T>(std::forward<Args>(args)...);
        block_ = link;
        observer_ = link->GetPtr();
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            InitWeakThis(observer_);
        }
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        block_ = other.block_;
        observer_ = ptr;
        block_->IncS();
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (!other.UseCount()) {
            throw BadWeakPtr();
        }
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_->IncS();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr<T>(other).Swap(*this);
        return *this;
    }

    template <class Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) noexcept {
        SharedPtr<T>(other).Swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        SharedPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    template <class Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) {
        SharedPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block_) {
            block_->DecS();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() noexcept {
        SharedPtr().Swap(*this);
    }

    template <class Y>
    void Reset(Y* ptr) {
        SharedPtr<T>(ptr).Swap(*this);
    }
    void Swap(SharedPtr& other) {
        std::swap(block_, other.block_);
        std::swap(observer_, other.observer_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return observer_;
    }
    T& operator*() const {
        return *observer_;
    }
    T* operator->() const {
        return observer_;
    }
    size_t UseCount() const {
        if (!block_) {
            return 0;
        }
        return block_->GetCnt();
    }
    explicit operator bool() const {
        return block_ != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // For EnableSharedFromThis
    template <typename Y>
    void InitWeakThis(EnableSharedFromThis<Y>* e) {
        e->weak_this_ = *this;
    }

    BlockBase* block_;
    T* observer_;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) noexcept {
    return left.Get() == right.Get();
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(1, std::forward<Args>(args)...);
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis : public ESFTBase {
public:
    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(weak_this_);
    }
    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(weak_this_);
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return weak_this_;
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return weak_this_;
    }

    WeakPtr<T> weak_this_ = WeakPtr<T>();
};
