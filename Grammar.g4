// CAPS.g4
// CAPS — Computational Algorithmic Process Sequence Language
// Full ANTLR4 grammar (EBNF-style) for the current CAPS surface syntax.
//
// Notes:
// - This grammar ENFORCES total transitions at syntax level:
//     * "if <expr> { ... -> S; } else { ... -> T; }"
//   No "if <expr> -> S" without else is allowed.
// - "receive c -> var x:T;" sugar is supported.
// - Channel decl syntax matches:  channel c : channel<int; 1>;
// - Result unwrap "?" is parsed as postfix.
// - Most semantic constraints (typing, scoping, topology, realtime rules) are enforced in later passes.

grammar CAPS;

// =========================
// Parser rules
// =========================

program
  : groupDecl+ EOF
  ;

groupDecl
  : annotation* KW_GROUP Identifier LBRACE groupItem* RBRACE
  ;

groupItem
  : channelDecl
  | processDecl
  | scheduleDecl
  ;

annotation
  : AT Identifier                              #AnnBare
  | AT Identifier LPAREN annArgList? RPAREN     #AnnCall
  ;

annArgList
  : annArg (COMMA annArg)*
  ;

annArg
  : Identifier ASSIGN expr
  ;

// ---------- Channels ----------

channelDecl
  : KW_CHANNEL Identifier COLON KW_CHANNEL LT type SEMI IntLiteral GT SEMI
  ;

// ---------- Processes ----------

processDecl
  : annotation* KW_PROCESS Identifier LPAREN paramList? RPAREN ARROW LPAREN resultList? RPAREN
    LBRACE processItem* RBRACE
  ;

paramList
  : param (COMMA param)*
  ;

param
  : Identifier COLON type
  ;

resultList
  : resultParam (COMMA resultParam)*
  ;

resultParam
  : Identifier COLON type
  ;

processItem
  : stateDecl
  | varDecl
  | onBlock
  ;

stateDecl
  : KW_STATE Identifier SEMI
  ;

varDecl
  : KW_VAR Identifier COLON type SEMI
  | KW_LET Identifier COLON type ASSIGN expr SEMI
  ;

// ---------- On blocks / actions / transitions ----------

onBlock
  : KW_ON Identifier LBRACE action* transition RBRACE
  ;

action
  : sendStmt
  | receiveStmt
  | trySendStmt
  | tryReceiveStmt
  | assignStmt
  ;

sendStmt
  : KW_SEND expr ARROW Identifier SEMI
  ;

// receive c -> x;
receiveStmt
  : KW_RECEIVE Identifier ARROW Identifier SEMI
  // sugar: receive c -> var x:T;
  | KW_RECEIVE Identifier ARROW KW_VAR Identifier COLON type SEMI
  ;

trySendStmt
  : KW_TRY_SEND expr ARROW Identifier ARROW Identifier SEMI
  ;

tryReceiveStmt
  : KW_TRY_RECEIVE Identifier ARROW Identifier SEMI
  ;

assignStmt
  : Identifier ASSIGN expr SEMI
  ;

// Transitions are STRONG: either goto, or total if/else with branch blocks.
transition
  : gotoTransition
  | ifElseTransition
  ;

gotoTransition
  : ARROW Identifier SEMI
  ;

ifElseTransition
  : KW_IF expr branchBlock KW_ELSE branchBlock
  ;

branchBlock
  : LBRACE action* ARROW Identifier SEMI RBRACE
  ;

// ---------- Schedule ----------

scheduleDecl
  : KW_SCHEDULE LBRACE scheduleItem+ RBRACE
  ;

scheduleItem
  : KW_STEP IntLiteral COLON Identifier SEMI
  | KW_REPEAT SEMI
  ;

// ---------- Types ----------

type
  : primitiveType
  | resultType
  | Identifier            // user-defined / nominal types (future-proof)
  ;

