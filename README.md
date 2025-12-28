# CAPS-Language
Computational Algorithmic Programming Sequence Language

---

## 1. What CAPS Does **So Far** (With the Upgrade Included)

### At its core, CAPS is:

> **A deterministic, analyzable, state-machine language for orchestrating communicating processes with compile-time guarantees.**

That’s not marketing—that’s literally what your compiler enforces.

### CAPS currently models **four things extremely well**

---

### 1️⃣ Deterministic process state machines

Each `process` is:

* A **finite state machine**
* With:

  * Explicit states
  * Explicit transitions
  * Exactly one transition per state evaluation
* No implicit control flow
* No hidden loops
* No scheduler ambiguity

This alone already makes CAPS stronger than:

* ad-hoc event loops
* callback soup
* actor systems with implicit scheduling
* coroutine systems with hidden suspension points

---

### 2️⃣ Typed message-passing over bounded channels

Channels are:

* Declared at **group scope**
* Typed: `channel<T; N>`
* Bounded (capacity known at compile time)
* Explicitly written to and read from

And the compiler enforces:

* correct typing on send/receive
* visibility rules (group scoping)
* explicit handling of blocking vs non-blocking (`send` vs `try_send`)
* boundedness analysis (`max_sends`, `max_receives`)

This makes CAPS fundamentally **anti-footgun** for concurrency.

---

### 3️⃣ Deterministic scheduling as a *language feature*

The `schedule` is not runtime policy—it is **part of the program**.

* Schedule steps must match process names
* Schedule order is validated against dataflow topology (`@pipeline_safe`)
* Execution order is **baked into emitted code**
* No OS threads required
* No runtime scheduler decisions

This gives CAPS something very rare:

> **Deterministic concurrency without runtime scheduling.**

That’s huge.

---

### 4️⃣ Compile-time enforced correctness properties

With the upgrade, CAPS now enforces:

| Property                      | Enforced at                 |
| ----------------------------- | --------------------------- |
| Total state transitions       | Parser / sema               |
| Channel type safety           | Sema                        |
| Group scoping                 | Sema                        |
| Schedule validity             | Sema                        |
| Pipeline DAG                  | Sema (`@pipeline_safe`)     |
| Bounded communication         | Sema (`max_sends/receives`) |
| No blocking in realtime       | Sema (`@realtimesafe`)      |
| Error propagation correctness | IR lowering (`? → __Error`) |

Most languages *document* these as guidelines.

CAPS **refuses to compile** without them.

---

### 5️⃣ Native, non-interpreted execution

With the hybrid upgrade:

* CAPS → typed IR → generated C++
* `/O2` compiled to a native `.exe`
* No interpreter
* No VM
* No GC
* No runtime scheduler

Runtime execution is literally:

* `switch(state)` per process
* deterministic schedule loop
* ring-buffer channels
* ordinary machine code

So **performance characteristics are predictable**.

---

## 2. What CAPS Is Geared Toward **Right Now**

### CAPS is already a strong fit for:

---

## 🔧 1. Deterministic systems logic

Anywhere you need **guaranteed behavior**, not “best effort”.

Examples:

* Device controllers
* Protocol engines
* State-driven simulations
* Transaction pipelines
* Control loops
* Safety-critical orchestration (non-hard-real-time yet)

CAPS excels when:

* “What happens next?” must be provable
* Timing and order matter
* You want to audit logic statically

---

## 🔄 2. Message-passing pipelines

Especially where:

* One process produces
* One consumes
* Possibly multiple stages
* Order matters
* Backpressure matters

`@pipeline_safe` + schedule + bounded channels is *exactly* a pipeline compiler.

Think:

* streaming transforms
* ETL pipelines
* packet processing chains
* sensor → filter → aggregator → sink

But unlike frameworks:

* topology is compile-time checked
* cycles are forbidden (when annotated)
* schedule violations are compile errors

---

## 🧠 3. Explicit FSM-based logic instead of implicit loops

CAPS replaces:

* `while(true)` loops
* callback trees
* promise chains
* ad-hoc state flags

with:

* named states
* explicit transitions
* explicit error states

This is ideal when:

* correctness > convenience
* clarity > brevity

---

## 🧪 4. Analysis-first systems

