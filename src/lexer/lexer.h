#pragma once
#include <string>
#include <unordered_map>
#include "lexer/token.h"
#include "util/diag.h"

struct Lexer {
  Lexer(const std::string& src, Diag& d);

  Token next();
  Token peek();
  void consume();

private:
  Diag& diag;
  std::string s;
  size_t i = 0;
  int line = 1;
  int col = 1;

  Token look;
  bool has_look = false;

  char cur() const;
  char nxt() const;
  bool eof() const;

  void adv();
  void skip_ws_and_comments();

  Token make(TokenKind k, const std::string& t, SourcePos p) const;

  Token lex_number();
  Token lex_ident_or_kw();
  Token lex_text();

  std::unordered_map<std::string, TokenKind> kw;
};
