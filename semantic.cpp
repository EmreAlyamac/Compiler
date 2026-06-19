#include "semantic.h"
#include <iostream>
#include <stdexcept>

Scope::Scope(Scope *parent) : parent(parent) {}

bool Scope::Declare(const VariableSymbol &symbol) {
  if (variables.find(symbol.name) != variables.end()) {
    return false;
  }
  variables[symbol.name] = symbol;
  return true;
}

const VariableSymbol *Scope::Resolve(const std::string &name) const {
  auto it = variables.find(name);
  if (it != variables.end()) {
    return &it->second;
  }
  if (parent != nullptr) {
    return parent->Resolve(name);
  }
  return nullptr;
}

void SemanticAnalyzer::Analyze(AST &ast) {
  auto *program = dynamic_cast<ProgramNode *>(ast.root);
  if (program == nullptr) {
    throw std::runtime_error("Sematic analyzer error: Program node not found");
  }

  for (Node *statement : program->statements) {
    auto function = dynamic_cast<FunctionDeclNode *>(statement);
    if (function == nullptr) { // curently doesnt allow global variables
      // TODO: add support for global variables
      throw std::runtime_error("Sematic analyzer error: Top level statement is "
                               "not a function declaration");
    }
    if (functions.find(function->name) != functions.end()) {
      std::string parameters = "(";

      for (std::size_t i = 0; i < function->parameters.size(); ++i) {
        if (i > 0) {
          parameters += ", ";
        }

        ParameterNode *parameter = function->parameters[i];
        parameters += parameter->type->to_string();
      }

      parameters += ")";

      const std::string returnType = function->returnType->to_string();

      throw std::runtime_error("Semantic analyzer error: Function '" +
                               function->name + "' with parameters " +
                               parameters + " and return type '" + returnType +
                               "' is already declared");
    }
    FunctionSymbol symbol;
    symbol.name = function->name;
    symbol.returnType = function->returnType;
    for (ParameterNode *parameter : function->parameters) {
      symbol.parameters.push_back(parameter);
    }
    functions[function->name] = symbol;
  }

  for (Node *statement : program->statements) {
    auto *function = dynamic_cast<FunctionDeclNode *>(statement);

    AnalyzeFunction(function);
  }
}

void SemanticAnalyzer::AnalyzeFunction(FunctionDeclNode *function) {
  Scope functionScope;

  for (ParameterNode *parameter : function->parameters) {
    VariableSymbol symbol{parameter->name, parameter->type->to_string()};

    if (!functionScope.Declare(symbol)) {
      throw std::runtime_error(
          "Semantic analyzer error: Parameter " + symbol.name + " with type " +
          symbol.type + " already declared in function " + function->name);
    }
  }
  AnalyzeBlock(function->body, functionScope);
}

void SemanticAnalyzer::AnalyzeBlock(BlockNode *block, Scope &parentScope) {
  Scope blockScope(&parentScope);
  for (Node *statement : block->statements) {
    AnalyzeStatement(statement, blockScope);
  }
}

void SemanticAnalyzer::AnalyzeStatement(Node *statement, Scope &scope) {
  auto *variable = dynamic_cast<VariableDeclNode *>(statement);

  if (variable == nullptr) {
    // We do not analyze other statement types yet.
    return;
  }

  if (variable->initializer != nullptr) {
    std::string initializerType =
        AnalyzeExpression(variable->initializer, scope);

    if (initializerType != variable->declaredType->to_string()) {
      throw std::runtime_error(
          "Semantic analyzer error: Variable '" + variable->name +
          "' declared as '" + variable->declaredType->to_string() +
          "' cannot be initialized with type '" + initializerType + "'");
    }
  }

  VariableSymbol symbol{
      variable->name,
      variable->declaredType->to_string(),
  };

  if (!scope.Declare(symbol)) {
    throw std::runtime_error("Semantic analyzer error: Variable '" +
                             symbol.name + "' with type '" + symbol.type +
                             "' is already declared in this scope");
  }
}

std::string SemanticAnalyzer::AnalyzeExpression(Node *expression,
                                                Scope &scope) {
  if (dynamic_cast<NumberNode *>(expression) != nullptr) {
    return "int";
  }

  if (auto *identifier = dynamic_cast<IdentifierNode *>(expression)) {
    const VariableSymbol *symbol = scope.Resolve(identifier->name);

    if (symbol == nullptr) {
      throw std::runtime_error("Semantic analyzer error: Variable '" +
                               identifier->name + "' was not declared");
    }

    return symbol->type;
  }

  throw std::runtime_error("Semantic analyzer error: Unsupported expression");
}