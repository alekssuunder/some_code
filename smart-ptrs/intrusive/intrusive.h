#pragma once

#include <cstddef>  // for std::nullptr_t
#include <utility>  // for std::exchange / std::swap

class SimpleCounter {
public:
    size_t IncRef() {
        ++count_;
        return count_;
    }
    size_t DecRef() {
        --count_;
        return count_;
    }
    size_t RefCount() const {
        return count_;
    }
    template <class AnotherCounter>
    SimpleCounter& operator=(AnotherCounter& other) {
        return *this;
    }

private:
    size_t count_ = 0;
};

struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    // Increase reference counter.
    void IncRef() {
        counter_.IncRef();
    }

    // Decrease reference counter.
    // Destroy object using Deleter when the last instance dies.
    void DecRef() {
        if (!counter_.DecRef()) {
            Deleter::Destroy(static_cast<Derived*>(this));
        }
    }

    // Get current counter value (the number of strong references).
    size_t RefCount() const {
        return counter_.RefCount();
    }
    RefCounted& operator=(const RefCounted<Derived, Counter, Deleter>& other) {
        return *this;
    }

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
    template <typename Y>
    friend class IntrusivePtr;

public:
    // Constructors
    IntrusivePtr() {
        link_ = nullptr;
    }
    IntrusivePtr(std::nullptr_t) : IntrusivePtr() {
    }
    IntrusivePtr(T* ptr) {
        link_ = ptr;
        if (link_) {
            link_->IncRef();
        }
    }

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other) {
        link_ = other.link_;
        if (link_) {
            link_->IncRef();
        }
    }

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other) {
        link_ = std::move(other.link_);
        other.link_ = nullptr;
    }

    IntrusivePtr(const IntrusivePtr& other) {
        link_ = other.link_;
        if (link_) {
            link_->IncRef();
        }
    }
    IntrusivePtr(IntrusivePtr&& other) {
        link_ = std::move(other.link_);
        other.link_ = nullptr;
    }

    // `operator=`-s
    IntrusivePtr& operator=(const IntrusivePtr& other) {
        IntrusivePtr<T>(other).Swap(*this);
        return *this;
    }
    IntrusivePtr& operator=(IntrusivePtr&& other) {
        IntrusivePtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    // Destructor
    ~IntrusivePtr() {
        if (link_) {
            link_->DecRef();
        }
    }

    // Modifiers
    void Reset() {
        IntrusivePtr().Swap(*this);
    }
    void Reset(T* ptr) {
        IntrusivePtr(ptr).Swap(*this);
    }
    void Swap(IntrusivePtr& other) {
        std::swap(link_, other.link_);
    }

    // Observers
    T* Get() const {
        return link_;
    }
    T& operator*() const {
        return *link_;
    }
    T* operator->() const {
        return link_;
    }
    size_t UseCount() const {
        if (!link_) {
            return 0;
        }
        return link_->RefCount();
    }
    explicit operator bool() const {
        return link_ != nullptr;
    }

private:
    T* link_;
};

template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    return IntrusivePtr(new T(args)...);
}
