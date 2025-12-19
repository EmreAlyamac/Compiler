#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

const std::unordered_set<std::string> keywords = { //
    "if", "else", "while", "for", "return", "int", "void"};

const std::unordered_set<std::string> operators = {
    "+",  "-",  "*",  "/",  "=",  "==", "!=", "<",  ">",  "<=", ">=",
    "++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^="};

const std::unordered_set<std::string> parentheses = {"(", ")", "{", "}",
                                                     "[", "]", ",", "."};

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

const std::unordered_map<std::string, TokenType> TokenTypeMap = {
    // Keywords
    {"if", KW_IF},
    {"else", KW_ELSE},
    {"while", KW_WHILE},
    {"for", KW_FOR},
    {"return", KW_RETURN},
    {"int", KW_INT},
    {"void", KW_VOID},

    // Simple Operators
    {"+", PLUS},
    {"-", MINUS},
    {"*", MULTIPLY},
    {"/", DIVIDE},
    {"=", ASSIGN},
    {"==", EQUAL},
    {"!=", NOT_EQUAL},
    {"<", LESS},
    {">", GREATER},
    {"<=", LESS_EQUAL},
    {">=", GREATER_EQUAL},
    {"++", INCREMENT},
    {"--", DECREMENT},

    {"+=", PLUS_ASSIGN},
    {"-=", MINUS_ASSIGN},
    {"*=", MUL_ASSIGN},
    {"/=", DIV_ASSIGN},
    {"%=", MOD_ASSIGN},
    {"&=", AND_ASSIGN},
    {"|=", OR_ASSIGN},
    {"^=", XOR_ASSIGN},

    {"(", LPAREN},
    {")", RPAREN},
    {"{", LBRACE},
    {"}", RBRACE},
    {"[", LBRACKET},
    {"]", RBRACKET},
    {";", SEMICOLON},
    {",", COMMA},
    {".", DOT},

    // Special
    {"EOF", EOF_TOKEN},
    {"UNKNOWN", UNKNOWN}};

bool IsIdentifier(std::string Word);
bool IsInteger(std::string Word);
bool IsOperator(std::string Word);
bool IsParenthesis(std::string Word);
bool IsKeyword(std::string word);

struct Token {
  TokenType type;
  std::string value;
};

Token GetToken(std::string Word) {
  try {
    if (IsKeyword(Word)) {
      return Token{TokenTypeMap.at(Word), Word};
    } else if (IsIdentifier(Word)) {
      return Token{IDENTIFIER, Word};
    } else if (IsInteger(Word)) {
      return Token{INTEGER, Word};
    } else if (IsOperator(Word)) {
      return Token{TokenTypeMap.at(Word), Word};
    } else if (IsParenthesis(Word)) {
      return Token{TokenTypeMap.at(Word), Word};
    } else {
      return Token{TokenTypeMap.at(Word), Word};
    }
  } catch (const std::out_of_range &e) {
    std::cerr << "Error: Token not found: " << Word << std::endl;
    return Token{UNKNOWN, Word};
  }
}

bool IsIdentifier(std::string Word) {

  if (!isalpha(Word[0]) && Word[0] != '_') {
    return false;
  }
  for (std::string::size_type i = 1; i < Word.length(); i++) {
    if (!isalnum(Word[i]) && Word[i] != '_') {
      return false;
    }
  }
  return true;
}

bool IsInteger(std::string Word) {
  for (std::string::size_type i = 0; i < Word.length(); i++) {
    if (!isdigit(Word[i])) {
      return false;
    }
  }
  return true;
}

bool IsOperator(std::string word) { // TODO make it use const&
  return operators.find(word) != operators.end();
}

bool IsParenthesis(std::string word) { // TODO make it use const&
  return parentheses.find(word) != parentheses.end();
}

bool IsKeyword(std::string word) {
  return keywords.find(word) != keywords.end();
}

std::vector<Token> Tokenize(std::string &SourceCode) {
  std::vector<Token> tokens;
  std::string::size_type index = 0;
  std::string word;
  while (index < SourceCode.length()) {
    if (isspace(SourceCode[index])) {
      if (!word.empty()) {
        Token token = GetToken(word);
        tokens.push_back(token);
        word.clear();
      }
      index++;
      continue;
    }

    else if (IsParenthesis((std::string){SourceCode[index]})) {
      if (!word.empty()) {
        Token token = GetToken(word);
        tokens.push_back(token);
        word.clear();
      }
      word.push_back(SourceCode[index]);
      Token token = GetToken(word);
      tokens.push_back(token);
      word.clear();
      index++;
      continue;
    } else if (IsOperator((std::string){SourceCode[index]})) {
      if (!word.empty()) {
        Token token = GetToken(word);
        tokens.push_back(token);
        word.clear();
      }
      word.push_back(SourceCode[index]);

      if (index + 1 < SourceCode.length() &&
          IsOperator((std::string){SourceCode[index + 1]})) {
        std::string temp =
            std::string(1, static_cast<char>(SourceCode[index])) +
            std::string(1, static_cast<char>(SourceCode[index + 1]));
        if (IsOperator(temp)) {
          word = temp;
          index += 1;
        }
      }
      Token token = GetToken(word);
      tokens.push_back(token);
      word.clear();
      index++;
      continue;

    } else if (SourceCode[index] == ';') {
      if (!word.empty()) {
        Token token = GetToken(word);
        tokens.push_back(token);
        word.clear();
      }
      word.push_back(';');
      Token token = GetToken(word);
      tokens.push_back(token);
      word.clear();
      index++;
      continue;
    } else {
      word.push_back(SourceCode[index]);
      index++;
    }
  }
  if (!word.empty()) {
    Token token = GetToken(word);
    tokens.push_back(token);
    word.clear();
  }
  tokens.push_back(Token{EOF_TOKEN, ""});
  return tokens;
}
