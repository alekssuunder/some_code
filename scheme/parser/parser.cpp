#include <parser.h>
#include <error.h>
#include <vector>

Number::Number(int val) : val_(val) {
}

int Number::GetValue() const {
    return val_;
}

Symbol::Symbol(const std::string& val) : val_(val) {
}

const std::string& Symbol::GetName() const {
    return val_;
}

Cell::Cell(const std::shared_ptr<Object>& first, const std::shared_ptr<Object>& second)
    : first_(first), second_(second) {
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {

    if (tokenizer->IsEnd()) {
        throw SyntaxError("Empty");
    }
    Token t = tokenizer->GetToken();
    tokenizer->Next();

    if (std::get_if<ConstantToken>(&t)) {
        return std::make_shared<Number>(std::get<ConstantToken>(t).value);
    } else if (std::get_if<BracketToken>(&t)) {
        if (std::get<BracketToken>(t) == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            throw SyntaxError("No matching open bracket for close bracket");
        }
    } else if (std::get_if<SymbolToken>(&t)) {
        return std::make_shared<Symbol>(std::get<SymbolToken>(t).name);
    } else if (std::get_if<QuoteToken>(&t)) {
        throw SyntaxError("Quote");
    } else {
        return std::make_shared<Symbol>(".");
    }
}

std::shared_ptr<Object> BuildList(const std::vector<std::shared_ptr<Object>>& list, size_t pos) {

    if (pos == list.size()) {
        return nullptr;
    } else {
        if (list[pos] && list[pos]->GetType() == TypeObject::SYMBOL &&
            static_pointer_cast<Symbol>(list[pos])->GetName() == ".") {
            if (pos != 0 && pos == list.size() - 2) {
                return list.back();
            } else {
                throw SyntaxError("Bad dot");
            }
        }
        return std::make_shared<Cell>(list[pos], BuildList(list, pos + 1));
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {

    if (tokenizer->IsEnd()) {
        throw SyntaxError("No matching close bracket for open bracket");
    }

    std::vector<std::shared_ptr<Object>> list;

    Token t = tokenizer->GetToken();

    while (!tokenizer->IsEnd() &&
           !(std::get_if<BracketToken>(&t) && std::get<BracketToken>(t) == BracketToken::CLOSE)) {
        list.push_back(Read(tokenizer));
        if (!tokenizer->IsEnd()) {
            t = tokenizer->GetToken();
        }
    }
    if (tokenizer->IsEnd()) {
        throw SyntaxError("No matching close bracket for open bracket");
    }
    tokenizer->Next();
    return BuildList(list, 0);
}