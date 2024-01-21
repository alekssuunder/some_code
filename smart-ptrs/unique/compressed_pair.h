#pragma once

#include <type_traits>
#include <memory>

template <typename T, std::size_t N, bool = !std::is_final_v<T> && std::is_class_v<T>>
class CompressedElement {
public:
    CompressedElement() = default;
    CompressedElement(T& data) : data_(data) {
    }
    CompressedElement(T&& data) : data_(std::forward<T>(data)) {
    }
    const T& GetElement() const {
        return data_;
    }
    T& GetElement() {
        return data_;
    }
    T data_;
};

template <typename T, std::size_t N>
class CompressedElement<T, N, true> : T {
public:
    CompressedElement() = default;
    CompressedElement(T& data) : T(data) {
    }
    CompressedElement(T&& data) : T(std::forward<T>(data)) {
    }
    const T& GetElement() const {
        return *this;
    }
    T& GetElement() {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : CompressedElement<F, 0>, CompressedElement<S, 1> {
public:
    CompressedPair() : CompressedElement<F, 0>(), CompressedElement<S, 1>() {
    }

    CompressedPair(F&& first, S&& second)
        : CompressedElement<F, 0>(std::forward<F>(first)),
          CompressedElement<S, 1>(std::forward<S>(second)) {
    }
    CompressedPair(F& first, S&& second)
        : CompressedElement<F, 0>(first), CompressedElement<S, 1>(std::forward<S>(second)) {
    }
    CompressedPair(F&& first, S& second)
        : CompressedElement<F, 0>(std::forward<F>(first)), CompressedElement<S, 1>(second) {
    }
    CompressedPair(F& first, S& second)
        : CompressedElement<F, 0>(first), CompressedElement<S, 1>(second) {
    }

    F& GetFirst() {
        return CompressedElement<F, 0>::GetElement();
    }

    const F& GetFirst() const {
        return CompressedElement<F, 0>::GetElement();
    }

    S& GetSecond() {
        return CompressedElement<S, 1>::GetElement();
    }

    const S& GetSecond() const {
        return CompressedElement<S, 1>::GetElement();
    }
};
