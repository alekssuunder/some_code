#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <memory>

struct BlockBase {
    virtual void IncS() {
    }
    virtual void DecS() {
    }
    virtual void IncW() {
    }
    virtual void DecW() {
    }
    virtual size_t GetCnt() = 0;
    virtual ~BlockBase() {
    }
};

template <class T>
struct BlockObject : BlockBase {
    template <typename... Args>
    BlockObject(Args&& ...args) {
        cntS_ = 1;
        cntW_ = 0;
        new (&storage_) T(std::forward<Args>(args)...);
    }
    ~BlockObject() {
    }
    void IncS() override {
        ++cntS_;
    }
    void IncW() override {
        ++cntW_;
    }
    void DecS() override {
        --cntS_;
        if (!cntS_ && !cntW_) {
            std::destroy_at(std::launder(reinterpret_cast<T*>(&storage_)));
            delete this;
        }
        else if (!cntS_) {
            std::destroy_at(std::launder(reinterpret_cast<T*>(&storage_)));
        }
    }
    void DecW() override {
        --cntW_;
        if (!cntW_ && !cntS_) {
            delete this;
        }
    }
    size_t GetCnt() override {
        return cntS_;
    }
    T* GetPtr() {
        return reinterpret_cast<T*>(&storage_);
    }
    size_t cntS_;
    size_t cntW_;
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

template <class T>
struct BlockPtr : BlockBase {
    template <class Y>
    BlockPtr(Y* ptr) noexcept {
        cntS_ = 1;
        cntW_ = 0;
        obj_ = ptr;
    }
    ~BlockPtr() {
    }
    void IncS() override {
        ++cntS_;
    }
    void IncW() override {
        ++cntW_;
    }
    void DecS() override {
        --cntS_;
        if (!cntS_ && !cntW_) {
            delete obj_;
            delete this;
        }
        else if (!cntS_) {
            delete obj_;
        }
    }
    void DecW() override {
        --cntW_;
        if (!cntW_ && !cntS_) {
            delete this;
        }
    }
    size_t GetCnt() override {
        return cntS_;
    }
    size_t cntS_;
    size_t cntW_;
    T* obj_;
};

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
    SharedPtr(int i, Args&& ...args) {
        //i need 1st arg to avoid using this constructor when i need constructor (10) from cpprefernece
        auto link = new BlockObject<T>(std::forward<Args>(args)...);
        block_ = link;
        observer_ = link->GetPtr();
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
    /*template <class Y>
    explicit SharedPtr(const WeakPtr<Y>& other); {
        block_ = other.block_;
        observer_ = other.observer_;
        if (block_) {
            block_.IncS();
        }
    }*/

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
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
