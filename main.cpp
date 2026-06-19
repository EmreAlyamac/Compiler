#include "lexer.h"
#include "parser.h"
#include "semantic.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string WriteToken(Token token) {
  switch (token.type) {
  case TokenType::KW_BREAK:
    return "Keyword: break";
  case TokenType::KW_CONTINUE:
    return "Keyword: continue";
  case TokenType::KW_IF:
    return "Keyword: if";
  case TokenType::KW_ELSE:
    return "Keyword: else";
  case TokenType::KW_WHILE:
    return "Keyword: while";
  case TokenType::KW_FOR:
    return "Keyword: for";
  case TokenType::KW_RETURN:
    return "Keyword: return";
  case TokenType::KW_INT:
    return "Keyword: int";
  case TokenType::KW_VOID:
    return "Keyword: void";
  case TokenType::IDENTIFIER:
    return "Identifier: " + token.value;
  case TokenType::INTEGER:
    return "Integer: " + token.value;

  case TokenType::PLUS:
    return "Operator: +";
  case TokenType::MINUS:
    return "Operator: -";
  case TokenType::MULTIPLY:
    return "Operator: *";
  case TokenType::DIVIDE:
    return "Operator: /";
  case TokenType::ASSIGN:
    return "Operator: =";
  case TokenType::EQUAL:
    return "Operator: ==";
  case TokenType::NOT_EQUAL:
    return "Operator: !=";
  case TokenType::LESS:
    return "Operator: <";
  case TokenType::GREATER:
    return "Operator: >";
  case TokenType::LESS_EQUAL:
    return "Operator: <=";
  case TokenType::GREATER_EQUAL:
    return "Operator: >=";
  case TokenType::INCREMENT:
    return "Operator: ++";
  case TokenType::DECREMENT:
    return "Operator: --";

  case TokenType::PLUS_ASSIGN:
    return "Operator: +=";
  case TokenType::MINUS_ASSIGN:
    return "Operator: -=";
  case TokenType::MUL_ASSIGN:
    return "Operator: *=";
  case TokenType::DIV_ASSIGN:
    return "Operator: /=";
  case TokenType::MOD_ASSIGN:
    return "Operator: %=";
  case TokenType::AND_ASSIGN:
    return "Operator: &=";
  case TokenType::OR_ASSIGN:
    return "Operator: |=";
  case TokenType::XOR_ASSIGN:
    return "Operator: ^=";
  case TokenType::RSHIFT_ASSIGN:
    return "Operator: >>=";
  case TokenType::LSHIFT_ASSIGN:
    return "Operator: <<=";

  case TokenType::LPAREN:
    return "Punctuation: (";
  case TokenType::RPAREN:
    return "Punctuation: )";
  case TokenType::LBRACE:
    return "Punctuation: {";
  case TokenType::RBRACE:
    return "Punctuation: }";
  case TokenType::LBRACKET:
    return "Punctuation: [";
  case TokenType::RBRACKET:
    return "Punctuation: ]";
  case TokenType::SEMICOLON:
    return "Punctuation: ;";
  case TokenType::COMMA:
    return "Punctuation: ,";
  case TokenType::DOT:
    return "Punctuation: .";

  case TokenType::EOF_TOKEN:
    return "End of File";
  case TokenType::UNKNOWN:
    return "Unknown Token: " + token.value;
  }
  return token.value;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
    return 1;
  }

  std::vector<Token> tokens;
  AST ast;

  try {
    std::ifstream file(argv[1]);
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    tokens = Tokenize(source);

    ast = Parse(tokens);
    SemanticAnalyzer analyzer;
    analyzer.Analyze(ast);

  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
#ifdef LEXER_TEST
  std::cout << "Tokens:" << std::endl;
  for (const auto &token : tokens) {
    std::cout << WriteToken(token) << std::endl;
  }
#endif
#ifdef PARSER_TEST

  std::cout << "AST:" << std::endl;
  std::cout << ast.root->to_string() << std::endl;
#endif
#ifdef SEMANTIC_TEST

#endif

  return 0;
}