primitiveType
  : KW_INT
  | KW_BOOL
  | KW_REAL
  | KW_TEXT
  ;

resultType
  : KW_RESULT LT type COMMA type GT
  ;

// ---------- Expressions (precedence climbing) ----------

expr
  : exprOr
  ;

exprOr
  : exprAnd (OR exprAnd)*
  ;

exprAnd
  : exprEq (AND exprEq)*
  ;

exprEq
  : exprRel ((EQ | NEQ) exprRel)*
  ;

exprRel
  : exprAdd ((LT_ | GT_ | LE | GE) exprAdd)*
  ;

exprAdd
  : exprMul ((PLUS | MINUS) exprMul)*
  ;

exprMul
  : exprUnary ((STAR | SLASH) exprUnary)*
  ;

// unary binds tighter than postfix
exprUnary
  : (BANG | MINUS) exprUnary
  | exprPostfix
  ;

// postfix includes calls, indexing (future), and Result unwrap '?'
exprPostfix
  : exprPrimary postfix*
  ;

postfix
  : callSuffix
  | unwrapSuffix
  ;

callSuffix
  : LPAREN argList? RPAREN
  ;

argList
  : expr (COMMA expr)*
  ;

unwrapSuffix
  : QMARK
  ;

exprPrimary
  : literal
  | Identifier
  | LPAREN expr RPAREN
  ;

literal
  : IntLiteral
  | BoolLiteral
  | TextLiteral
  ;

// =========================
// Lexer rules
// =========================

// Keywords
KW_GROUP       : 'group' ;
KW_PROCESS     : 'process' ;
KW_STATE       : 'state' ;
KW_ON          : 'on' ;
KW_SCHEDULE    : 'schedule' ;
KW_STEP        : 'step' ;
KW_REPEAT      : 'repeat' ;

KW_CHANNEL     : 'channel' ;
KW_SEND        : 'send' ;
KW_RECEIVE     : 'receive' ;
KW_TRY_SEND    : 'try_send' ;
KW_TRY_RECEIVE : 'try_receive' ;

KW_VAR         : 'var' ;
KW_LET         : 'let' ;
KW_IF          : 'if' ;
KW_ELSE        : 'else' ;

KW_RESULT      : 'Result' ;

KW_INT         : 'int' ;
KW_BOOL        : 'bool' ;
KW_REAL        : 'real' ;
KW_TEXT        : 'text' ;

// Operators / punctuation
ARROW   : '->' ;
ASSIGN  : '=' ;
EQ      : '==' ;
NEQ     : '!=' ;
LE      : '<=' ;
GE      : '>=' ;
LT_     : '<' ;
GT_     : '>' ;

PLUS    : '+' ;
MINUS   : '-' ;
STAR    : '*' ;
SLASH   : '/' ;
BANG    : '!' ;

AND     : '&&' ;
OR      : '||' ;

AT      : '@' ;
QMARK   : '?' ;

LPAREN  : '(' ;
RPAREN  : ')' ;
LBRACE  : '{' ;
RBRACE  : '}' ;
LT      : '<' ;
GT      : '>' ;
SEMI    : ';' ;
COLON   : ':' ;
COMMA   : ',' ;

// Literals
BoolLiteral
  : 'true'
  | 'false'
  ;

IntLiteral
  : Digit+
  ;

// Text: double-quoted with escapes \" \\ \n \t \r
TextLiteral
  : '"' ( EscapeSeq | ~["\\\r\n] )* '"'
  ;

fragment EscapeSeq
  : '\\' ( ["\\/bfnrt] )
  ;

// Identifier
Identifier
  : Letter (Letter | Digit | '_')*
  ;

// Fragments
fragment Letter : [A-Za-z] ;
fragment Digit  : [0-9] ;

// Comments / whitespace
LineComment
  : '//' ~[\r\n]* -> skip
  ;

WS
  : [ \t\r\n]+ -> skip
  ;
