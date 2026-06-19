#pragma once

#include "node.h"
#include "parser.h"
#include <string>
#include <unordered_map>
#include <vector>

struct FunctionSymbol {
  std::string name;
  TypeNode *returnType;
  std::vector<ParameterNode *> parameters;
};

struct VariableSymbol {
  std::string name;
  std::string type;
};

class Scope {
public:
  explicit Scope(Scope *parent = nullptr);
  bool Declare(const VariableSymbol &symbol);
  const VariableSymbol *Resolve(const std::string &name) const;

private:
  Scope *parent;
  std::unordered_map<std::string, VariableSymbol> variables;
};

class SemanticAnalyzer {
public:
  void Analyze(AST &ast);

private:
  std::unordered_map<std::string, FunctionSymbol> functions;
  void AnalyzeFunction(FunctionDeclNode *function);
  void AnalyzeBlock(BlockNode *block, Scope &scope);
  void AnalyzeStatement(Node *statement, Scope &scope);
  std::string AnalyzeExpression(Node *expression, Scope &scope);
};
