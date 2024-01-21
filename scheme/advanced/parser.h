#pragma once

#include <memory>
#include <vector>

#include "object.h"
#include <tokenizer.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer);
std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);
std::shared_ptr<Object> BuildList(const std::vector<std::shared_ptr<Object>>& list, size_t pos);
