#pragma once
#include "lexer/lexer.h"
#include "ast/ast.h"
#include "util/diag.h"

struct Parser {
  Parser(Lexer& lx, Diag& d);

  Program parse_program();

private:
  Lexer& lex;
  Diag& diag;

  Token t();
  Token eat();
  bool is(TokenKind k);
  bool accept(TokenKind k);
  Token expect(TokenKind k, const char* msg);

  // annotations: @name or @name(...)
  std::vector<Annotation> parse_annotations();

  Program parse_program_inner();
  ModuleDecl parse_module();

  GroupDecl parse_group();
  ChannelDecl parse_channel_decl();

  ProcessDecl parse_process();
  std::vector<Param> parse_param_list();
  Param parse_param();

  TypeRef parse_type_ref();
  int parse_int_literal_value(const Token& tok);

  Stmt parse_stmt();
  Action parse_action();
  Transition parse_transition();

  OnBlock parse_on_block();

  ScheduleDecl parse_schedule();

  // Expressions: Pratt
  Expr parse_expr(int min_bp = 0);
  Expr parse_primary();
  int infix_binding_power(TokenKind op, bool& right_assoc) const;
};
