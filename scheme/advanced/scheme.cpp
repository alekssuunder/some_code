#include "scheme.h"
#include <unordered_map>
#include <sstream>
#include <error.h>
#include <vector>
#include <iostream>

struct Function {
    virtual std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) = 0;

    virtual ~Function() = default;
};

std::vector<std::shared_ptr<Object>> CellToVector(std::shared_ptr<Object> list) {
    std::vector<std::shared_ptr<Object>> ret;
    if (list == nullptr) {
        return ret;
    }
    while (Is<Cell>(As<Cell>(list)->GetSecond())) {
        ret.push_back(As<Cell>(list)->GetFirst());
        list = As<Cell>(list)->GetSecond();
    }
    if (As<Cell>(list)->GetFirst()) {
        ret.push_back(As<Cell>(list)->GetFirst());
    }
    else {
        ret.push_back(nullptr);
    }
    if (As<Cell>(list)->GetSecond()) {
        ret.push_back(As<Cell>(list)->GetSecond());
    }
    return ret;
}

template <class T>
bool IsListOfT(std::vector<std::shared_ptr<Object>>& list) {
    bool ret = true;
    for (size_t i = 0; i < list.size(); ++i) {
        if (!Is<T>(list[i])) {
            ret = false;
        }
    }
    return ret;
}

bool IsListOfSize(std::vector<std::shared_ptr<Object>>& list, size_t n) {
    return list.size() == n;
}

struct IsNumber : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        if (IsListOfT<Number>(list) && IsListOfSize(list, 1)) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct Greater {
    bool operator()(int a, int b) {
        return a > b;
    }
};

struct Less {
    bool operator()(int a, int b) {
        return a < b;
    }
};

struct GreaterEqual {
    bool operator()(int a, int b) {
        return a >= b;
    }
};

struct LessEqual {
    bool operator()(int a, int b) {
        return a <= b;
    }
};

struct Equal {
    bool operator()(int a, int b) {
        return a == b;
    }
};

template <class Comp>
struct Compare : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        if (!IsListOfT<Number>(list)) {
            throw RuntimeError("Wrong type");
        }
        bool f = true;
        for (size_t i = 1; i < list.size(); ++i) {
            if (!Comp()(As<Number>(list[i - 1])->GetValue(), As<Number>(list[i])->GetValue())) {
                f = false;
                break;
            }
        }
        if (f) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct Sum {
    int operator()(int a, int b) {
        return a + b;
    }
};

struct Mul {
    int operator()(int a, int b) {
        return a * b;
    }
};

template <class Op>
struct DefFirst : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        if (!IsListOfT<Number>(list)) {
            throw RuntimeError("Wrong type");
        }
        int ret;
        if (typeid(Op) == typeid(Sum)) {
            ret = 0;
        }
        else {
            ret = 1;
        }
        for (size_t i = 0; i < list.size(); ++i) {
            ret = Op()(ret, As<Number>(list[i])->GetValue());
        }
        return std::make_shared<Number>(ret);
    }
};

struct Minus {
    int operator()(int a, int b) {
        return a - b;
    }
};

struct Devide {
    int operator()(int a, int b) {
        return a / b;
    }
};

struct Max {
    int operator()(int a, int b) {
        return std::max(a, b);
    }
};

struct Min {
    int operator()(int a, int b) {
        return std::min(a, b);
    }
};

template <class Op>
struct NotDefFirst : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        if (!IsListOfT<Number>(list)) {
            throw RuntimeError("Wrong type");
        }
        if (IsListOfSize(list, 0)) {
            throw RuntimeError(std::string("No input for ") + std::string(typeid(Op).name()));
        }
        int ret = As<Number>(list[0])->GetValue();
        for (size_t i = 1; i < list.size(); ++i) {
            ret = Op()(ret, As<Number>(list[i])->GetValue());
        }
        return std::make_shared<Number>(ret);
    }
};

struct Abs : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        if (!IsListOfT<Number>(list)) {
            throw RuntimeError("Wrong type");
        }
        if (!IsListOfSize(list, 1)) {
            throw RuntimeError(std::string("Wrong input for abs"));
        }
        return std::make_shared<Number>(std::abs(As<Number>(list[0])->GetValue()));
    }
};

