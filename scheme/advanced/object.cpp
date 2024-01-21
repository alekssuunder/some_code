#include <object.h>

// Number
Number::Number(int val) : val_(val) {
}

int Number::GetValue() const {
    return val_;
}

std::string Number::ToString() {
    return std::to_string(GetValue());
}

TypeObject Number::GetType() {
    return TypeObject::NUMBER;
}

std::shared_ptr<Object> Number::Clone() {
    return std::make_shared<Number>(val_);
}

// Symbol
Symbol::Symbol(const std::string& val) : val_(val) {
}

const std::string& Symbol::GetName() const {
    return val_;
}

std::string Symbol::ToString() {
    return GetName();
}

TypeObject Symbol::GetType() {
    return TypeObject::SYMBOL;
}

std::shared_ptr<Object> Symbol::Clone() {
    return std::make_shared<Symbol>(val_);
}

// Cell
Cell::Cell(const std::shared_ptr<Object>& first, const std::shared_ptr<Object>& second)
    : first_(first), second_(second) {
}

std::string Cell::ToString() {
    if (!GetFirst()) {
        return "()";
    }
    if (!GetSecond()) {
        return GetFirst()->ToString();
    } else if (GetSecond()->GetType() != TypeObject::CELL) {
        return GetFirst()->ToString() + " . " + GetSecond()->ToString();
    } else {
        return GetFirst()->ToString() + " " + GetSecond()->ToString();
    }
}

TypeObject Cell::GetType() {
    return TypeObject::CELL;
}

std::shared_ptr<Object> Cell::Clone() {
    return std::make_shared<Cell>(first_, second_);
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}
std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}

void Cell::SetFirst(std::shared_ptr<Object> val) {
    first_ = val;
}

void Cell::SetSecond(std::shared_ptr<Object> val) {
    second_ = val;
}

//lambda

Lambda::Lambda() {
    scope_ = std::make_shared<Scope>();
    scope_->prev_ = curr;
}

Lambda::Lambda(const std::vector<std::shared_ptr<Object>>& body, const std::vector<std::string>& vars) {
    body_ = body;
    l_vars_ = vars;
}

std::shared_ptr<Object> Lambda::Execute() {
    curr = scope_;
    for (size_t i = 0; i < body_.size() - 1; ++i) {
        body_[i]->Execute();
    }
    auto ret = body_.back()->Execute();
    curr = curr->prev_;
    return ret;
}

//вообще, у лямбде никогда не будет вызываться ToString()
//поэтому эта функция нужна просто потому что лямбда это Object
std::string Lambda::ToString() {
    return "";
}

//насчет нужности этого метода я тоже не уверен, но тоже просто нужно его сделать
TypeObject Lambda::GetType() {
    return TypeObject::LAMBDA;
}

//аналогично GetType()
std::shared_ptr<Object> Lambda::Clone() {
    return std::make_shared<Lambda>();
}

std::vector<std::string>& Lambda::GetVars() {
    return l_vars_;
}

std::shared_ptr<Scope>& Lambda::GetScope() {
    return scope_;
}
