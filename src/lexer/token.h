#pragma once
#include <string>
#include "util/diag.h"

enum class TokenKind {
  End,
  Ident,
  IntLit,
  RealLit,
  TextLit,

  // keywords
  KwModule,
  KwGroup,
  KwProcess,
  KwState,
  KwOn,
  KwDo,
  KwFn,
  KwType,
  KwLet,
  KwVar,
  KwReturn,
  KwChannel,
  KwSchedule,
  KwStep,
  KwRepeat,
  KwIf,
  KwElse,
  KwSend,
  KwReceive,
  KwTrySend,
  KwTryReceive,
  KwImport,

  // punctuation / operators
  LParen, RParen,
  LBrace, RBrace,
  LBracket, RBracket,
  Comma, Colon, Semi,
  Dot,
  Arrow,     // ->
  Assign,    // =
  At,        // @
  Question,  // ?

  Plus, Minus, Star, Slash,
  EqEq, NotEq,
  Lt, Lte, Gt, Gte,
  AndAnd, OrOr,
};

struct Token {
  TokenKind kind = TokenKind::End;
  std::string text;
  SourcePos pos;
};
