#include "parser.h"
#include "lexer.h"
#include "node.h"

#include <memory>
#include <stdexcept>

AST Parse(std::vector<Token> tokens) {
  if (tokens.empty()) {
    throw std::runtime_error("Input file can't be tokenized");
  }

  AST ast;

  ast.nodes.push_back(std::make_unique<ProgramNode>());
  ast.root = static_cast<ProgramNode *>(ast.nodes.back().get());

  return ast;
}
