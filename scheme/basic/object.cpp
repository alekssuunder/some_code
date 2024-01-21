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