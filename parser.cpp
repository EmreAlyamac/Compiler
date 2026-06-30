#include "parser.h"
#include "lexer.h"
#include "node.h"

#include <memory>
#include <stdexcept>

Node *ParseExpression(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParseStatement(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParseBlock(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParseIf(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParseWhile(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParseFunctionDecl(AST &ast, std::vector<Token> &tokens, size_t &i);
TypeNode *ParseType(AST &ast, std::vector<Token> &tokens, size_t &i);
ParameterNode *ParseParameter(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParseFunctionCall(AST &ast, std::vector<Token> &tokens, size_t &i);
std::vector<ParameterNode *>
ParseParameterList(AST &ast, std::vector<Token> &tokens, size_t &i);
Node *ParsePostfixExpression(AST &ast, std::vector<Token> &tokens, size_t &i);

// Return pointer to the parsed expression either integer or a variable or a
// parenthesized expression
Node *ParseSimpleExpression(AST &ast, std::vector<Token> &tokens, size_t &i) {
  if (tokens[i].type == INTEGER) {
    auto number = std::make_unique<NumberNode>(std::stoi(tokens[i].value));
    Node *ptr = number.get();
    ast.nodes.push_back(std::move(number));
    i++;
    return ptr;
  }

  if (tokens[i].type == IDENTIFIER) {
    std::string name = tokens[i].value;
    i++; // consume identifier

    if (tokens[i].type == LPAREN) {
      i++; // consume (

      std::vector<Node *> arguments;

      if (tokens[i].type != RPAREN) {
        while (true) {
          Node *argument = ParseExpression(ast, tokens, i);
          arguments.push_back(argument);

          if (tokens[i].type == COMMA) {
            i++; // consume ,
            continue;
          }

          break;
        }
      }

      if (tokens[i].type != RPAREN) {
        throw std::runtime_error("Expected ) after function call arguments");
      }
      i++; // consume )

      auto call = std::make_unique<FunctionCallNode>(name, arguments);
      Node *ptr = call.get();
      ast.nodes.push_back(std::move(call));
      return ptr;
    }

    auto identifier = std::make_unique<IdentifierNode>(name);
    Node *ptr = identifier.get();
    ast.nodes.push_back(std::move(identifier));
    return ptr;
  }

  if (tokens[i].type == LPAREN) {
    i++;
    Node *expr = ParseExpression(ast, tokens, i);
    if (tokens[i].type != RPAREN) {
      throw std::runtime_error("Expected )");
    }
    i++;
    return expr;
  }

  throw std::runtime_error("Expected integer, identifier, or (expression)");
}

Node *ParseMulDiv(AST &ast, std::vector<Token> &tokens, size_t &i) {
  Node *left = ParsePostfixExpression(ast, tokens, i);

  while (tokens[i].type == MULTIPLY || tokens[i].type == DIVIDE) {
    std::string op = tokens[i].value;
    i++;

    Node *right = ParsePostfixExpression(ast, tokens, i);

    auto binary = std::make_unique<BinaryExpressionNode>(left, right, op);
    left = binary.get();
    ast.nodes.push_back(std::move(binary));
  }

  return left;
}

Node *ParseAddSub(AST &ast, std::vector<Token> &tokens, size_t &i) {
  Node *left = ParseMulDiv(ast, tokens, i);

  while (tokens[i].type == PLUS || tokens[i].type == MINUS) {
    std::string op = tokens[i].value;
    i++;

    Node *right = ParseMulDiv(ast, tokens, i);

    auto binary = std::make_unique<BinaryExpressionNode>(left, right, op);
    left = binary.get();
    ast.nodes.push_back(std::move(binary));
  }

  return left;
}

Node *ParseComparison(AST &ast, std::vector<Token> &tokens, size_t &i) {
  Node *left = ParseAddSub(ast, tokens, i);

  while (tokens[i].type == LESS || tokens[i].type == GREATER ||
         tokens[i].type == LESS_EQUAL || tokens[i].type == GREATER_EQUAL ||
         tokens[i].type == EQUAL || tokens[i].type == NOT_EQUAL) {
    std::string op = tokens[i].value;
    i++;

    Node *right = ParseAddSub(ast, tokens, i);

    auto binary = std::make_unique<BinaryExpressionNode>(left, right, op);
    left = binary.get();
    ast.nodes.push_back(std::move(binary));
  }

  return left;
}

Node *ParseExpression(AST &ast, std::vector<Token> &tokens, size_t &i) {
  return ParseComparison(ast, tokens, i);
}

Node *ParseReturn(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++; // consume return

  Node *value = nullptr;
  if (tokens[i].type != SEMICOLON) {
    value = ParseExpression(ast, tokens, i);
  }

  if (tokens[i].type != SEMICOLON) {
    throw std::runtime_error("Expected ;");
  }

  i++;
  auto returnNode = std::make_unique<ReturnNode>(value);
  Node *ptr = returnNode.get();
  ast.nodes.push_back(std::move(returnNode));
  return ptr;
}

Node *ParseIntDeclaration(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++;
  if (tokens[i].type != IDENTIFIER) {
    throw std::runtime_error("Expected variable name");
  }
  std::string name = tokens[i].value;
  i++;
  if (tokens[i].type == ASSIGN) {
    i++;

    Node *value = ParseExpression(ast, tokens, i);
    if (tokens[i].type != SEMICOLON) {
      throw std::runtime_error("Expected ;");
    }
    i++;

    auto type = std::make_unique<TypeNode>("int");
    TypeNode *typePtr = type.get();
    ast.nodes.push_back(std::move(type));

    auto decl = std::make_unique<VariableDeclNode>(name, typePtr, value);

    Node *ptr = decl.get();
    ast.nodes.push_back(std::move(decl));
    return ptr;
  }
  if (tokens[i].type == SEMICOLON) {
    i++;
    auto type = std::make_unique<TypeNode>("int");
    TypeNode *typePtr = type.get();
    ast.nodes.push_back(std::move(type));
    auto decl = std::make_unique<VariableDeclNode>(name, typePtr, nullptr);
    Node *ptr = decl.get();
    ast.nodes.push_back(std::move(decl));
    return ptr;
  }
  throw std::runtime_error("Expected = or ;");
}

Node *ParseAssignment(AST &ast, std::vector<Token> &tokens, size_t &i) {
  std::string name = tokens[i].value;
  i++;

  if (tokens[i].type != ASSIGN) {
    throw std::runtime_error("Expected = in assignment");
  }
  i++;

  Node *value = ParseExpression(ast, tokens, i);

  if (tokens[i].type != SEMICOLON) {
    throw std::runtime_error("Expected ;");
  }
  i++;

  auto assignment = std::make_unique<AssignmentNode>(name, value);
  Node *ptr = assignment.get();
  ast.nodes.push_back(std::move(assignment));

  return ptr;
}

Node *ParseBlock(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++;

  auto block = std::make_unique<BlockNode>();
  BlockNode *blockPtr = block.get();
  ast.nodes.push_back(std::move(block));

  while (tokens[i].type != RBRACE) {
    if (tokens[i].type == EOF_TOKEN) {
      throw std::runtime_error("Expected }");
    }
    Node *statement = ParseStatement(ast, tokens, i);
    blockPtr->statements.push_back(statement);
    statement->parent = blockPtr;
  }
  i++;

  return blockPtr;
}

Node *ParseIf(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++; // consume if

  if (tokens[i].type != LPAREN) {
    throw std::runtime_error("Expected ( after if");
  }
  i++; // consume (

  Node *condition = ParseExpression(ast, tokens, i);

  if (tokens[i].type != RPAREN) {
    throw std::runtime_error("Expected ) after if condition");
  }
  i++; // consume )

  if (tokens[i].type != LBRACE) {
    throw std::runtime_error("Expected { after if condition");
  }

  Node *thenBlock = ParseBlock(ast, tokens, i);
  Node *elseBranch = nullptr;

  if (tokens[i].type == KW_ELSE) {
    i++; // consume else

    if (tokens[i].type == KW_IF) {
      elseBranch = ParseIf(ast, tokens, i);
    } else if (tokens[i].type == LBRACE) {
      elseBranch = ParseBlock(ast, tokens, i);
    } else {
      throw std::runtime_error("Expected if or { after else");
    }
  }

  auto ifNode = std::make_unique<IfNode>(condition, thenBlock, elseBranch);

  Node *ptr = ifNode.get();
  ast.nodes.push_back(std::move(ifNode));
  return ptr;
}

Node *ParseWhile(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++; // consume while
  if (tokens[i].type != LPAREN) {
    throw std::runtime_error("Expected (");
  }
  i++; // consume (

  Node *condition = ParseExpression(ast, tokens, i);

  if (tokens[i].type != RPAREN) {
    throw std::runtime_error("Expected )");
  }
  i++; // consume )

  Node *body = ParseBlock(ast, tokens, i);
  auto whileNode = std::make_unique<WhileNode>(condition, body);

  Node *ptr = whileNode.get();
  ast.nodes.push_back(std::move(whileNode));
  return ptr;
}

Node *ParsePostfixExpression(AST &ast, std::vector<Token> &tokens, size_t &i) {
  Node *operand = ParseSimpleExpression(ast, tokens, i);

  while (tokens[i].type == INCREMENT || tokens[i].type == DECREMENT) {
    std::string op = tokens[i].value;
    i++;

    auto postfix = std::make_unique<PostfixExpressionNode>(operand, op);

    operand = postfix.get();
    ast.nodes.push_back(std::move(postfix));
  }

  return operand;
}

Node *ParseAssignmentWithoutSemicolon(AST &ast, std::vector<Token> &tokens,
                                      size_t &i) {
  std::string name = tokens[i].value;
  i++;

  if (tokens[i].type != ASSIGN) {
    throw std::runtime_error("Expected = in assignment");
  }
  i++;

  Node *value = ParseExpression(ast, tokens, i);

  auto assignment = std::make_unique<AssignmentNode>(name, value);
  Node *ptr = assignment.get();
  ast.nodes.push_back(std::move(assignment));
  return ptr;
}

Node *ParseFor(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++; // consume for

  if (tokens[i].type != LPAREN) {
    throw std::runtime_error("Expected ( after for");
  }
  i++; // consume (

  Node *initialization = nullptr;

  if (tokens[i].type == KW_INT) {
    initialization = ParseIntDeclaration(ast, tokens, i);
  } else if (tokens[i].type == IDENTIFIER && tokens[i + 1].type == ASSIGN) {
    initialization = ParseAssignment(ast, tokens, i);
  } else if (tokens[i].type == SEMICOLON) {
    i++;
  } else {
    throw std::runtime_error("Expected for-loop initialization");
  }

  Node *condition = nullptr;

  if (tokens[i].type != SEMICOLON) {
    condition = ParseExpression(ast, tokens, i);
  }

  if (tokens[i].type != SEMICOLON) {
    throw std::runtime_error("Expected ; after for-loop condition");
  }
  i++;

  Node *increment = nullptr;

  if (tokens[i].type != RPAREN) {
    if (tokens[i].type == IDENTIFIER && tokens[i + 1].type == ASSIGN) {
      increment = ParseAssignmentWithoutSemicolon(ast, tokens, i);
    } else {
      increment = ParseExpression(ast, tokens, i);
    }
  }
  if (tokens[i].type != RPAREN) {
    throw std::runtime_error("Expected ) after for-loop clauses");
  }
  i++;

  Node *body = ParseBlock(ast, tokens, i);

  auto forNode =
      std::make_unique<ForNode>(initialization, condition, increment, body);

  Node *ptr = forNode.get();
  ast.nodes.push_back(std::move(forNode));
  return ptr;
}

Node *ParseBreak(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++; // consume break
  if (tokens[i].type != SEMICOLON) {
    throw std::runtime_error("Expected ; after break");
  }
  i++;
  auto breakNode = std::make_unique<BreakNode>();
  Node *ptr = breakNode.get();
  ast.nodes.push_back(std::move(breakNode));
  return ptr;
}

Node *ParseContinue(AST &ast, std::vector<Token> &tokens, size_t &i) {
  i++; // consume continue
  if (tokens[i].type != SEMICOLON) {
    throw std::runtime_error("Expected ; after continue");
  }
  i++;
  auto continueNode = std::make_unique<ContinueNode>();
  Node *ptr = continueNode.get();
  ast.nodes.push_back(std::move(continueNode));
  return ptr;
}

Node *ParseFunctionDecl(AST &ast, std::vector<Token> &tokens, size_t &i) {
  TypeNode *returnType = ParseType(ast, tokens, i);

  if (tokens[i].type != IDENTIFIER) {
    throw std::runtime_error("Expected function name");
  }

  std::string name = tokens[i].value;
  i++; // consume function name

  if (tokens[i].type != LPAREN) {
    throw std::runtime_error("Expected ( after function name");
  }
  i++; // consume (

  std::vector<ParameterNode *> parameters = ParseParameterList(ast, tokens, i);

  if (tokens[i].type != RPAREN) {
    throw std::runtime_error("Expected ) after function parameters");
  }
  i++; // consume )

  Node *bodyNode = ParseBlock(ast, tokens, i);
  auto *body = static_cast<BlockNode *>(bodyNode);

  auto functionDecl =
      std::make_unique<FunctionDeclNode>(name, returnType, parameters, body);

  Node *ptr = functionDecl.get();
  ast.nodes.push_back(std::move(functionDecl));
  return ptr;
}

TypeNode *ParseType(AST &ast, std::vector<Token> &tokens, size_t &i) {
  if (tokens[i].type == KW_INT) {
    auto type = std::make_unique<TypeNode>("int");
    TypeNode *typePtr = type.get();
    ast.nodes.push_back(std::move(type));
    i++; // consume int
    return typePtr;
  }

  if (tokens[i].type == KW_VOID) {
    auto type = std::make_unique<TypeNode>("void");
    TypeNode *typePtr = type.get();
    ast.nodes.push_back(std::move(type));
    i++; // consume void
    return typePtr;
  }

  throw std::runtime_error("Expected int or void");
}

std::vector<ParameterNode *>
ParseParameterList(AST &ast, std::vector<Token> &tokens, size_t &i) {
  std::vector<ParameterNode *> parameters;

  if (tokens[i].type == RPAREN) {
    return parameters;
  }

  while (true) {
    ParameterNode *parameter = ParseParameter(ast, tokens, i);
    parameters.push_back(parameter);

    if (tokens[i].type == COMMA) {
      i++; // consume ,
      continue;
    }

    break;
  }

  return parameters;
}

ParameterNode *ParseParameter(AST &ast, std::vector<Token> &tokens, size_t &i) {
  TypeNode *type = ParseType(ast, tokens, i);

  if (tokens[i].type != IDENTIFIER) {
    throw std::runtime_error("Expected parameter name");
  }

  std::string name = tokens[i].value;
  i++; // consume parameter name

  auto parameter = std::make_unique<ParameterNode>(type, name);
  ParameterNode *ptr = parameter.get();
  ast.nodes.push_back(std::move(parameter));
  return ptr;
}

Node *ParseStatement(AST &ast, std::vector<Token> &tokens, size_t &i) {
  if (tokens[i].type == KW_RETURN) {
    return ParseReturn(ast, tokens, i);
  }

  if (tokens[i].type == KW_INT ||
      tokens[i].type ==
          KW_VOID) { // This will allow void a = 5 type declarations
    // TODO: fix

    if (tokens[i + 1].type == IDENTIFIER && tokens[i + 2].type == LPAREN) {
      return ParseFunctionDecl(ast, tokens, i);
    }

    if (tokens[i].type == KW_VOID){
      throw std::runtime_error("Cannot initialize a variable of type 'void'");
    }
    //
    return ParseIntDeclaration(ast, tokens, i);
  }

  if (tokens[i].type == IDENTIFIER) {
    if (tokens[i + 1].type == ASSIGN) {
      return ParseAssignment(ast, tokens, i);
    }

    Node *expr = ParseExpression(ast, tokens, i);

    if (tokens[i].type != SEMICOLON) {
      throw std::runtime_error("Expected ; after expression statement");
    }
    i++;

    auto exprStmt = std::make_unique<ExpressionStatementNode>(expr);
    Node *ptr = exprStmt.get();
    ast.nodes.push_back(std::move(exprStmt));
    return ptr;
  }

  if (tokens[i].type == LBRACE) {
    return ParseBlock(ast, tokens, i);
  }

  if (tokens[i].type == KW_IF) {
    return ParseIf(ast, tokens, i);
  }

  if (tokens[i].type == KW_WHILE) {
    return ParseWhile(ast, tokens, i);
  }

  if (tokens[i].type == KW_FOR) {
    return ParseFor(ast, tokens, i);
  }

  if (tokens[i].type == KW_BREAK) {
    return ParseBreak(ast, tokens, i);
  }
  if (tokens[i].type == KW_CONTINUE) {
    return ParseContinue(ast, tokens, i);
  }

  throw std::runtime_error("Unexpected token: " + tokens[i].value);
}

AST Parse(std::vector<Token> tokens) {
  AST ast;

  ast.nodes.push_back(std::make_unique<ProgramNode>());
  ast.root = static_cast<ProgramNode *>(ast.nodes.back().get());

  size_t i = 0;

  while (tokens[i].type != EOF_TOKEN) {
    Node *statement = ParseStatement(ast, tokens, i);
    ast.root->statements.push_back(statement);
  }

  return ast;
}
