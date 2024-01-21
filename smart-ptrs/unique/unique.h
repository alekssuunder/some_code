#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t

template <class T>
struct Slug {
    Slug() = default;

    template <class A>
    Slug(Slug<A>) {
    }

    void operator()(T* ptr) {
        delete ptr;
    }
};

template <class T>
struct Slug<T[]> {
    Slug() = default;

    template <class A>
    Slug(Slug<A>) {
    }

    void operator()(T* ptr) {
        delete[] ptr;
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) noexcept {
        data_.GetFirst() = ptr;
        data_.GetSecond() = Deleter();
    }

    template <class A, class B>
    UniquePtr(A* ptr, B deleter) noexcept {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::move(deleter);
    }

    template <class A, class B>
    UniquePtr(UniquePtr<A, B>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        if (Get()) {
            Reset(nullptr);
        }
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    template <class A, class B>
    UniquePtr& operator=(UniquePtr<A, B>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        if (Get()) {
            Reset(nullptr);
        }
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (Get()) {
            Reset(nullptr);
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        GetDeleter()(data_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() noexcept {
        T* ret = Get();
        data_.GetFirst() = nullptr;
        return ret;
    }
    void Reset(T* ptr = nullptr) noexcept {
        T* old = data_.GetFirst();
        data_.GetFirst() = ptr;
        GetDeleter()(old);
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(GetDeleter(), other.GetDeleter());
        std::swap(data_.GetFirst(), other.data_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() noexcept {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const noexcept {
        return data_.GetSecond();
    }
    explicit operator bool() const noexcept {
        return data_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const noexcept {
        return *data_.GetFirst();
    }
    T* operator->() const noexcept {
        return data_.GetFirst();
    }

    CompressedPair<T*, Deleter> data_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) noexcept {
        data_.GetFirst() = ptr;
        data_.GetSecond() = Deleter();
    }

    template <class A, class B>
    UniquePtr(A* ptr, B deleter) noexcept {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::move(deleter);
    }

    template <class A, class B>
    UniquePtr(UniquePtr<A, B>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        if (Get()) {
            Reset(nullptr);
        }
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    template <class A, class B>
    UniquePtr& operator=(UniquePtr<A, B>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        if (Get()) {
            Reset(nullptr);
        }
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (Get()) {
            Reset(nullptr);
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        GetDeleter()(data_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() noexcept {
        T* ret = Get();
        data_.GetFirst() = nullptr;
        return ret;
    }
    void Reset(T* ptr = nullptr) noexcept {
        T* old = data_.GetFirst();
        data_.GetFirst() = ptr;
        GetDeleter()(old);
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(GetDeleter(), other.GetDeleter());
        std::swap(data_.GetFirst(), other.data_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() noexcept {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const noexcept {
        return data_.GetSecond();
    }
    explicit operator bool() const noexcept {
        return data_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const noexcept {
        return *data_.GetFirst();
    }
    T* operator->() const noexcept {
        return data_.GetFirst();
    }
    T& operator[](size_t i) {
        return data_.GetFirst()[i];
    }

    CompressedPair<T*, Deleter> data_;
};
