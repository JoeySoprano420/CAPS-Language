Below is a complete, self-contained CAPS syntax specification, written as a formal EBNF-style grammar plus semantic notes where grammar alone cannot express the rule (because CAPS is intentionally strict).

This is suitable to be:

the authoritative language reference

the basis for an ANTLR / recursive-descent parser

a SYNTAX.md file in your GitHub repo


I will not hand-wave. This is the entire language as it exists now, including all the upgrades you locked in.


---

CAPS Language Syntax Specification

> CAPS — Causal Actor / Process System



This document defines the surface syntax of CAPS.
Semantic constraints (type rules, topology checks, realtime rules) are listed separately where needed.


---

1. Lexical Structure

1.1 Identifiers

Identifier ::= Letter ( Letter | Digit | "_" )*
Letter     ::= "A"…"Z" | "a"…"z"
Digit      ::= "0"…"9"

Identifiers are case-sensitive.


---

1.2 Keywords (reserved)

group
process
state
on
schedule
step
repeat

channel
send
receive
try_send
try_receive

var
let
if
else
true
false
Result

int
bool
text
real

These identifiers may not be used as user symbols.


---

1.3 Literals

IntLiteral    ::= Digit+
BoolLiteral   ::= "true" | "false"
TextLiteral   ::= '"' ( any-char-except-quote | '\"' )* '"'


---

1.4 Comments

LineComment ::= "//" .* newline

(No block comments by design — CAPS favors clarity over cleverness.)


---

2. Top-Level Structure

Program ::= GroupDecl+

A CAPS program is one or more groups.


---

3. Annotations

Annotations attach to groups and processes.

Annotation ::= "@" Identifier
             | "@" Identifier "(" AnnotationArgs? ")"

AnnotationArgs ::= AnnotationArg ( "," AnnotationArg )*
AnnotationArg  ::= Identifier "=" Expr

Examples:

@pipeline_safe
@realtimesafe(maxtick_cost = 500)
@max_sends(c = 10)

Annotations are parsed and preserved in the AST and enforced during semantic analysis.


---

4. Group Declaration

GroupDecl ::=
    Annotation*
    "group" Identifier "{"
        GroupItem*
    "}"

Group items

GroupItem ::=
      ChannelDecl
    | ProcessDecl
    | ScheduleDecl


---

5. Channel Declaration

ChannelDecl ::=
    "channel" Identifier ":" "channel" "<" Type ";" IntLiteral ">" ";"

Example:

channel c : channel<int; 1>;

Semantic rules (not syntax):

Channels are visible only to processes in the same group.

Capacity must be > 0.

Channel types are invariant.



---

6. Process Declaration

ProcessDecl ::=
    Annotation*
    "process" Identifier "(" ParamList? ")" "->" "(" ResultList? ")" "{"
        ProcessItem*
    "}"

Parameters and results

ParamList  ::= Param ( "," Param )*
Param      ::= Identifier ":" Type

ResultList ::= ResultParam ( "," ResultParam )*
ResultParam ::= Identifier ":" Type


---

Process items

ProcessItem ::=
      StateDecl
    | VarDecl
    | OnBlock


---

7. State Declaration

StateDecl ::= "state" Identifier ";"

States are symbols, not values.


---

8. Variable Declarations

VarDecl ::=
      "var" Identifier ":" Type ";"
    | "let" Identifier ":" Type "=" Expr ";"

var = mutable

let = immutable



---

9. On Blocks (State Behavior)

OnBlock ::=
    "on" Identifier "{"
        Action*
        Transition
    "}"

> Important invariant:
Every on block has exactly one Transition.




---

10. Actions

Action ::=
      SendStmt
    | ReceiveStmt
    | TrySendStmt
    | TryReceiveStmt
    | AssignmentStmt


---

10.1 Send / Receive

SendStmt    ::= "send" Expr "->" Identifier ";"
ReceiveStmt ::= "receive" Identifier "->" Identifier ";"

Sugar allowed:

receive c -> var x : T;

(desugars to var x:T; receive c -> x;)


---

10.2 Try Send / Try Receive