struct Quote : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 1)) {
            throw RuntimeError("Wrong input for quote");
        }
        return list[0];
    }
};

struct IsBoolean : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (IsListOfSize(list, 1) && IsListOfT<Symbol>(list) &&
            (As<Symbol>(list[0])->GetName() == "#t" || As<Symbol>(list[0])->GetName() == "#f")) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct Not : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 1)) {
            throw RuntimeError("Wrong input");
        }
        if (IsListOfT<Symbol>(list) && As<Symbol>(list[0])->GetName() == "#f") {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct And : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            list[i] = list[i]->Execute();
            if (Is<Symbol>(list[i]) && As<Symbol>(list[i])->GetName() == "#f") {
                return std::make_shared<Symbol>("#f");
            }
        }
        if (list.empty()) {
            return std::make_shared<Symbol>("#t");
        }
        return list.back();
    }
};

struct Or : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            list[i] = list[i]->Execute();
            if (Is<Symbol>(list[i]) && As<Symbol>(list[i])->GetName() == "#t") {
                return std::make_shared<Symbol>("#t");
            }
        }
        if (list.empty()) {
            return std::make_shared<Symbol>("#f");
        }
        return list.back();
    }
};

struct Pair : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 1)) {
            throw RuntimeError("Wrong input");
        }
        list = CellToVector(list[0]->Execute());
        if (IsListOfSize(list, 2)) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct Null : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 1)) {
            throw RuntimeError("Wrong input");
        }
        if (list[0]->Execute() == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct CheckList : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 1)) {
            throw RuntimeError("Wrong input");
        }
        std::shared_ptr<Object> li = list[0]->Execute();
        if (li == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        while (Is<Cell>(As<Cell>(li)->GetSecond())) {
            li = As<Cell>(li)->GetSecond();
        }
        if (As<Cell>(li)->GetSecond() == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct Cons : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 2)) {
            throw RuntimeError("Wrong input");
        }
        return std::make_shared<Cell>(list[0], list[1]);
    }
};

struct Car : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        if (As<Cell>(obj)->GetFirst() == nullptr || As<Cell>(obj)->GetSecond() != nullptr) {
            throw RuntimeError("Wrong input");
        }
        std::shared_ptr<Object> li = As<Cell>(obj)->GetFirst()->Execute();
        if (!Is<Cell>(li)) {
            throw RuntimeError("Wrong input");
        }
        return As<Cell>(li)->GetFirst();
    }
};

struct Cdr : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        if (As<Cell>(obj)->GetFirst() == nullptr || As<Cell>(obj)->GetSecond() != nullptr) {
            throw RuntimeError("Wrong input");
        }
        std::shared_ptr<Object> li = As<Cell>(obj)->GetFirst()->Execute();
        if (!Is<Cell>(li)) {
            throw RuntimeError("Wrong input");
        }
        return As<Cell>(li)->GetSecond();
    }
};

struct MakeList : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        return obj;
    }
};

struct ListRef : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 2)) {
            throw RuntimeError("Wrong input");
        }
        if (!Is<Cell>(list[0]) || !Is<Number>(list[1])) {
            throw RuntimeError("Wrong input");
        }
        size_t index = As<Number>(list[1])->GetValue();
        list = CellToVector(list[0]->Execute());
        if (index >= list.size()) {
            throw RuntimeError("Bad index in ListRef");
        }
        return list[index];
    }
};

struct ListTail : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 2)) {
            throw RuntimeError("Wrong input");
        }
        if (!Is<Cell>(list[0]) || !Is<Number>(list[1])) {
            throw RuntimeError("Wrong input");
        }
        size_t index = As<Number>(list[1])->GetValue();
        list = CellToVector(list[0]->Execute());
        if (index > list.size()) {
            throw RuntimeError("Bad index in ListRef");
        }
        std::vector<std::shared_ptr<Object>> ret;
        for (size_t i = index; i < list.size(); ++i) {
            ret.push_back(list[i]);
        }
        return BuildList(ret, 0);
    }
};

struct IsSymbol : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        if (IsListOfT<Symbol>(list) && IsListOfSize(list, 1)) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

