#pragma once

#include "sw_fwd.h"  // Forward declaration

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() noexcept {
        block_ = nullptr;
        observer_ = nullptr;
    }

    WeakPtr(const WeakPtr& other) noexcept {
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_->IncW();
        }
    }

    template <class Y>
    WeakPtr(const WeakPtr<Y>& other) noexcept {
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_->IncW();
        }
    }

    WeakPtr(WeakPtr&& other) {
        block_ = std::move(other.block_);
        observer_ = std::move(other.observer_);
        other.block_ = nullptr;
        other.observer_ = nullptr;
    }

    template <class Y>
    WeakPtr(WeakPtr<Y>&& other) {
        block_ = std::move(other.block_);
        observer_ = std::move(other.observer_);
        other.block_ = nullptr;
        other.observer_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    template <class Y>
    WeakPtr(const SharedPtr<Y>& other) noexcept {
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_->IncW();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        WeakPtr<T>(other).Swap(*this);
        return *this;
    }

    template <class Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) noexcept {
        WeakPtr<T>(other).Swap(*this);
        return *this;
    }

    template <class Y>
    WeakPtr& operator=(const SharedPtr<Y>& other) noexcept {
        WeakPtr<T>(other).Swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        WeakPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    template <class Y>
    WeakPtr& operator=(WeakPtr<Y>&& other) noexcept {
        WeakPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (block_) {
            block_->DecW();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() noexcept {
        WeakPtr().Swap(*this);
    }
    void Swap(WeakPtr& other) noexcept {
        std::swap(block_, other.block_);
        std::swap(observer_, other.observer_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const noexcept {
        if (!block_) {
            return 0;
        }
        return block_->GetCnt();
    }
    bool Expired() const noexcept {
        return UseCount() == 0;
    }
    SharedPtr<T> Lock() const noexcept {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

    BlockBase* block_;
    T* observer_;
};
