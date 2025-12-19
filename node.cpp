#include "node.h"
#include <string>
#include <vector>

class BinaryExpressionNode : public Node {
public:
  Node *left = nullptr;
  Node *right = nullptr;
  BinaryExpressionNode(Node *left, Node *right) : left(left), right(right) {}
  virtual ~BinaryExpressionNode() = default;
};

class TypeNode : public Node {
public:
  std::string type;
  TypeNode(std::string type) : type(std::move(type)) {}
  virtual ~TypeNode() = default;
};

class VariableDeclNode : public Node {
public:
  std::string name;
  TypeNode *declaredType = nullptr;
  Node *initializer = nullptr;

  VariableDeclNode(std::string name, TypeNode *type, Node *init)
      : name(std::move(name)), declaredType(type), initializer(init) {}
  virtual ~VariableDeclNode() = default;
};