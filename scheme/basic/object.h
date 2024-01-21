#pragma once

#include <memory>
#include <string>

enum class TypeObject { NUMBER, SYMBOL, CELL };

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Execute() = 0;

    virtual std::string ToString() = 0;

    virtual TypeObject GetType() = 0;

    virtual std::shared_ptr<Object> Clone() = 0;

    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number() = default;

    Number(int val);

    std::shared_ptr<Object> Execute() override;

    std::string ToString() override;

    TypeObject GetType() override;

    std::shared_ptr<Object> Clone() override;

    int GetValue() const;

    int val_;
};

class Symbol : public Object {
public:
    Symbol() = default;

    Symbol(const std::string& val);

    std::shared_ptr<Object> Execute() override;

    std::string ToString() override;

    TypeObject GetType() override;

    std::shared_ptr<Object> Clone() override;

    const std::string& GetName() const;

    std::string val_;
};

class Cell : public Object {
public:
    Cell() = default;

    Cell(const std::shared_ptr<Object>& first, const std::shared_ptr<Object>& second);

    std::shared_ptr<Object> Execute() override;

    std::string ToString() override;

    TypeObject GetType() override;

    std::shared_ptr<Object> Clone() override;

    std::shared_ptr<Object> GetFirst() const;

    std::shared_ptr<Object> GetSecond() const;

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return static_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (obj == nullptr) {
        return false;
    }
    return T().GetType() == obj->GetType();
}
