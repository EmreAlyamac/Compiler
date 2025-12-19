#pragma once
#include <string>
#include <vector>

class Node {
public:
  Node *parent = nullptr;
  virtual ~Node() = default;
};

class ProgramNode : public Node {};
