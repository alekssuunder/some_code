#pragma once

#include <memory>

enum class TypeObject { NUMBER, SYMBOL, CELL };

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual TypeObject GetType() = 0;

    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number() = default;

    Number(int val);

    TypeObject GetType() override {
        return TypeObject::NUMBER;
    }

    int GetValue() const;

    int val_;
};

class Symbol : public Object {
public:
    Symbol() = default;

    Symbol(const std::string& val);

    TypeObject GetType() override {
        return TypeObject::SYMBOL;
    }

    const std::string& GetName() const;

    std::string val_;
};

class Cell : public Object {
public:
    Cell() = default;

    Cell(const std::shared_ptr<Object>& first, const std::shared_ptr<Object>& second);

    TypeObject GetType() override {
        return TypeObject::CELL;
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return static_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return T().GetType() == obj->GetType();
}
