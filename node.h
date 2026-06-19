#pragma once
#include <string>
#include <vector>

inline std::string IndentLines(const std::string &text,
                               const std::string &indent) {
  std::string result = indent;

  for (char c : text) {
    result += c;
    if (c == '\n') {
      result += indent;
    }
  }

  return result;
}

class Node {
public:
  Node *parent = nullptr;
  virtual std::string to_string() = 0;
  virtual ~Node() = default;
};

class ExpressionNode : public Node {};

class StatementNode : public Node {};

class BinaryExpressionNode : public ExpressionNode {
public:
  Node *left = nullptr;
  Node *right = nullptr;
  std::string op;
  BinaryExpressionNode(Node *left, Node *right, std::string op)
      : left(left), right(right), op(op) {}
  virtual ~BinaryExpressionNode() = default;
  virtual std::string to_string() override {
    return "(" + left->to_string() + " " + op + " " + right->to_string() + ")";
  }
};

class ReturnNode : public StatementNode {
public:
  Node *value = nullptr;

  ReturnNode(Node *value) : value(value) {}

  std::string to_string() override {
    if (value == nullptr) {
      return "return";
    }

    return "return " + value->to_string();
  }
};

class BlockNode : public StatementNode {
public:
  std::vector<Node *> statements;

  std::string to_string() override {
    std::string result = "{\n";

    for (const auto &statement : statements) {
      result += IndentLines(statement->to_string(), "  ") + "\n";
    }

    result += "}";
    return result;
  }
};

class IdentifierNode : public ExpressionNode {
public:
  std::string name;

  IdentifierNode(std::string name) : name(std::move(name)) {}

  std::string to_string() override { return name; }
};

class NumberNode : public Node {
public:
  int value;
  NumberNode(int value) : value(value) {}
  virtual ~NumberNode() = default;
  virtual std::string to_string() override { return std::to_string(value); }
};

class TypeNode : public Node {
public:
  std::string type;
  TypeNode(std::string type) : type(std::move(type)) {}
  virtual ~TypeNode() = default;
  virtual std::string to_string() override { return type; }
};

class VariableDeclNode : public Node {
public:
  std::string name;
  TypeNode *declaredType = nullptr;
  Node *initializer = nullptr;

  VariableDeclNode(std::string name, TypeNode *type, Node *init)
      : name(std::move(name)), declaredType(type), initializer(init) {}
  virtual ~VariableDeclNode() = default;
  virtual std::string to_string() override {
    std::string result = "var " + name;

    if (initializer != nullptr) {
      result += " = " + initializer->to_string();
    }

    return result;
  }
};

class ProgramNode : public Node {
public:
  std::vector<Node *> statements;
  virtual std::string to_string() override {
    std::string result = "";
    for (const auto &statement : statements) {
      result += statement->to_string() + "\n";
    }
    return result;
  }
};

class AssignmentNode : public StatementNode {
public:
  std::string name;
  Node *value = nullptr;

  AssignmentNode(std::string name, Node *value)
      : name(std::move(name)), value(value) {}

  virtual ~AssignmentNode() = default;
  virtual std::string to_string() override {
    return name + " = " + value->to_string();
  }
};

class IfNode : public StatementNode {
public:
  Node *condition = nullptr;
  Node *thenBlock = nullptr;
  Node *elseBlock = nullptr;

  IfNode(Node *condition, Node *thenBlock, Node *elseBlock)
      : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

  std::string to_string() override {
    std::string result = "if (" + condition->to_string() + ") ";
    result += thenBlock->to_string();

    if (elseBlock != nullptr) {
      result += " else ";
      result += elseBlock->to_string();
    }

    return result;
  }
};

class WhileNode : public StatementNode {
public:
  Node *condition = nullptr;
  Node *body = nullptr;

  WhileNode(Node *condition, Node *body) : condition(condition), body(body) {}

  std::string to_string() override {
    return "while (" + condition->to_string() + ") " + body->to_string();
  }
};

class ParameterNode : public Node {
public:
  TypeNode *type = nullptr;
  std::string name;

  ParameterNode(TypeNode *type, std::string name)
      : type(type), name(std::move(name)) {}

  std::string to_string() override {
    std::string result = type->to_string() + " " + name;
    return result;
  }
};

class FunctionDeclNode : public Node {
public:
  std::string name;
  TypeNode *returnType = nullptr;
  std::vector<ParameterNode *> parameters;
  BlockNode *body = nullptr;

  FunctionDeclNode(std::string name, TypeNode *returnType,
                   std::vector<ParameterNode *> parameters, BlockNode *body)
      : name(std::move(name)), returnType(returnType),
        parameters(std::move(parameters)), body(body) {}

  std::string to_string() override {
    std::string result = "function " + name + "(";

    for (size_t i = 0; i < parameters.size(); i++) {
      result += parameters[i]->to_string();

      if (i + 1 < parameters.size()) {
        result += ", ";
      }
    }

    result += ") returns " + returnType->to_string() + " ";
    result += body->to_string();

    return result;
  }
};

class FunctionCallNode : public ExpressionNode {
public:
  std::string name;
  std::vector<Node *> arguments;

  FunctionCallNode(std::string name, std::vector<Node *> arguments)
      : name(std::move(name)), arguments(std::move(arguments)) {}

  std::string to_string() override {
    std::string result = name + "(";

    for (size_t i = 0; i < arguments.size(); i++) {
      result += arguments[i]->to_string();
      if (i + 1 < arguments.size()) {
        result += ", ";
      }
    }

    result += ")";
    return result;
  }
};

class PostfixExpressionNode : public ExpressionNode {
public:
  Node *operand = nullptr;
  std::string op;

  PostfixExpressionNode(Node *operand, std::string op)
      : operand(operand), op(std::move(op)) {}

  std::string to_string() override { return operand->to_string() + op; }
};

class ForNode : public StatementNode {
public:
  Node *initialization = nullptr;
  Node *condition = nullptr;
  Node *increment = nullptr;
  Node *body = nullptr;

  ForNode(Node *initialization, Node *condition, Node *increment, Node *body)
      : initialization(initialization), condition(condition),
        increment(increment), body(body) {}

  std::string to_string() override {
    std::string result = "for (";
    if (initialization != nullptr) {
      result += initialization->to_string();
    }
    result += "; ";
    if (condition != nullptr) {
      result += condition->to_string();
    }
    result += "; ";
    if (increment != nullptr) {
      result += increment->to_string();
    }
    result += ") ";
    result += body->to_string();
    return result;
  }
};

class BreakNode : public StatementNode {
public:
  std::string to_string() override { return "break"; }
};

class ContinueNode : public StatementNode {
public:
  std::string to_string() override { return "continue"; }
};

class ExpressionStatementNode : public StatementNode {
public:
  Node *expression = nullptr;

  ExpressionStatementNode(Node *expression) : expression(expression) {}

  std::string to_string() override { return expression->to_string(); }
};