TrySendStmt    ::= "try_send" Expr "->" Identifier "->" Identifier ";"
TryReceiveStmt ::= "try_receive" Identifier "->" Identifier ";"

Typing:

try_send result: Result<bool, text>

try_receive result: Result<T, text>



---

10.3 Assignment

AssignmentStmt ::= Identifier "=" Expr ";"


---

11. Transitions (Strongly Enforced)

Transition ::=
      GotoTransition
    | IfElseTransition


---

11.1 Goto Transition

GotoTransition ::= "->" Identifier ";"


---

11.2 Conditional Transition (Total)

IfElseTransition ::=
    "if" Expr
        BranchBlock
    "else"
        BranchBlock

BranchBlock ::=
    "{"
        Action*
        "->" Identifier ";"
    "}"

> Syntax-level rule:
if transitions must include else.
Partial conditionals are illegal.




---

12. Schedule Declaration

ScheduleDecl ::=
    "schedule" "{"
        ScheduleItem+
    "}"

ScheduleItem ::=
      "step" IntLiteral ":" Identifier ";"
    | "repeat" ";"

Example:

schedule {
  step 0: Producer;
  step 1: Consumer;
  repeat;
}

Semantic rules:

Step identifiers must name processes in the group

Schedule order is validated against pipeline topology when annotated



---

13. Types

Type ::=
      PrimitiveType
    | ResultType
    | Identifier


---

13.1 Primitive Types

PrimitiveType ::=
      "int"
    | "bool"
    | "text"
    | "real"


---

13.2 Result Type

ResultType ::= "Result" "<" Type "," Type ">"

Currently enforced:

Result<T, text> is the canonical error-carrying type



---

14. Expressions

Expr ::=
      Literal
    | Identifier
    | CallExpr
    | BinaryExpr
    | UnaryExpr
    | ResultUnwrap
    | "(" Expr ")"


---

14.1 Literals

Literal ::= IntLiteral | BoolLiteral | TextLiteral


---

14.2 Function Calls (future-proofed)

CallExpr ::= Identifier "(" ArgList? ")"
ArgList  ::= Expr ( "," Expr )*

(Currently minimal use; expands naturally.)


---

14.3 Binary Expressions

BinaryExpr ::= Expr BinaryOp Expr
BinaryOp   ::= "==" | "!=" | "<" | ">" | "<=" | ">=" | "+" | "-" | "*" | "/"


---

14.4 Unary Expressions

UnaryExpr ::= "!" Expr | "-" Expr


---

14.5 Result Unwrap Operator (?)

ResultUnwrap ::= Expr "?"

Semantic rule (critical):

Expr must have type Result<T, E>

? desugars during IR lowering into:

error write to __last_error : text

transition to __Error state



? is not a runtime operator.
It is a control-flow rewrite.


---

15. Implicit / Compiler-Injected Elements

These are not written by users, but are guaranteed to exist when needed.

15.1 Error State

state __Error;
var __last_error : text;

Injected when ? is used in a process.


---

16. Semantic Constraints (Non-Grammar)

These rules are enforced after parsing, during semantic analysis:

16.1 Group Scoping

Processes may only access channels declared in their group


16.2 State Correctness

All referenced states must exist

Every on block must have a total transition


16.3 Channel Typing

send / receive types must match channel element type


16.4 Pipeline Safety (@pipeline_safe)

Build Process → Channel → Process graph

Must be acyclic

Schedule must respect topological order

Ambiguous channels are rejected (or warned when dumping topology)


16.5 Realtime Safety (@realtimesafe)

Blocking send / receive forbidden

Only try_send / try_receive allowed

Optional cost bounds enforced


16.6 Bounded Communication

@max_sends / @max_receives validated statically

Producer bounds must not exceed consumer bounds



---

17. Execution Model (Informative)

Each process compiles to a switch(state) machine

Channels compile to ring buffers

Schedule compiles to a fixed deterministic loop

No interpreter

No runtime scheduler

Native /O2 machine code



---

18. One-Line Description (Specification-Level)

> CAPS is a deterministic, statically verified, state-machine and message-passing language whose syntax enforces total transitions, bounded communication, and analyzable concurrency.




---

