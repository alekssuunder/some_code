#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class TypeObject { NUMBER, SYMBOL, CELL, LAMBDA};

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Execute() = 0;

    virtual std::string ToString() = 0;

    virtual TypeObject GetType() = 0;

    virtual std::shared_ptr<Object> Clone() = 0;

    virtual ~Object() = default;
};

class Scope {
public:
    Scope() : prev_(nullptr) {}
    std::shared_ptr<Scope> prev_;
    std::unordered_map<std::string, std::shared_ptr<Object>> vars_;
};

inline std::unordered_map<std::string, std::shared_ptr<Object>> lambdas;

inline std::shared_ptr<Scope> curr(new Scope);

inline std::vector<std::shared_ptr<Object>> arguments;

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

    void SetFirst(std::shared_ptr<Object> val);

    void SetSecond(std::shared_ptr<Object> val);

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class Lambda : public Object {
public:

    Lambda();

    Lambda(const std::vector<std::shared_ptr<Object>>&, const std::vector<std::string>&);

    std::shared_ptr<Object> Execute() override;

    std::string ToString() override;

    TypeObject GetType() override;

    std::shared_ptr<Object> Clone() override;

    std::vector<std::shared_ptr<Object>> GetBody();

    std::vector<std::string>& GetVars();

    std::shared_ptr<Scope>& GetScope();

private:

    std::vector<std::shared_ptr<Object>> body_;

    std::vector<std::string> l_vars_;

    std::shared_ptr<Scope> scope_;

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
