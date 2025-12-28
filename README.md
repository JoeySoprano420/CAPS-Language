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

- *****

- 