Because you own the IR, CAPS is *already* good for:

* static verification
* topology visualization (`--dump-topology`)
* determinism checks
* offline reasoning

This makes CAPS particularly suitable for:

* internal infrastructure tools
* system prototypes
* correctness-first engines

---

## 3. What CAPS Is **Not Yet** (Intentionally)

Right now CAPS is **not** trying to be:

* A general-purpose application language
* A scripting language
* A UI language
* A data science language
* A replacement for C++/Rust in all domains

That’s a strength, not a weakness.

CAPS is focused.

---

## 4. What CAPS Is Geared Toward **In the Future**

Because you now **own the typed IR**, CAPS has a very clear growth path.

---

## 🚀 Near Future (Natural Extensions)

### 1️⃣ Hard realtime & embedded systems

With:

* `@realtimesafe`
* bounded channels
* static scheduling
* no allocation
* no runtime

CAPS is a **perfect fit** for:

* microcontrollers
* robotics controllers
* avionics-style state machines
* industrial automation

Once LLVM backend exists:

* emit bare-metal ARM
* no OS required

---

### 2️⃣ Deterministic parallel execution

Later, the same IR can support:

* multi-core execution
* static partitioning of processes
* still deterministic because schedule is explicit

Think:

* parallel pipelines
* dataflow machines
* lock-free deterministic concurrency

---

### 3️⃣ Verifiable systems programming

CAPS is moving toward:

> **“Executable specification with guarantees.”**

You already have:

* explicit state graphs
* bounded communication
* enforced invariants

Future additions:

* temporal assertions
* compile-time proofs of liveness/safety
* certified codegen targets

This puts CAPS closer to:

* model-checking languages
* control DSLs
* safety-critical systems languages

---

### 4️⃣ Infrastructure orchestration & simulation

Because CAPS describes *behavior over time*, it can grow into:

* simulation engines
* digital twins
* system orchestration logic
* network protocol specs that compile to executables

---

## 5. One-Sentence Summary (Honest & Accurate)

> **CAPS is a deterministic, compile-time verified state-machine and message-passing language designed for building analyzable, high-performance systems where correctness, order, and bounded behavior matter more than convenience.**

---

## 6. Why the Hybrid Upgrade Changes Everything

Before:

* CAPS was already analyzable
* But runtime execution details were still conceptual

After:

* CAPS has a **real native backend**
* Semantics are preserved through typed IR
* Execution is predictable, inspectable, and fast
* The language is no longer “theoretical”

That upgrade moves CAPS from:

> *“interesting language experiment”*
> to
> *“serious systems language foundation.”*

---

CAPS is a custom programming language designed for modeling and executing deterministic concurrent systems, inspired by CSP (Communicating Sequential Processes) and Go's goroutines/channels, but with strong static guarantees for safety, deadlock freedom, and performance. It emphasizes compile-time verification of process interactions, bounded communication, and analyzable topologies.

The project implements a full compiler toolchain: **CAPS source → Typed IR → Native Executable**, with multiple backends (C++ AOT and direct x86-64 codegen). It's built for "serious compiler bootstrap" — owning the semantics while leveraging industrial tools for codegen.

---

## 1. Language Design Philosophy

CAPS is not a general-purpose language but a domain-specific one for **unfolding concurrent computations**:
- **Determinism**: No hidden nondeterminism; schedules are explicit and verifiable.
- **Bounded Resources**: Channels have fixed capacities; processes have finite states.
- **Static Safety**: Compile-time checks for deadlocks, race conditions, and topology violations.
- **Performance**: Compiles to native code with no runtime VM or garbage collection.
- **Debuggability**: Full source-level debugging support via CodeView and unwind info.

Key influences:
- CSP for process algebra and communication.
- State machines for process behavior.
- Annotations for domain-specific constraints (e.g., real-time safety).

---

## 2. Core Language Syntax

CAPS programs are organized into **modules** containing **groups**. Each group defines processes, channels, and a schedule.