struct Define : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 2)) {
            throw SyntaxError("Wrong syntax in define");
        }
        if (!Is<Symbol>(list[0])) {
            throw SyntaxError("Wrong syntax for variable name");
        }
        if (!list[1]) {
            throw RuntimeError("Wronggg");
        }
        list[1] = list[1]->Execute();
        curr->vars_[As<Symbol>(list[0])->GetName()] = list[1];
        return nullptr;
    }
};

struct Set : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (!IsListOfSize(list, 2)) {
            throw SyntaxError("Wrong syntax in set!");
        }
        if (!Is<Symbol>(list[0])) {
            throw SyntaxError("Wrong syntax for variable name in set!");
        }
        if (curr->vars_.find(As<Symbol>(list[0])->GetName()) == curr->vars_.end()) {
            throw NameError(std::string("No such variable: ") + As<Symbol>(list[0])->GetName());
        }
        if (!list[1]) {
            throw RuntimeError("Wronggg");
        }
        list[1] = list[1]->Execute();
        curr->vars_[As<Symbol>(list[0])->GetName()] = list[1];
        return nullptr;
    }
};

struct If : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (IsListOfSize(list, 2)) {
            if (!list[0]) {
                throw RuntimeError("Wronggg");
            }
            list[0] = list[0]->Execute();
            if (!(Is<Symbol>(list[0]) && (As<Symbol>(list[0])->GetName() == "#t" || As<Symbol>(list[0])->GetName() == "#f"))) {
                throw SyntaxError("Wrong conition type in if");
            }
            if (As<Symbol>(list[0])->GetName() == "#t") {
                if (!list[1]) {
                    throw RuntimeError("Wronggg");
                }
                list[1] = list[1]->Execute();
                return list[1];
            }
            else {
                return nullptr;
            }
        }
        else if (IsListOfSize(list, 3)) {
            if (!list[0]) {
                throw RuntimeError("Wronggg");
            }
            list[0] = list[0]->Execute();
            if (!(Is<Symbol>(list[0]) && (As<Symbol>(list[0])->GetName() == "#t" || As<Symbol>(list[0])->GetName() == "#f"))) {
                throw SyntaxError("Wrong condition type in if");
            }
            if (As<Symbol>(list[0])->GetName() == "#t") {
                if (!list[1]) {
                    throw RuntimeError("Wronggg");
                }
                list[1] = list[1]->Execute();
                return list[1];
            }
            else {
                if (!list[2]) {
                    throw RuntimeError("Wronggg");
                }
                list[2] = list[2]->Execute();
                return list[2];
            }
        }
        else {
            throw SyntaxError("Wrong input in if");
        }
    }
};

struct SetCar : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        for (size_t i = 0; i < list.size(); ++i) {
            if (!list[i]) {
                throw RuntimeError("Wronggg");
            }
            list[i] = list[i]->Execute();
        }
        As<Cell>(list[0])->SetFirst(As<Cell>(list[1]));
        return nullptr;
    }
};

struct MakeLambda : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        std::vector<std::shared_ptr<Object>> list = CellToVector(obj);
        if (list.size() < 2) {
            throw SyntaxError("alarm");
        }
        if (!Is<Cell>(list[1])) {
            throw SyntaxError("alarm-alarm");
        }
        std::vector<std::shared_ptr<Object>> l_args = CellToVector(list[1]);
        std::vector<std::shared_ptr<Object>> l_body;
        for (size_t i = 2; i < list.size(); ++i) {
            l_body.push_back(list[i]);
        }
        std::vector<std::string> l_vars;
        for (auto i : l_args) {
            l_vars.push_back(i->ToString());
        }
        return std::make_shared(l_body, l_vars);
    }
};

struct SetCdr : Function {
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> obj) override {
        auto first = As<Cell>(obj)->GetFirst();
        first = first->Execute();
        auto second = As<Cell>(As<Cell>(obj)->GetSecond())->GetFirst();
        second = second->Execute();

        As<Cell>(first)->SetSecond(second);
        return nullptr;
    }
};

