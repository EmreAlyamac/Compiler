#include "parser.h"
#include "lexer.h"
#include "node.h"

#include <memory>
#include <stdexcept>

AST Parse(std::vector<Token> tokens) {
  AST ast;

  ast.nodes.push_back(std::make_unique<ProgramNode>());
  ast.root = static_cast<ProgramNode *>(ast.nodes.back().get());

  size_t i = 0;

  while (tokens[i].type != EOF_TOKEN) {
    if (tokens[i].type == KW_INT) {
      i++;

      if (tokens[i].type != IDENTIFIER) {
        throw std::runtime_error("Expected variable name");
      }

      std::string name = tokens[i].value;
      i++;

      if (tokens[i].type == ASSIGN) {
        i++;
        if (tokens[i].type != INTEGER) {
          throw std::runtime_error("Expected integer");
        }

        auto number = std::make_unique<NumberNode>(std::stoi(tokens[i].value));
        Node *numberPtr = number.get();
        ast.nodes.push_back(std::move(number));
        i++;

        if (tokens[i].type != SEMICOLON) {
          throw std::runtime_error("Expected ;");
        }
        i++;

        auto type = std::make_unique<TypeNode>("int");
        TypeNode *typePtr = type.get();
        ast.nodes.push_back(std::move(type));

        auto decl =
            std::make_unique<VariableDeclNode>(name, typePtr, numberPtr);

        ast.root->statements.push_back(decl.get());
        ast.nodes.push_back(std::move(decl));

      } else if (tokens[i].type == SEMICOLON) {
        auto type = std::make_unique<TypeNode>("int");
        TypeNode *typePtr = type.get();
        ast.nodes.push_back(std::move(type));

        auto decl = std::make_unique<VariableDeclNode>(name, typePtr, nullptr);
        ast.root->statements.push_back(decl.get());
        ast.nodes.push_back(std::move(decl));

        i++;
        continue;
      }
      // TODO: Function support will break here
      else {
        throw std::runtime_error(
            "Expected = or ; (Functions curently not supported)");
      }
      continue;
    }

    if (tokens[i].type == IDENTIFIER) {
      std::string name = tokens[i].value;
      i++;

      if (tokens[i].type != ASSIGN) {
        throw std::runtime_error("Expected = in assignment");
      }
      i++;

      if (tokens[i].type != INTEGER) {
        throw std::runtime_error("Expected integer");
      }

      auto number = std::make_unique<NumberNode>(std::stoi(tokens[i].value));
      Node *numberPtr = number.get();
      ast.nodes.push_back(std::move(number));
      i++;

      if (tokens[i].type != SEMICOLON) {
        throw std::runtime_error("Expected ;");
      }
      i++;

      auto assignment = std::make_unique<AssignmentNode>(name, numberPtr);
      ast.root->statements.push_back(assignment.get());
      ast.nodes.push_back(std::move(assignment));

      continue;
    }
    throw std::runtime_error("Unexpected token: " + tokens[i].value);
  }

  return ast;
}