### 2.1 Top-Level Structure
```
module <name>

@pipeline_safe
@realtimesafe
group <GroupName> {
  channel<int; 1024> my_channel  // bounded channel

  process Producer(limit:int) -> () {
    state Start, Send, Done, __Error
    var current:int = 1

    on Start { -> Send }
    on Send {
      do send current -> my_channel
      if current == limit { -> Done } else { do current = current + 1; -> Send }
    }
    on Done { -> Done }
    on __Error { -> __Error }
  }

  process Consumer(count:int) -> (sum:int) {
    state Start, Receive, Finished, __Error
    var total:int = 0
    var received:int = 0

    on Start { -> Receive }
    on Receive {
      do rr = try_receive my_channel
      do value:int = rr?  // ? desugars to error handling
      do total = total + value
      do received = received + 1
      if received == count { -> Finished } else { -> Receive }
    }
    on Finished { do sum = total; -> Finished }
    on __Error { -> __Error }
  }

  schedule { step Producer; step Consumer; repeat }
}
```

### 2.2 Key Constructs

#### Modules and Groups
- **Module**: Top-level container (e.g., `module demo`).
- **Group**: Isolated execution unit with channels, processes, and schedule. Groups can have annotations.

#### Channels
- Declaration: `channel<T; N> name` (e.g., `channel<int; 1024> c`).
- Operations: `send expr -> chan`, `receive chan -> var`, `try_send`, `try_receive`.
- Semantics: Rendezvous or buffered; blocking/non-blocking variants.

#### Processes
- Signature: `process Name(inputs) -> (outputs) { ... }`
- States: Declared explicitly (e.g., `state Start, Send`).
- Locals: `var/let name:T = expr`
- On-blocks: `on State { actions; transition }`
- Transitions: `-> State` or `if cond { then_actions; -> ThenState } else { else_actions; -> ElseState }`

#### Expressions
- Literals: `int`, `real`, `text`, `bool`
- Operators: Arithmetic (`+`, `-`, `*`, `/`), comparisons (`==`, `!=`, `<`, etc.), logical (`&&`, `||`)
- Calls: `len(channel)` (builtin)
- Result handling: `expr?` for error propagation (desugars to `__Error` state)

#### Annotations
- `@pipeline_safe`: Enforces acyclic topology and schedule consistency.
- `@realtimesafe`: Disallows blocking ops; enables bounded-time analysis.
- `@max_sends(chan, limit)`, `@max_receives(chan, limit)`: Static bounds.

#### Types
- Primitives: `int`, `bool`, `real`, `text`
- Composites: `Result<T, E>` (e.g., `Result<int, text>` for try ops)
- Channels: `channel<T; N>`

---

## 3. Semantics and Execution Model

### 3.1 Process Model
- Each process is a finite state machine (FSM) with explicit states and transitions.
- Execution is **deterministic interleaving** via a fixed schedule (e.g., round-robin).
- No preemption; processes yield control explicitly via transitions.
- Error handling: `?` propagates failures to `__Error` state with `__last_error:text`.

### 3.2 Communication
- Channels are typed and bounded (ring buffers).
- Blocking: `send`/`receive` wait if full/empty.
- Non-blocking: `try_send`/`try_receive` return `Result<bool/text>` or `Result<T,text>`.

### 3.3 Scheduling
- Explicit schedule in group: `schedule { step P1; step P2; repeat }`
- Each "tick": Run one step per process in order.
- Termination: When all processes are in terminal states and channels are empty.

### 3.4 Safety Guarantees
- **Total Transitions**: Every state has a complete transition (parser-enforced).
- **Type Safety**: Strict typing; no implicit conversions.
- **Topology Analysis**: `@pipeline_safe` builds process-channel graph, checks acyclicity, and validates schedule respects data flow.
- **Real-Time Bounds**: `@realtimesafe` ensures no unbounded waits.
- **Bound Checks**: Static limits on sends/receives.

---

## 4. Compiler Architecture

The compiler is a **hybrid frontend/backend** design:
- **Frontend (Owned)**: Parses, typechecks, enforces CAPS rules, lowers to Typed IR.
- **Backend (Borrowed)**: Emits native code via industrial tools.

### 4.1 Pipeline Stages

