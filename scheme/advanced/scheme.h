#pragma once

#include <string>
#include <parser.h>
#include <unordered_map>

class Interpreter {
public:
    Interpreter() {
        lambdas.clear();
        curr->vars_.clear();
    }
    std::string Run(const std::string&);
};
