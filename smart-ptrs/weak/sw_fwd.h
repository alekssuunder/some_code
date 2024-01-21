#pragma once

#include <exception>
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
    BlockObject(Args&&... args) {
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
        } else if (!cntS_) {
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
        } else if (!cntS_) {
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

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