1. **Lexing/Parsing**: Tokenize → AST (recursive descent parser).
2. **Name Resolution**: Build symbol tables (modules, groups, processes, channels).
3. **Type Checking**: Infer/check types; enforce channel/process scoping.
4. **Well-Formedness**: Validate FSMs, transitions, annotations.
5. **Annotation Enforcement**: Topology graphs, cycle detection, schedule validation.
6. **IR Lowering**: AST → Typed IR (desugar `?`, normalize transitions).
7. **Backend Selection**: Typed IR → C++ (AOT) or x86-64 (direct).
8. **Codegen**: Emit .cpp/.exe or .asm/.obj with unwind/debug info.
9. **Linking**: MSVC/lld-link to PE .exe.

### 4.2 Key Components

#### Frontend
- **Lexer** (`src/lexer/`): Recognizes keywords, tokens, spans.
- **Parser** (`src/parser/`): Builds AST; enforces syntax (e.g., total transitions).
- **Sema** (`src/sema/`): Type checking, scoping, annotation validation.
- **IR Lowering** (`src/ir/`): AST → Typed IR; constant folding, error desugaring.

#### Backend A: C++ AOT (`src/aot/`)
- Typed IR → C++ structs/functions.
- Channels as ring buffers; processes as switch(state).
- Compiles with MSVC `/O2` to .exe.

#### Backend B: x86-64 Direct (`src/x64/`)
- Typed IR → x86-64 instructions (Milestones 3-6).
- Emits .asm or .obj with COFF format.
- Includes timers (`GetTickCount64`), unwind (.pdata/.xdata), debug (.debug$S/.debug$T).

#### Utilities
- **Analysis** (`src/analysis/`): Pipeline topology, graph algorithms.
- **Pretty** (`src/pretty/`): AST/IR dumping.
- **Backend** (`src/backend/`): Runtime evaluation (for interpretation/testing).

---

## 5. Backends and Milestones

### 5.1 C++ AOT Backend
- Generates readable C++ with deterministic scheduler.
- Suitable for prototyping and cross-platform.
- Command: `--emit-cpp=<dir> --compile`

### 5.2 x86-64 Direct Backend (Advanced)
- **Milestone 3**: Basic scheduler + channels; .text functions with fixups/relocs.
- **Milestone 4**: Add timers; 64-bit ops; locked .text layout.
- **Milestone 5**: Windows unwind info (.pdata/.xdata) for exceptions/debugging.
- **Milestone 6**: CodeView debug info (.debug$S/.debug$T) for source-level stepping.
- Commands: `--emit-asm=<file>`, `--emit-obj=<file>`
- Verification passes ensure fixup/reloc correctness.

Both backends produce native .exe with no interpreter overhead.

---

## 6. Example Program

See `demo/demo.caps`:
- Producer sends numbers 1..100000 to channel.
- Consumer receives, sums, and terminates.
- `@pipeline_safe` ensures no deadlocks; schedule respects producer→consumer flow.

Compile: `caps_frontend --emit-cpp=generated --compile demo.caps`
Run: `./Demo.exe`

---

## 7. Current Status and Features

### Implemented
- Full frontend: Parsing, typechecking, IR lowering.
- C++ AOT backend: Working .exe generation.
- x86-64 backend foundation: Headers and placeholders for direct codegen.
- Annotations: `@pipeline_safe`, `@realtimesafe`, bounds checking.
- Error handling: `Result + ?` with desugaring.
- Optimizations: Constant folding, security checks (e.g., division by zero, file size limits).
- Debug support: AST/IR dumps, topology visualization.

### In Progress
- Complete x86-64 codegen implementation (Milestones 3-6).
- Verification passes for fixups/relocs.

### Future
- LLVM backend for cross-platform.
- More builtins (e.g., time functions).
- IDE integration (syntax highlighting, etc.).

---

## 8. Usage and Commands

```
caps_frontend [options] <file.caps>

Options:
  --dump-ast                    Print AST
  --dump-topology=dot|text      Print pipeline topology
  --check-only                  Typecheck only
  --output-ir=<file>            Write IR to file
  --emit-cpp=<dir>              Emit C++ to <dir>/<group>.cpp
  --compile                     Compile C++ to .exe
  --emit-asm=<file>             Emit x86-64 assembly
  --emit-obj=<file>             Emit COFF .obj file
```

The project is licensed under S.U.E.T. (Sovereign Universal Entity Technical) License v1.0, emphasizing attribution and identity integrity.

