#pragma once
#include <string>
#include <vector>

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
  BinaryExpressionNode(Node *left, Node *right) : left(left), right(right) {}
  virtual ~BinaryExpressionNode() = default;
  virtual std::string to_string() override {
    return "(" + left->to_string() + " " + right->to_string() + ")";
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