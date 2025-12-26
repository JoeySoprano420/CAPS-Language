#include "lexer/lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& src, Diag& d) : diag(d), s(src) {
  kw = {
    {"module", TokenKind::KwModule},
    {"group", TokenKind::KwGroup},
    {"process", TokenKind::KwProcess},
    {"state", TokenKind::KwState},
    {"on", TokenKind::KwOn},
    {"do", TokenKind::KwDo},
    {"fn", TokenKind::KwFn},
    {"type", TokenKind::KwType},
    {"let", TokenKind::KwLet},
    {"var", TokenKind::KwVar},
    {"return", TokenKind::KwReturn},
    {"channel", TokenKind::KwChannel},
    {"schedule", TokenKind::KwSchedule},
    {"step", TokenKind::KwStep},
    {"repeat", TokenKind::KwRepeat},
    {"if", TokenKind::KwIf},
    {"else", TokenKind::KwElse},
    {"send", TokenKind::KwSend},
    {"receive", TokenKind::KwReceive},
    {"try_send", TokenKind::KwTrySend},
    {"try_receive", TokenKind::KwTryReceive},
    {"import", TokenKind::KwImport},
  };
}

char Lexer::cur() const { return i < s.size() ? s[i] : '\0'; }
char Lexer::nxt() const { return (i + 1) < s.size() ? s[i + 1] : '\0'; }
bool Lexer::eof() const { return i >= s.size(); }

void Lexer::adv() {
  if (eof()) return;
  if (s[i] == '\n') { line++; col = 1; }
  else col++;
  i++;
}

Token Lexer::make(TokenKind k, const std::string& t, SourcePos p) const {
  Token tok;
  tok.kind = k;
  tok.text = t;
  tok.pos = p;
  return tok;
}

void Lexer::skip_ws_and_comments() {
  while (!eof()) {
    if (std::isspace((unsigned char)cur())) { adv(); continue; }
    // line comment //
    if (cur() == '/' && nxt() == '/') {
      while (!eof() && cur() != '\n') adv();
      continue;
    }
    break;
  }
}

Token Lexer::lex_text() {
  SourcePos p{line, col};
  adv(); // "
  std::string out;
  while (!eof() && cur() != '"') {
    if (cur() == '\\') {
      adv();
      if (eof()) break;
      char c = cur();
      if (c == 'n') out.push_back('\n');
      else if (c == 't') out.push_back('\t');
      else out.push_back(c);
      adv();
    } else {
      out.push_back(cur());
      adv();
    }
  }
  if (cur() != '"') diag.error(p, "unterminated string literal");
  else adv();
  return make(TokenKind::TextLit, out, p);
}

Token Lexer::lex_number() {
  SourcePos p{line, col};
  std::string out;
  bool is_real = false;
  while (std::isdigit((unsigned char)cur())) { out.push_back(cur()); adv(); }
  if (cur() == '.' && std::isdigit((unsigned char)nxt())) {
    is_real = true;
    out.push_back(cur()); adv();
    while (std::isdigit((unsigned char)cur())) { out.push_back(cur()); adv(); }
  }
  return make(is_real ? TokenKind::RealLit : TokenKind::IntLit, out, p);
}

Token Lexer::lex_ident_or_kw() {
  SourcePos p{line, col};
  std::string out;
  while (std::isalnum((unsigned char)cur()) || cur() == '_') { out.push_back(cur()); adv(); }
  auto it = kw.find(out);
  if (it != kw.end()) return make(it->second, out, p);
  return make(TokenKind::Ident, out, p);
}

Token Lexer::next() {
  skip_ws_and_comments();
  SourcePos p{line, col};
  if (eof()) return make(TokenKind::End, "", p);

  char c = cur();

  // strings
  if (c == '"') return lex_text();

  // numbers
  if (std::isdigit((unsigned char)c)) return lex_number();

  // ident/kw
  if (std::isalpha((unsigned char)c) || c == '_') return lex_ident_or_kw();

  // multi-char ops
  if (c == '-' && nxt() == '>') { adv(); adv(); return make(TokenKind::Arrow, "->", p); }
  if (c == '=' && nxt() == '=') { adv(); adv(); return make(TokenKind::EqEq, "==", p); }
  if (c == '!' && nxt() == '=') { adv(); adv(); return make(TokenKind::NotEq, "!=", p); }
  if (c == '<' && nxt() == '=') { adv(); adv(); return make(TokenKind::Lte, "<=", p); }
  if (c == '>' && nxt() == '=') { adv(); adv(); return make(TokenKind::Gte, ">=", p); }
  if (c == '&' && nxt() == '&') { adv(); adv(); return make(TokenKind::AndAnd, "&&", p); }
  if (c == '|' && nxt() == '|') { adv(); adv(); return make(TokenKind::OrOr, "||", p); }

  // single-char
  switch (c) {
    case '(': adv(); return make(TokenKind::LParen, "(", p);
    case ')': adv(); return make(TokenKind::RParen, ")", p);
    case '{': adv(); return make(TokenKind::LBrace, "{", p);
    case '}': adv(); return make(TokenKind::RBrace, "}", p);
    case '[': adv(); return make(TokenKind::LBracket, "[", p);
    case ']': adv(); return make(TokenKind::RBracket, "]", p);
    case ',': adv(); return make(TokenKind::Comma, ",", p);
    case ':': adv(); return make(TokenKind::Colon, ":", p);
    case ';': adv(); return make(TokenKind::Semi, ";", p);
    case '.': adv(); return make(TokenKind::Dot, ".", p);
    case '=': adv(); return make(TokenKind::Assign, "=", p);
    case '+': adv(); return make(TokenKind::Plus, "+", p);
    case '-': adv(); return make(TokenKind::Minus, "-", p);
    case '*': adv(); return make(TokenKind::Star, "*", p);
    case '/': adv(); return make(TokenKind::Slash, "/", p);
    case '<': adv(); return make(TokenKind::Lt, "<", p);
    case '>': adv(); return make(TokenKind::Gt, ">", p);
    case '@': adv(); return make(TokenKind::At, "@", p);
    case '?': adv(); return make(TokenKind::Question, "?", p);
    default:
      diag.error(p, std::string("unexpected character: '") + c + "'");
      adv();
      return make(TokenKind::End, "", p);
  }
}

Token Lexer::peek() {
  if (!has_look) { look = next(); has_look = true; }
  return look;
}

void Lexer::consume() {
  if (!has_look) { (void)next(); return; }
  has_look = false;
}