std::unordered_map<std::string, std::shared_ptr<Function>> k_functions{
    {"number?", std::make_shared<IsNumber>()},
    {">", std::make_shared<Compare<Greater>>()},
    {"<", std::make_shared<Compare<Less>>()},
    {">=", std::make_shared<Compare<GreaterEqual>>()},
    {"<=", std::make_shared<Compare<LessEqual>>()},
    {"=", std::make_shared<Compare<Equal>>()},
    {"+", std::make_shared<DefFirst<Sum>>()},
    {"*", std::make_shared<DefFirst<Mul>>()},
    {"-", std::make_shared<NotDefFirst<Minus>>()},
    {"/", std::make_shared<NotDefFirst<Devide>>()},
    {"max", std::make_shared<NotDefFirst<Max>>()},
    {"min", std::make_shared<NotDefFirst<Min>>()},
    {"abs", std::make_shared<Abs>()},
    {"quote", std::make_shared<Quote>()},
    {"boolean?", std::make_shared<IsBoolean>()},
    {"not", std::make_shared<Not>()},
    {"and", std::make_shared<And>()},
    {"or", std::make_shared<Or>()},
    {"pair?", std::make_shared<Pair>()},
    {"null?", std::make_shared<Null>()},
    {"list?", std::make_shared<CheckList>()},
    {"cons", std::make_shared<Cons>()},
    {"car", std::make_shared<Car>()},
    {"cdr", std::make_shared<Cdr>()},
    {"list", std::make_shared<MakeList>()},
    {"list-ref", std::make_shared<ListRef>()},
    {"list-tail", std::make_shared<ListTail>()},
    {"symbol?", std::make_shared<IsSymbol>()},
    {"define", std::make_shared<Define>()},
    {"set!", std::make_shared<Set>()},
    {"if", std::make_shared<If>()},
    {"set-car!", std::make_shared<SetCar>()},
    {"set-cdr!", std::make_shared<SetCdr>()},
    {"lambda", std::make_shared<MakeLambda>()}

};

std::string Interpreter::Run(const std::string& str) {
    std::stringstream ss{ str };
    Tokenizer tokenizer(&ss);
    auto obj = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Wrong input");
    }
    if (!obj) {
        throw RuntimeError("You typed nothing");
    }
    obj = obj->Execute();
    if (obj == nullptr) {
        return "()";
    }
    if (Is<Cell>(obj)) {
        return std::string("(") + obj->ToString() + std::string(")");
    }
    return obj->ToString();
}

std::shared_ptr<Object> Number::Execute() {
    return this->Clone();
}

std::shared_ptr<Object> Symbol::Execute() {
    std::string name = GetName();
    if (name == "#f" || name == "#t") {
        return this->Clone();
    }
    if (curr->vars_.find(name) == curr->vars_.end()) {
        throw NameError(std::string("No such variable: ") + name);
    }
    return curr->vars_.find(name)->second;
}

std::shared_ptr<Object> Cell::Execute() {
    if (first_ == nullptr) {
        throw RuntimeError("No function was typed");
    }

    if (Is<Cell>(first_) && Is<Symbol>(As<Cell>(first_)->GetFirst()) && As<Symbol>(As<Cell>(first_)->GetFirst())->GetName() == "lambda") {
        arguments = CellToVector(second_);
        std::shared_ptr<Object> lmbd = (*k_functions["lambda"])(first_));
        return lmbd->Execute();
    }

    if (!Is<Symbol>(first_)) {
        throw RuntimeError("Wrong name of function");
    }
    std::string fun_name = As<Symbol>(first_)->GetName();
    
    /*if (curr->vars_.find(fun_name) != curr->vars_.end()) {
        auto input = CellToVector(second_);
        auto lambda = As<Lambda>(curr->vars_[fun_name]);
        for (size_t i = 0; i < lambda->GetVars().size(); ++i) {
            lambda->GetScope()->vars_[lambda->GetVars()[i]] = input[i]->Execute();
        }
        return lambda->Execute();
    }*/
    if (k_functions.find(fun_name) == k_functions.end()) {
        throw RuntimeError("No such function");
    }
    if (second_ != nullptr && !Is<Cell>(second_)) {
        throw RuntimeError("Shit happens");
    }
    if (fun_name == "define") {
        (*k_functions[fun_name])(second_);
        return first_;
    }
    return (*k_functions[fun_name])(second_);
}