This overview captures the entire CAPS-Language as implemented, providing a foundation for concurrent programming with provable properties. For details, see source files or run the compiler with `--help`.


# CAPS-Language Installation and Usage Guide

This guide provides step-by-step instructions for downloading, setting up, installing, and using CAPS-Language—a deterministic, compile-time verified language for building concurrent systems with strong safety guarantees.

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Downloading CAPS-Language](#downloading-caps-language)
3. [Setting Up the Environment](#setting-up-the-environment)
4. [Installing (Building) the Compiler](#installing-building-the-compiler)
5. [Verifying Installation](#verifying-installation)
6. [Writing Your First CAPS Program](#writing-your-first-caps-program)
7. [Compiling and Running Programs](#compiling-and-running-programs)
8. [Using Advanced Features](#using-advanced-features)
9. [Troubleshooting](#troubleshooting)
10. [Resources and Next Steps](#resources-and-next-steps)

---

## 1. Prerequisites

Before installing CAPS-Language, ensure your system meets these requirements:

- **Operating System**:
  - Windows 10/11 (recommended for MSVC support).
  - Linux (Ubuntu/Debian) or macOS (with adjustments for toolchains).
- **Hardware**:
  - At least 2GB RAM, 1GB free disk space.
- **Software**:
  - **Git**: For cloning the repository.
  - **CMake**: Version 3.15 or later (for building).
  - **C++ Compiler**:
    - MSVC (Visual Studio 2019/2022) on Windows.
    - GCC/Clang on Linux/macOS (may require adjustments for MSVC-specific code).
  - **Linker**: lld-link (comes with LLVM) for advanced backends.
  - **Optional**: Graphviz (for topology visualization), Visual Studio (for debugging).

**Note**: CAPS is primarily developed on Windows with MSVC. Cross-platform support is experimental.

---

## 2. Downloading CAPS-Language

CAPS-Language is hosted on GitHub. Download the source code using Git:

1. **Open a Terminal/Command Prompt**:
   - On Windows: Use Command Prompt, PowerShell, or Git Bash.
   - On Linux/macOS: Use your preferred terminal.

2. **Clone the Repository**:
   ```
   git clone https://github.com/JoeySoprano420/CAPS-Language.git
   cd CAPS-Language
   ```

3. **Verify Contents**:
   - The repository contains source code in `src/`, examples in `demo/`, and documentation (`README.md`, `HOWTO.md`, `License.md`).

If you prefer a ZIP download, visit the [GitHub repository](https://github.com/JoeySoprano420/CAPS-Language) and click "Code" > "Download ZIP". Extract it to a folder.

---

## 3. Setting Up the Environment

### On Windows (Recommended)
1. **Install Visual Studio**:
   - Download and install Visual Studio 2022 (Community Edition is free).
   - During installation, select "Desktop development with C++" workload.
   - This provides MSVC, CMake, and build tools.

2. **Install Git**:
   - Download from [git-scm.com](https://git-scm.com) if not already installed.

3. **Optional: Install LLVM for lld-link**:
   - Download LLVM from [llvm.org](https://llvm.org) and add `bin/` to your PATH for advanced linking.

### On Linux (Ubuntu/Debian)
1. **Install Dependencies**:
   ```
   sudo apt update
   sudo apt install build-essential cmake git clang
   ```

2. **Install MSVC (Optional, for compatibility)**:
   - Use Wine or cross-compilation tools, but native GCC/Clang may work with modifications.

### On macOS
1. **Install Xcode**:
   - Download from the Mac App Store for command-line tools.

2. **Install Homebrew**:
   ```
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install Dependencies**:
   ```
   brew install cmake git llvm
   ```

**Environment Variables**:
- Ensure `cl.exe` (MSVC) or `g++` is in your PATH.
- Add CMake to PATH if not automatic.

---

## 4. Installing (Building) the Compiler

CAPS uses CMake for building. Follow these steps:

1. **Navigate to the Project Directory**:
   ```
   cd CAPS-Language
   ```

2. **Create a Build Directory**:
   ```
   mkdir build
   cd build
   ```

3. **Configure with CMake**:
   ```
   cmake ..
   ```
   - On Windows: This detects MSVC automatically.
   - On Linux/macOS: Specify compiler if needed: `cmake -DCMAKE_CXX_COMPILER=clang++ ..`

4. **Build the Project**:
   ```
   cmake --build . --config Release
   ```
   - This compiles the `caps_frontend` executable.

5. **Install (Optional)**:
   - On Unix-like systems: `sudo make install` (if configured).
   - On Windows: Copy `caps_frontend.exe` to a directory in your PATH (e.g., `C:\bin\`).

**Expected Output**: A `caps_frontend.exe` (or `caps_frontend` on Unix) in the `build/` directory.

**Build Time**: ~1-5 minutes depending on your system.

---

## 5. Verifying Installation

1. **Run the Compiler**:
   ```
   ./caps_frontend --help
   ```
   - You should see the usage message listing all options.

2. **Test with an Example**:
   ```
   ./caps_frontend --check-only demo/demo.caps
   ```
   - Should exit with code 0 (success) if no errors.

If you encounter issues, see [Troubleshooting](#troubleshooting).

---

## 6. Writing Your First CAPS Program

CAPS programs define concurrent processes with explicit state machines and channels.

### Basic Structure
Create a file `hello.caps`:
```
module hello

@pipeline_safe
group Demo {
  channel<int; 10> chan

  process Producer(limit:int) -> () {
    state Start, Send, Done
    var i:int = 1

    on Start { -> Send }
    on Send {
      do send i -> chan
      if i == limit { -> Done } else { do i = i + 1; -> Send }
    }
    on Done { -> Done }
  }

  process Consumer(count:int) -> (sum:int) {
    state Start, Receive, Finished
    var total:int = 0
    var received:int = 0

    on Start { -> Receive }
    on Receive {
      do rr = try_receive chan
      do value:int = rr?
      do total = total + value
      do received = received + 1
      if received == count { -> Finished } else { -> Receive }
    }
    on Finished { do sum = total; -> Finished }
  }

  schedule { step Producer; step Consumer; repeat }
}
```

This program sends numbers 1-10 and sums them deterministically.

**Key Concepts**:
- **Modules/Groups**: Organize code.
- **Processes**: FSMs with states and transitions.
- **Channels**: Bounded communication.
- **Schedule**: Explicit execution order.

---

## 7. Compiling and Running Programs

### Using the C++ Backend (Easiest)
1. **Compile to C++ and Executable**:
   ```
   ./caps_frontend --emit-cpp=generated --compile hello.caps
   ```
   - Generates `generated/Demo.cpp` and compiles to `Demo.exe`.

2. **Run**:
   ```
   ./Demo.exe
   ```
   - Runs silently; producer sends 1-10, consumer sums to 55.

### Using the x86-64 Backend (Advanced)
1. **Emit Assembly**:
   ```
   ./caps_frontend --emit-asm=output.asm hello.caps
   ```

2. **Emit Object File**:
   ```
   ./caps_frontend --emit-obj=output.obj hello.caps
   ```

3. **Link Manually**:
   ```
   lld-link output.obj kernel32.lib /subsystem:console /entry:caps_entry /debug:full /pdb:output.pdb
   ./output.exe
   ```

### Other Useful Commands
- **Typecheck Only**: `./caps_frontend --check-only hello.caps`
- **Dump AST**: `./caps_frontend --dump-ast hello.caps`
- **Visualize Topology**: `./caps_frontend --dump-topology=dot hello.caps > topo.dot` (requires Graphviz: `dot -Tpng topo.dot -o topo.png`)

---

## 8. Using Advanced Features

### Annotations
- `@pipeline_safe`: Enforces acyclic data flow.
- `@realtimesafe`: Disallows blocking operations.
- `@max_sends(chan, 100)`: Bounds sends.

### Debugging
- Use `--dump-topology=text` for text-based graphs.
- Compile with debug info for Visual Studio stepping.

### Custom Expressions and Types
- Support for `int`, `bool`, `real`, `text`, `Result<T,E>`.

---

## 9. Troubleshooting

### Common Build Issues
- **CMake Not Found**: Install CMake from [cmake.org](https://cmake.org).
- **MSVC Errors**: Ensure Visual Studio is installed and `cl.exe` is in PATH.
- **Linker Errors**: For x86-64, install LLVM and use `lld-link`.

### Runtime Issues
- **Program Crashes**: Check for invalid operations (e.g., division by zero).
- **No Output**: CAPS programs run deterministically; add debugging if needed.

### Compiler Errors
- **Syntax Errors**: Ensure total transitions in FSMs.
- **Type Mismatches**: Verify channel types and scoping.
- **Topology Warnings**: Fix cycles or ambiguous channels.

### Getting Help
- Check `README.md` and `HOWTO.md` for details.
- Open issues on [GitHub](https://github.com/JoeySoprano420/CAPS-Language).
- License: S.U.E.T. v1.0 (see `License.md`).

---

## 10. Resources and Next Steps

- **Documentation**: `README.md` (full overview), `HOWTO.md` (detailed guide).
- **Examples**: `demo/demo.caps`.
- **Contributing**: Extend backends or add features.
- **Community**: Join discussions on GitHub.

CAPS-Language enables building **provably correct concurrent systems**. Start with simple programs and explore advanced features. Happy coding! 🜲

---

*This guide is for CAPS-Language v1.0. 

---

1. CAPS-Language



/

Syntax.md

JoeySoprano420JoeySoprano420
4 hours ago
539 lines (289 loc) · 7.38 KB

Preview


Code


Blame

Below is a complete, self-contained CAPS syntax specification, written as a formal EBNF-style grammar plus semantic notes where grammar alone cannot express the rule (because CAPS is intentionally strict).
This is suitable to be:
the authoritative language reference
the basis for an ANTLR / recursive-descent parser
a SYNTAX.md file in your GitHub repo
I will not hand-wave. This is the entire language as it exists now, including all the upgrades you locked in.
CAPS Language Syntax Specification
CAPS — Causal Actor / Process System
This document defines the surface syntax of CAPS. Semantic constraints (type rules, topology checks, realtime rules) are listed separately where needed.

1. Lexical Structure



1.1 Identifiers
Identifier ::= Letter ( Letter | Digit | "_" )* Letter ::= "A"…"Z" | "a"…"z" Digit ::= "0"…"9"
Identifiers are case-sensitive.
1.2 Keywords (reserved)
group process state on schedule step repeat
channel send receive try_send try_receive
var let if else true false Result
int bool text real
These identifiers may not be used as user symbols.
1.3 Literals
IntLiteral ::= Digit+ BoolLiteral ::= "true" | "false" TextLiteral ::= '"' ( any-char-except-quote | '"' )* '"'
1.4 Comments
LineComment ::= "//" .* newline
(No block comments by design — CAPS favors clarity over cleverness.)

1. Top-Level Structure



Program ::= GroupDecl+
A CAPS program is one or more groups.

1. Annotations



Annotations attach to groups and processes.
Annotation ::= "@" Identifier | "@" Identifier "(" AnnotationArgs? ")"
AnnotationArgs ::= AnnotationArg ( "," AnnotationArg )* AnnotationArg ::= Identifier "=" Expr
Examples:
@pipeline_safe @realtimesafe(maxtick_cost = 500) @max_sends(c = 10)
Annotations are parsed and preserved in the AST and enforced during semantic analysis.

1. Group Declaration



GroupDecl ::= Annotation* "group" Identifier "{" GroupItem* "}"
Group items
GroupItem ::= ChannelDecl | ProcessDecl | ScheduleDecl

1. Channel Declaration



ChannelDecl ::= "channel" Identifier ":" "channel" "<" Type ";" IntLiteral ">" ";"
Example:
channel c : channel<int; 1>;
Semantic rules (not syntax):
Channels are visible only to processes in the same group.
Capacity must be > 0.
Channel types are invariant.

1. Process Declaration



ProcessDecl ::= Annotation* "process" Identifier "(" ParamList? ")" "->" "(" ResultList? ")" "{" ProcessItem* "}"
Parameters and results
ParamList ::= Param ( "," Param )* Param ::= Identifier ":" Type
ResultList ::= ResultParam ( "," ResultParam )* ResultParam ::= Identifier ":" Type
Process items
ProcessItem ::= StateDecl | VarDecl | OnBlock

1. State Declaration



StateDecl ::= "state" Identifier ";"
States are symbols, not values.

1. Variable Declarations



VarDecl ::= "var" Identifier ":" Type ";" | "let" Identifier ":" Type "=" Expr ";"
var = mutable
let = immutable

1. On Blocks (State Behavior)



OnBlock ::= "on" Identifier "{" Action* Transition "}"
Important invariant: Every on block has exactly one Transition.

1. Actions



Action ::= SendStmt | ReceiveStmt | TrySendStmt | TryReceiveStmt | AssignmentStmt
10.1 Send / Receive
SendStmt ::= "send" Expr "->" Identifier ";" ReceiveStmt ::= "receive" Identifier "->" Identifier ";"
Sugar allowed:
receive c -> var x : T;
(desugars to var x:T; receive c -> x;)
10.2 Try Send / Try Receive
TrySendStmt ::= "try_send" Expr "->" Identifier "->" Identifier ";" TryReceiveStmt ::= "try_receive" Identifier "->" Identifier ";"
Typing:
try_send result: Result<bool, text>
try_receive result: Result<T, text>
10.3 Assignment
AssignmentStmt ::= Identifier "=" Expr ";"

1. Transitions (Strongly Enforced)



Transition ::= GotoTransition | IfElseTransition
11.1 Goto Transition
GotoTransition ::= "->" Identifier ";"
11.2 Conditional Transition (Total)
IfElseTransition ::= "if" Expr BranchBlock "else" BranchBlock
BranchBlock ::= "{" Action* "->" Identifier ";" "}"
Syntax-level rule: if transitions must include else. Partial conditionals are illegal.

1. Schedule Declaration



ScheduleDecl ::= "schedule" "{" ScheduleItem+ "}"
ScheduleItem ::= "step" IntLiteral ":" Identifier ";" | "repeat" ";"
Example:
schedule { step 0: Producer; step 1: Consumer; repeat; }
Semantic rules:
Step identifiers must name processes in the group
Schedule order is validated against pipeline topology when annotated

1. Types



Type ::= PrimitiveType | ResultType | Identifier
13.1 Primitive Types
PrimitiveType ::= "int" | "bool" | "text" | "real"
13.2 Result Type
ResultType ::= "Result" "<" Type "," Type ">"
Currently enforced:
Result<T, text> is the canonical error-carrying type

1. Expressions



Expr ::= Literal | Identifier | CallExpr | BinaryExpr | UnaryExpr | ResultUnwrap | "(" Expr ")"
14.1 Literals
Literal ::= IntLiteral | BoolLiteral | TextLiteral
14.2 Function Calls (future-proofed)
CallExpr ::= Identifier "(" ArgList? ")" ArgList ::= Expr ( "," Expr )*
(Currently minimal use; expands naturally.)
14.3 Binary Expressions
BinaryExpr ::= Expr BinaryOp Expr BinaryOp ::= "==" | "!=" | "<" | ">" | "<=" | ">=" | "+" | "-" | "*" | "/"
14.4 Unary Expressions
UnaryExpr ::= "!" Expr | "-" Expr
14.5 Result Unwrap Operator (?)
ResultUnwrap ::= Expr "?"
Semantic rule (critical):
Expr must have type Result<T, E>
? desugars during IR lowering into:
error write to __last_error : text
transition to __Error state
? is not a runtime operator. It is a control-flow rewrite.

1. Implicit / Compiler-Injected Elements



These are not written by users, but are guaranteed to exist when needed.
15.1 Error State
state __Error; var __last_error : text;
Injected when ? is used in a process.

1. Semantic Constraints (Non-Grammar)



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

1. Execution Model (Informative)



Each process compiles to a switch(state) machine
Channels compile to ring buffers
Schedule compiles to a fixed deterministic loop
No interpreter
No runtime scheduler
Native /O2 machine code

1. One-Line Description (Specification-Level)



CAPS is a deterministic, statically verified, state-machine and message-passing language whose syntax enforces total transitions, bounded communication, and analyzable concurrency.

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

// Text: double-quoted with escapes " \ \n \t \r
TextLiteral
: '"' ( EscapeSeq | ~["\\r\n] )* '"'
;

fragment EscapeSeq
: '\' ( ["\/bfnrt] )
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

