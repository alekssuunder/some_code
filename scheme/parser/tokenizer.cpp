#include <tokenizer.h>
#include <error.h>

const char kSpace = ' ';
const char kEndLine = '\n';
const char kLeftBracket = '(';
const char kRightBracket = ')';
const char kQuote = '\'';
const char kDot = '.';
const char kPlus = '+';
const char kMinus = '-';
const std::string kStartPossibleChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*/#";
const std::string kOtherPossibleChar =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*/#0123456789?!-";
const std::string kDigits = "0123456789";

SymbolToken::SymbolToken(const std::string& val) : name(val) {
}

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

ConstantToken::ConstantToken(int val) : value(val) {
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream* in) : in_(in), token_(nullptr) {
    this->Next();
}

bool Tokenizer::IsEnd() {
    if (in_->peek() == EOF && token_ == nullptr) {
        return true;
    } else {
        return false;
    }
}

void Tokenizer::Next() {
    while (in_->peek() == kSpace || in_->peek() == kEndLine) {
        in_->get();
    }
    if (in_->peek() == EOF) {
        token_.reset(nullptr);
        return;
    }
    char curr = in_->peek();
    if (curr == kLeftBracket) {
        in_->get();
        token_.reset(new Token(BracketToken::OPEN));
    } else if (curr == kRightBracket) {
        in_->get();
        token_.reset(new Token(BracketToken::CLOSE));
    } else if (curr == kQuote) {
        in_->get();
        token_.reset(new Token(QuoteToken()));
    } else if (curr == kDot) {
        in_->get();
        token_.reset(new Token(DotToken()));
    } else if (kDigits.find(curr) != std::string::npos) {
        std::string temp;
        while (kDigits.find(curr) != std::string::npos) {
            temp.push_back(in_->get());
            curr = in_->peek();
        }
        token_.reset(new Token(ConstantToken(std::stoi(temp))));
    } else if (curr == kPlus) {
        std::string temp;
        temp.push_back(in_->get());
        curr = in_->peek();
        if (kDigits.find(curr) != std::string::npos) {
            while (kDigits.find(curr) != std::string::npos) {
                temp.push_back(in_->get());
                curr = in_->peek();
            }
            token_.reset(new Token(ConstantToken(std::stoi(temp))));
        } else {
            token_.reset(new Token(SymbolToken(temp)));
        }
    } else if (curr == kMinus) {
        std::string temp;
        temp.push_back(in_->get());
        curr = in_->peek();
        if (kDigits.find(curr) != std::string::npos) {
            while (kDigits.find(curr) != std::string::npos) {
                temp.push_back(in_->get());
                curr = in_->peek();
            }
            token_.reset(new Token(ConstantToken(std::stoi(temp))));
        } else {
            token_.reset(new Token(SymbolToken(temp)));
        }
    } else if (kStartPossibleChar.find(curr) != std::string::npos) {
        std::string temp;
        while (kOtherPossibleChar.find(curr) != std::string::npos) {
            temp.push_back(in_->get());
            curr = in_->peek();
        }
        if (curr != EOF && curr != kSpace && curr != kEndLine) {
            throw SyntaxError("Unresolved mid character");
        } else {
            token_.reset(new Token(SymbolToken(temp)));
        }
    } else {
        throw SyntaxError("Unresolved start character");
    }
}

Token Tokenizer::GetToken() {
    return *(token_.get());
}