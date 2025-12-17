#pragma once

#include <string>
#include <vector>

enum TokenType {
  KW_IF,
  KW_ELSE,
  KW_WHILE,
  KW_FOR,
  KW_RETURN,
  KW_INT,
  KW_VOID,
  IDENTIFIER,
  INTEGER,
  LESS,
  GREATER,
  LESS_EQUAL,
  GREATER_EQUAL,
  INCREMENT,
  DECREMENT,
  PLUS_ASSIGN,
  MINUS_ASSIGN,
  MUL_ASSIGN,
  DIV_ASSIGN,
  MOD_ASSIGN,
  AND_ASSIGN,
  OR_ASSIGN,
  XOR_ASSIGN,
  RSHIFT_ASSIGN,
  LSHIFT_ASSIGN,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET,
  PLUS,
  MINUS,
  MULTIPLY,
  EQUAL,
  ASSIGN,
  NOT_EQUAL,
  DIVIDE,
  SEMICOLON,
  COMMA,
  DOT,
  EOF_TOKEN,
  UNKNOWN,
};

struct Token {
  TokenType type;
  std::string value;
};

bool IsIdentifier(const std::string &word);
bool IsInteger(const std::string &word);
bool IsOperator(const std::string &word);
bool IsParenthesis(const std::string &word);
bool IsKeyword(const std::string &word);

Token GetToken(const std::string &word);

std::vector<Token> Tokenize(std::string &sourceCode);