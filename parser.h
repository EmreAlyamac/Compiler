#pragma once

#include "lexer.h"
#include "node.h"
#include <memory>

struct AST {
  ProgramNode *root;
  std::vector<std::unique_ptr<Node>> nodes;
};
AST Parse(std::vector<Token> tokens);
