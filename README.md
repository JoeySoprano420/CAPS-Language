# CAPS-Language
Computational Algorithmic Programming Sequence Language

The Origin of CAPS — A Narrative
CAPS didn’t begin as a language. It began as a frustration.

For years, engineers kept building systems that looked deterministic on paper but behaved unpredictably in the real world. Pipelines that reordered messages under load. Controllers that drifted into undefined states. Protocol engines that passed every test until the one moment timing shifted and everything fell apart. Everyone blamed “race conditions,” “scheduler quirks,” or “edge cases,” but the truth was simpler: the languages themselves were nondeterministic by design.

So developers did what they always do — they fought the language.
They hand‑rolled state machines in C. They built ring buffers from scratch. They wrote elaborate comments explaining the “intended” order of events. They prayed the runtime scheduler would behave. They tested, and tested, and tested again, hoping to catch the one interleaving that would break everything.

But hope is not a concurrency model.

The idea behind CAPS emerged from that tension — the realization that correctness shouldn’t depend on timing, luck, or the mercy of a scheduler. If a system has states, those states should be explicit. If it passes messages, those messages should be typed and bounded. If it runs concurrently, the schedule should be part of the program, not a hidden runtime decision.

CAPS grew from the belief that determinism isn’t a luxury.
It’s a foundation.

It draws its lineage from CSP’s mathematical clarity, Occam’s executable determinism, Promela’s explicit state machines, and the synchronous world’s obsession with analyzability. But it refuses to stay theoretical. CAPS compiles to native machine code. It enforces correctness at compile time. It treats concurrency as something you can prove, not something you hope works.

CAPS was created for the engineers who build the parts of the world that can’t afford surprises — the controllers, pipelines, protocol engines, and embedded systems where behavior must be predictable, auditable, and safe. It exists for the people who have been writing deterministic systems by hand for decades, wishing the language would help instead of getting in the way.

CAPS is the answer to that wish.
A language where the state machine is the program.
Where the schedule is explicit.
Where communication is bounded.
Where correctness is enforced.
Where concurrency is deterministic by design.

CAPS didn’t set out to be a general‑purpose language.
It set out to be a reliable one.

And that’s the story of how it came to be.

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

It’s not halfway between C++ and Go in terms of features or ergonomics.
It is between them in terms of purpose and level of abstraction — but with a completely different philosophy.

Here’s the real shape of it:

How CAPS relates to C++
CAPS is like C++ in that:

it compiles to native machine code

it has no runtime, no GC, no scheduler

performance is predictable

execution is basically a switch(state) loop per process

But CAPS is much higher‑level than C++ in how you express behavior:

you don’t write loops — you write states

you don’t write threads — you write processes

you don’t manage queues — you use typed, bounded channels

you don’t design schedulers — the schedule is part of the language

So CAPS is lower‑level in execution, but higher‑level in semantics.

How CAPS relates to Go
CAPS is like Go in that:

it uses message passing

it encourages pipeline thinking

it models systems as communicating processes

But CAPS is the opposite of Go in the things that matter:

Go has goroutines → CAPS has explicit state machines

Go has a runtime scheduler → CAPS has compile‑time scheduling

Go channels are unbounded unless you specify → CAPS channels are always bounded

Go is nondeterministic → CAPS is deterministic by design

So CAPS is higher‑integrity and more analyzable than Go, but not “easier.”

The clean answer:

CAPS sits between C++ and Go in purpose, but it’s philosophically closer to CSP and synchronous languages than either of them.

CAPS is C++‑level execution with Go‑style message passing, but fully deterministic and compile‑time verified.

*****

How Close Is CAPS to C, Ada, and Fortran?
CAPS vs C
Closest in execution model, farthest in semantics.

Similarities:

Native machine code

No runtime, no GC

Predictable performance

Straight‑line, low‑level execution model (your compiler emits a switch(state) loop per process)

Differences:

C has no concurrency model — you build everything yourself

CAPS has deterministic concurrency built in

C has implicit control flow everywhere

CAPS has explicit finite‑state machines only

C has unbounded, unsafe queues unless you implement them

CAPS has typed, bounded channels enforced at compile time

Summary:  
CAPS is lower‑level like C in execution, but much higher‑level in concurrency semantics.

CAPS vs Ada
This is the closest match of the three — but CAPS is far more deterministic.

Similarities:

Ada is designed for safety‑critical systems

Ada has tasking, rendezvous, and deterministic‑leaning constructs

Ada emphasizes compile‑time correctness

Ada is used in avionics, robotics, embedded systems — the same domains CAPS is targeting

Differences:

Ada tasks are runtime‑scheduled

CAPS processes are compile‑time‑scheduled

Ada concurrency is structured but not deterministic

CAPS concurrency is fully deterministic

Ada allows dynamic behavior

CAPS enforces bounded, analyzable behavior only

Summary:  
CAPS is like a stricter, more deterministic, CSP‑style Ada with no runtime and no nondeterminism.

CAPS vs Fortran
Almost no overlap except “native performance.”

Similarities:

Fortran is fast

CAPS is fast

Both compile to native code

Differences:

Fortran is a numerical computing language

CAPS is a deterministic concurrency language

Fortran has no concurrency model

CAPS is built entirely around concurrency

Fortran is used for scientific computing

CAPS is used for controllers, pipelines, protocol engines, embedded systems

Summary:  
CAPS and Fortran live in completely different universes.

The Cleanest Way to Say It
CAPS is execution‑level close to C, safety‑philosophy close to Ada, and nothing like Fortran.

Or even tighter:

CAPS = C‑level performance + Ada‑style safety + CSP‑level determinism.

*****

Why CAPS Is Different
Most languages treat concurrency as a runtime problem. Threads, goroutines, actors, async/await — they all rely on schedulers, queues, and implicit control flow that you can’t fully see or reason about. CAPS takes the opposite approach.

CAPS makes concurrency explicit, deterministic, and compile‑time verified. Every process is a finite‑state machine. Every channel is typed and statically bounded. Every schedule is part of the program itself. There are no hidden loops, no implicit suspension points, no runtime scheduler, and no nondeterministic interleaving.

Where other languages offer guidelines, CAPS offers guarantees.
If a pipeline is unsafe, if a schedule is invalid, if communication is unbounded, if a state machine is incomplete — the program simply will not compile.

This makes CAPS fundamentally different from general‑purpose systems languages. It isn’t trying to replace C++, Rust, Go, or Erlang. It’s solving a different problem: building systems where correctness, ordering, and bounded behavior matter more than convenience.

CAPS is not a theoretical model — it compiles to native machine code with predictable performance. It gives engineers a way to build controllers, pipelines, protocol engines, and deterministic systems with the same confidence you’d expect from formal verification tools, but in an executable language.

CAPS is different because it turns concurrency into something you can prove — not something you hope works.

*****

Language Taxonomy: Where CAPS Fits
CAPS doesn’t belong to the usual “systems language” categories like C, C++, Rust, or Go. It sits in a much rarer lineage — one defined by deterministic concurrency, explicit state machines, and compile‑time verification.

Here’s the taxonomy that shows its true family.

1. General‑Purpose Systems Languages
(Performance‑oriented, flexible, nondeterministic)  
Examples: C, C++, Rust, Zig

Native code

Manual or ownership‑based memory control

Threads, atomics, async/await

Nondeterministic concurrency

Runtime scheduling

CAPS is not in this category.

2. Runtime‑Scheduled Concurrency Languages
(Easy concurrency, nondeterministic execution)  
Examples: Go, Erlang/Elixir, Java, Kotlin, Python asyncio

Goroutines / actors / coroutines

Unbounded or semi‑bounded queues

Runtime schedulers

Nondeterministic interleaving

CAPS is the opposite of this category.

3. Formal Concurrency & Verification Languages
(Deterministic semantics, analyzable, not executable)  
Examples: CSP, Promela/SPIN, TLA+, Esterel, Lustre, SCADE

Explicit processes

Deterministic communication

Algebraic or synchronous semantics

Strong compile‑time reasoning

Usually not compiled to native executables

CAPS is closest to this category — but with a real compiler.

4. Embedded & Real‑Time DSLs
(Deterministic, bounded, domain‑specific)  
Examples: Stateflow, LabVIEW RT, synchronous dataflow DSLs

Deterministic scheduling

Bounded execution

State‑machine‑driven logic

Often domain‑specific or tool‑based

CAPS shares the determinism and analyzability, but is a full language.

5. CAPS’s Actual Category
Deterministic Concurrency Language (DCL)
(Executable, analyzable, compile‑time‑verified, state‑machine‑based)

CAPS combines:

CSP‑style processes

Typed, bounded channels

Explicit finite‑state machines

Compile‑time topology verification

Deterministic scheduling

Native code generation

This makes CAPS part of a very small class of languages that includes:

Occam / Occam‑π

Promela (conceptually)

Esterel / Lustre (philosophically)

But CAPS is unique because it is:

executable

native‑compiled

deterministic by design

fully verified at compile time

built for real systems, not just models

The One‑Line Classification
CAPS is a Deterministic Concurrency Language — a high‑level, analyzable systems language built on explicit state machines and compile‑time guarantees.

**

What Kind of Language Is CAPS?
CAPS is a deterministic concurrency language — a high‑level systems language built on explicit state machines, typed message passing, and compile‑time guarantees. It isn’t a general‑purpose language, and it isn’t a runtime‑scheduled concurrency model like threads, goroutines, or actors. Instead, CAPS treats concurrency as something that must be provable, bounded, and fully analyzable before the program ever runs.

At its core, CAPS combines the clarity of formal models like CSP and Promela with the practicality of native code generation. Every process is a finite‑state machine. Every channel is typed and statically bounded. Every schedule is part of the program itself. There is no runtime scheduler, no hidden control flow, and no nondeterministic interleaving.

This makes CAPS a different kind of systems language — one where correctness, ordering, and bounded behavior are enforced at compile time, and where execution is as predictable as a hand‑written state machine in C. CAPS is designed for engineers who need deterministic pipelines, controllers, protocol engines, and embedded logic that must behave the same way every time.

In short: CAPS is a high‑level deterministic systems language that compiles to low‑level native code, giving you concurrency you can reason about, verify, and trust.

*****

What CAPS Replaces
CAPS isn’t trying to replace general‑purpose languages — it replaces the ad‑hoc concurrency glue that engineers are forced to write in those languages. Anywhere you currently hand‑roll state machines, message‑passing logic, or deterministic pipelines, CAPS gives you a safer, analyzable, compile‑time‑verified alternative.

Here’s what CAPS eliminates from modern systems work.

Ad‑Hoc State Machines
Most systems end up with hidden or improvised state machines:

while(true) loops

switch‑case spaghetti

callback chains

flag‑driven control flow

CAPS replaces all of this with explicit, named finite‑state processes that the compiler verifies for totality and correctness.

Hand‑Rolled Concurrency
In C, C++, Rust, Go, and most systems languages, you end up building:

custom queues

ring buffers

thread pools

goroutine orchestration

actor‑like message loops

CAPS replaces these with typed, bounded channels and deterministic scheduling built into the language.

Runtime Schedulers
Most languages rely on:

OS threads

goroutine schedulers

actor runtimes

async executors

These introduce nondeterminism and hidden interleavings.

CAPS replaces them with a compile‑time schedule that is part of the program itself. No runtime decisions. No surprises.

Pipeline Glue Code
In streaming or ETL systems, you often write:

producer/consumer loops

backpressure logic

topology validation

ordering guarantees

cycle detection

CAPS replaces all of this with:

@pipeline_safe

static topology checks

bounded communication analysis

deterministic execution order

The compiler enforces pipeline correctness so you don’t have to.

Verification‑By‑Hand
In most languages, correctness is something you hope for:

code reviews

runtime tests

stress tests

race detectors

static analyzers

CAPS replaces this with compile‑time enforcement:

schedule validity

boundedness

total transitions

error‑propagation correctness

pipeline DAG validation

If it compiles, the concurrency is correct.

Embedded Control Logic in C/C++
For embedded and real‑time systems, engineers often write:

hand‑crafted state loops

interrupt‑driven logic

manual buffer management

timing‑sensitive code

CAPS replaces this with:

explicit state machines

deterministic scheduling

no dynamic allocation

predictable machine‑code generation

It gives embedded developers the determinism they already try to enforce manually.

In Short
CAPS replaces the unsafe, ad‑hoc concurrency infrastructure you normally have to build yourself.  
It doesn’t replace your whole stack — it replaces the parts where correctness, ordering, and bounded behavior matter most.

*****

When to Use CAPS
CAPS is designed for systems where determinism, analyzability, and bounded behavior are non‑negotiable. If you need concurrency you can prove instead of concurrency you merely test, CAPS is the right choice.

Use CAPS when the answers to the questions below are “yes.”

1. Does your system have explicit states?
If your logic naturally breaks down into:

idle → running → waiting → done

disconnected → connecting → connected → error

pending → authorized → settled

…then CAPS gives you a native way to express that structure as finite‑state processes with compiler‑verified transitions.

2. Do you need deterministic behavior?
If your system must behave the same way every time — regardless of timing, load, or scheduling — CAPS is built for that.

Use CAPS when you need:

predictable ordering

no race conditions

no scheduler surprises

no nondeterministic interleavings

3. Are you building a message‑passing pipeline?
CAPS is ideal for:

producer → consumer

sensor → filter → aggregator

parser → transformer → sink

packet → decoder → handler

If your system is a pipeline, CAPS gives you:

@pipeline_safe topology checks

bounded channels

compile‑time backpressure analysis

deterministic stage ordering

4. Do you need compile‑time guarantees?
Use CAPS when you want the compiler to enforce:

schedule validity

total state transitions

bounded communication

correct send/receive matching

no blocking in realtime contexts

error‑propagation correctness

If it compiles, the concurrency is correct.

5. Are you building embedded, control, or real‑time logic?
CAPS is a strong fit for:

device controllers

robotics sequences

IoT firmware

industrial automation

protocol engines

avionics‑style state machines

Anywhere you’d normally hand‑roll a deterministic loop in C, CAPS gives you a safer, analyzable alternative.

6. Do you want native performance without a runtime?
CAPS compiles to:

typed IR → C++ → optimized native

or direct x86‑64 codegen

No GC.
No VM.
No scheduler.
No threads.
Just machine code.

Use CAPS when you need predictable performance with zero runtime overhead.

When Not to Use CAPS
CAPS is not for:

GUIs

mobile apps

web apps

scripting

data science

general‑purpose programming

It’s a systems language, not an application language.

In Short
Use CAPS when you need deterministic, analyzable, correctness‑first systems — pipelines, controllers, protocol engines, and embedded logic that must behave exactly the same way every time.

*****

Language Family Tree: Where CAPS Sits

                           ┌──────────────────────────────┐
                           │     Programming Languages     │
                           └──────────────────────────────┘
                                        │
                                        ▼
                     ┌──────────────────────────────────────────┐
                     │           Systems Languages              │
                     └──────────────────────────────────────────┘
                                        │
        ┌───────────────────────────────┼────────────────────────────────┐
        ▼                               ▼                                ▼
┌────────────────┐            ┌────────────────────┐          ┌──────────────────────┐
│ Low-Level / C   │            │  Runtime Concurrency │          │ Formal Concurrency   │
│ Family          │            │  (Threads/Actors)    │          │ & Verification        │
└────────────────┘            └────────────────────┘          └──────────────────────┘
        │                               │                                │
        ▼                               ▼                                ▼
  C, C++, Zig, Rust              Go, Erlang/Elixir, Java        CSP, Promela, Esterel,
  (manual control,               (scheduler-driven,              Lustre, SCADE
   nondeterministic)              nondeterministic)              (deterministic models)
        │                               │                                │
        └───────────────┬───────────────┴───────────────┬────────────────┘
                        ▼                               ▼
            ┌──────────────────────┐         ┌──────────────────────────────┐
            │  Embedded & Control  │         │  Deterministic Concurrency   │
            │  DSLs / FSM Tools    │         │  Languages (Rare Class)      │
            └──────────────────────┘         └──────────────────────────────┘
                        │                               │
                        ▼                               ▼
          Stateflow, LabVIEW RT,               Occam / Occam-π, synchronous
          synchronous dataflow DSLs            languages, CSP-inspired models
                        │                               │
                        └───────────────┬───────────────┘
                                        ▼
                         ┌────────────────────────────────┐
                         │              CAPS               │
                         └────────────────────────────────┘


How to Read This Tree
CAPS does not descend from:
C/C++/Rust (too flexible, nondeterministic)

Go/Erlang (runtime schedulers, unbounded queues)

Fortran (numerical, not concurrency‑driven)

CAPS does descend from:
CSP (processes + channels + determinism)

Occam / Occam‑π (executable CSP)

Promela / SPIN (explicit state machines + verification)

Synchronous languages like Esterel/Lustre (deterministic, analyzable)

CAPS adds what those languages lack:
A real compiler

Typed IR

Native executables

Compile‑time schedule validation

Bounded channel analysis

Error‑propagation correctness

Deterministic pipelines as a language feature

The One‑Line Summary
CAPS sits in the tiny family of deterministic concurrency languages — descended from CSP and synchronous systems, but compiled like C.

*****

CAPS vs Its Ancestors
CAPS inherits ideas from several influential concurrency and verification models — but it isn’t a clone of any of them. It takes the determinism and analyzability of its ancestors and turns them into a practical, native‑compiled systems language.

Here’s how CAPS compares to the lineage it comes from.

CAPS vs CSP (Communicating Sequential Processes)
CSP is the theory. CAPS is the executable version.

CSP gives you:

algebraic process composition

deterministic communication semantics

formal reasoning tools

no real compiler or runtime

CAPS adds:

a real compiler and typed IR

native executables

compile‑time schedule validation

bounded channel analysis

explicit finite‑state processes

In short: CAPS takes CSP’s semantics and makes them practical for real systems.

CAPS vs Occam / Occam‑π
Closest ancestor — but CAPS is stricter, safer, and more analyzable.

Occam gives you:

CSP‑style processes and channels

deterministic concurrency

lightweight processes

static structure

CAPS adds:

typed, bounded channels with max‑send/receive analysis

explicit finite‑state machines

compile‑time pipeline safety (@pipeline_safe)

deterministic scheduling as a language feature

error‑propagation correctness

modern IR and native codegen

In short: CAPS is Occam evolved for modern correctness‑first systems.

CAPS vs Promela / SPIN
Promela is for verification. CAPS is for execution.

Promela gives you:

explicit state machines

message‑passing processes

model checking

nondeterministic exploration

CAPS gives you:

deterministic execution

compile‑time guarantees instead of model checking

native performance

no nondeterministic branching

In short: Promela explores all behaviors; CAPS enforces one correct behavior.

CAPS vs Esterel / Lustre / Synchronous Languages
Philosophically aligned — but CAPS is not synchronous.

Synchronous languages give you:

deterministic, cycle‑based semantics

strong compile‑time guarantees

excellent for embedded and control systems

CAPS gives you:

deterministic scheduling

explicit state machines

bounded communication

native code without a synchronous runtime

In short: CAPS shares the determinism and analyzability, but uses CSP‑style processes instead of synchronous clocks.

CAPS vs Embedded FSM Tools (Stateflow, LabVIEW RT, etc.)
Similar determinism — but CAPS is a real language.

FSM tools give you:

visual state machines

deterministic execution

domain‑specific workflows

CAPS adds:

a full programming language

typed channels

compile‑time pipeline and schedule verification

native code generation

In short: CAPS gives you the determinism of FSM tools without the limitations of a DSL.

The Big Picture
CAPS inherits:

determinism from CSP

executability from Occam

explicit state machines from Promela

compile‑time guarantees from synchronous languages

systems‑level performance from C‑family execution models

But it combines them into something none of its ancestors achieved:

A deterministic, analyzable, compile‑time‑verified concurrency language that compiles to native machine code.

*****

Evolution Timeline: How CAPS Emerges

1960s–1970s: Foundations of Deterministic Concurrency
-----------------------------------------------------
• Early formal models of computation
• Rise of process calculi and algebraic reasoning
• Growing need for analyzable concurrent systems
        │
        ▼

1978: CSP (Communicating Sequential Processes)
----------------------------------------------
• Tony Hoare introduces CSP
• Processes + channels + deterministic communication
• Algebraic, mathematical, verification‑friendly
• Not executable; purely a formal model
        │
        ▼

1983–1990s: Occam and Occam‑π
------------------------------
• Occam brings CSP into an executable language
• Deterministic concurrency on the Transputer
• Channels, processes, parallel composition
• Occam‑π adds mobility and modernizes the model
• Still limited by hardware era and tooling
        │
        ▼

1990s–2000s: Verification & Synchronous Languages
-------------------------------------------------
• Promela/SPIN: explicit state machines + model checking
• Esterel/Lustre/SCADE: deterministic synchronous semantics
• Strong compile‑time guarantees, but not general languages
• Reinforces the value of analyzable concurrency
        │
        ▼

2000s–2020s: Modern Systems Languages (C++, Rust, Go)
-----------------------------------------------------
• C++: zero‑cost abstractions, but nondeterministic concurrency
• Rust: memory safety, but async/await adds hidden scheduling
• Go: goroutines + channels, but runtime‑scheduled and nondeterministic
• None provide deterministic, analyzable concurrency
        │
        ▼

2020s: Determinism Returns (Embedded, Robotics, Safety)
-------------------------------------------------------
• Renewed demand for predictable systems
• Robotics, avionics, industrial automation need provable behavior
• Developers hand‑roll FSMs, pipelines, and bounded queues in C/C++
• No modern language offers deterministic concurrency by design
        │
        ▼

CAPS: A Modern Deterministic Concurrency Language
-------------------------------------------------
• Explicit finite‑state machines
• Typed, bounded channels with compile‑time analysis
• Deterministic scheduling as a language feature
• Pipeline safety (`@pipeline_safe`) and topology verification
• Error‑propagation correctness
• Native machine‑code generation (no runtime, no GC)
• The first practical successor to CSP/Occam for modern systems

The One‑Line Summary
CAPS is the modern evolution of CSP and Occam — a deterministic concurrency language rebuilt for today’s systems, with native performance and compile‑time guarantees.

*****

Deterministic Systems Design
Deterministic systems design is an engineering approach where a system’s behavior is fully predictable, fully analyzable, and fully bounded before it ever runs. Instead of relying on runtime schedulers, implicit control flow, or best‑effort concurrency, deterministic systems are built so that every state, transition, message, and ordering decision is known and provable at compile time.

In deterministic systems design:

State is explicit, not hidden in loops, callbacks, or flags

Communication is bounded, not left to unbounded queues or dynamic buffers

Scheduling is fixed, not delegated to threads, goroutines, or actor runtimes

Behavior is reproducible, not dependent on timing or load

Correctness is enforced, not inferred through testing or race detectors

This approach is common in safety‑critical domains — avionics, robotics, industrial automation, embedded controllers — where nondeterminism is unacceptable and correctness must be guaranteed, not hoped for.

CAPS is built around this philosophy. It turns deterministic systems design into a language feature: explicit finite‑state machines, typed bounded channels, compile‑time schedule validation, and native execution without a runtime. The result is a programming model where concurrency is something you can reason about, verify, and trust.

*****

Why Determinism Matters
Determinism matters because modern systems fail in the gaps between what developers intend and what their concurrency model actually does. Most languages rely on threads, goroutines, actors, or async runtimes — all of which introduce hidden scheduling, unbounded queues, and timing‑dependent behavior that can’t be fully predicted or reasoned about. These systems work most of the time, but they’re fundamentally nondeterministic: the exact ordering of events depends on load, timing, and runtime decisions outside the developer’s control.

Deterministic systems eliminate that uncertainty. When a system is deterministic, the same inputs always produce the same behavior, in the same order, with the same transitions. There are no race conditions to “hope” you avoided, no scheduler surprises, no timing‑dependent bugs that only appear in production. Determinism turns concurrency from a probabilistic problem into a logical one — something you can analyze, verify, and trust.

This is why determinism is essential in controllers, pipelines, protocol engines, embedded systems, and safety‑critical logic. These domains can’t rely on best‑effort behavior or runtime heuristics. They need guarantees. Determinism makes systems auditable, testable, and predictable, and it allows correctness to be enforced at compile time instead of discovered through failure.

CAPS is built around this principle. By making state, communication, and scheduling explicit — and by verifying them at compile time — CAPS ensures that concurrency behaves exactly as designed, every time.

*****

Deterministic Design Checklist (CAPS‑Specific)
Use this checklist to ensure your CAPS program is fully deterministic, analyzable, and correctness‑first — exactly what the language is built for.

1. State Machines Are Explicit
Every process is modeled as a finite‑state machine

All states are named, not implied

All transitions are explicit, not hidden in loops

No “flag‑driven” or “implicit” control flow

Every state has exactly one transition per evaluation

If the logic can’t be expressed as explicit states, it’s not CAPS‑ready.

2. Communication Is Bounded and Typed
All channels are declared with static capacity

Channel types are correct and enforced

No unbounded queues

No dynamic message shapes

Backpressure is handled by design, not by accident

If communication isn’t bounded, determinism is impossible.

3. Scheduling Is Explicit
The schedule is written in the program

Every step names a specific process

No runtime scheduler

No timing‑dependent behavior

No nondeterministic interleavings

If the schedule isn’t explicit, the system isn’t deterministic.

4. Topology Is Verified
Pipelines are annotated with @pipeline_safe

Cycles are forbidden unless explicitly modeled

Producer/consumer relationships are clear

All send/receive pairs match

The compiler validates the entire dataflow graph

If the topology isn’t statically safe, CAPS will reject it.

5. Communication Is Bounded by Analysis
max_sends and max_receives are satisfied

No channel can overflow

No process can starve

No deadlocks from mismatched communication

All blocking behavior is intentional and explicit

If boundedness can’t be proven, the program won’t compile.

6. Realtime Safety Is Declared
Realtime processes use @realtimesafe

No blocking operations inside realtime contexts

No dynamic allocation

No unbounded loops

Execution time is predictable

If it must be realtime, it must be statically safe.

7. Error Handling Is Deterministic
Error transitions are explicit

? lowering to __Error is correct

No hidden exception paths

All error states are reachable and handled

No silent failure modes

If errors aren’t modeled as states, they aren’t deterministic.

8. Execution Is Native and Predictable
The program compiles to typed IR

IR lowers to C++ or native code without altering semantics

No GC, no VM, no runtime scheduler

Performance is a function of the state machine, not the runtime

If execution isn’t predictable, the design isn’t deterministic.

In Short
A CAPS program is deterministic when:

State is explicit.
Communication is bounded.
Scheduling is fixed.
Topology is safe.
Errors are modeled.
Execution is predictable.

If all of those are true, you’re building systems the CAPS way.

******

CAPS’s Conceptual DNA
A text‑based diagram showing the core ingredients that make CAPS what it is — its ancestry, its philosophy, and the execution model that sets it apart.

                          CAPS’s Conceptual DNA

        ┌──────────────────────────┬──────────────────────────┬──────────────────────────┐
        │        CSP Theory        │     Synchronous Logic     │   Low-Level Execution    │
        │   (Processes + Channels) │   (Determinism + Static   │   (Native Code, No GC,   │
        │                          │        Guarantees)        │     No Runtime)          │
        └───────────────┬──────────┴──────────────┬───────────┴──────────────┬──────────┘
                        ▼                         ▼                          ▼
              ┌────────────────┐        ┌────────────────────┐      ┌────────────────────┐
              │   Occam / π    │        │  Promela / SPIN     │      │   C / C++ / LLVM    │
              │ (Executable CSP)│        │ (Explicit FSMs)     │      │ (Predictable Perf.) │
              └────────┬───────┘        └──────────┬─────────┘      └──────────┬─────────┘
                       ▼                            ▼                           ▼
            ┌────────────────────┐       ┌──────────────────────┐     ┌──────────────────────┐
            │  Deterministic     │       │  Compile-Time         │     │  Zero-Runtime        │
            │  Concurrency       │       │  Verification         │     │  Execution Model     │
            └─────────┬──────────┘       └──────────┬───────────┘     └──────────┬──────────┘
                      ▼                             ▼                            ▼
                     ┌──────────────────────────────────────────────────────────────┐
                     │                             CAPS                             │
                     │  Deterministic Concurrency Language (DCL)                    │
                     │  • Explicit FSMs                                              │
                     │  • Typed, bounded channels                                    │
                     │  • Compile-time schedule + topology checks                    │
                     │  • Native machine code                                        │
                     └──────────────────────────────────────────────────────────────┘


n short:  
CAPS = CSP semantics + synchronous determinism + Occam executability + Promela‑style explicitness + C‑level performance.

CAPS Design Principles
These are the core principles that define how CAPS works and how CAPS programs should be written. They’re the philosophical backbone of the language.

1. Determinism First
CAPS treats determinism as a language feature, not an emergent property.
Every process, transition, and message ordering is fixed and analyzable.

2. State Machines as the Unit of Logic
All behavior is expressed as explicit finite‑state machines.
No hidden loops. No implicit control flow. No “magic” concurrency.

3. Communication Must Be Bounded
All channels have static capacity.
All sends and receives are matched and analyzable.
Unbounded queues are forbidden by design.

4. Scheduling Is Part of the Program
There is no runtime scheduler.
The schedule is written by the developer and validated by the compiler.
Execution order is guaranteed.

5. Topology Must Be Safe
Pipelines, DAGs, and process networks must be statically valid.
@pipeline_safe enforces acyclic, analyzable dataflow.
If the topology is unsafe, the program does not compile.

6. Errors Are States, Not Exceptions
Error handling is explicit and deterministic.
? lowering to __Error is validated at IR time.
No hidden exception paths or runtime surprises.

7. No Runtime, No GC, No Threads
CAPS compiles to native machine code with predictable performance.
No garbage collector, no VM, no scheduler, no dynamic allocation.
Execution is as transparent as a hand‑written state loop in C.

8. Compile‑Time Guarantees Over Runtime Heuristics
If something can be checked at compile time, CAPS checks it.
If something cannot be proven safe, CAPS rejects it.
Correctness is enforced, not inferred.

9. Behavior Must Be Auditable
CAPS programs are designed to be inspected, reasoned about, and verified.
The IR is typed, structured, and analyzable.
The runtime behavior is predictable and inspectable.

10. Simplicity Over Cleverness
CAPS favors clarity, explicitness, and analyzability over abstraction.
If a feature introduces nondeterminism or ambiguity, it does not belong in CAPS.

*****

Evolution Timeline: From CSP → Occam → Modern Deterministic Systems → CAPS

1960s–1970s: Foundations of Formal Concurrency
----------------------------------------------
• Early process calculi and algebraic models
• Growing need for analyzable concurrent behavior
• Determinism emerges as a core research theme
        │
        ▼

1978: CSP (Communicating Sequential Processes)
----------------------------------------------
• Tony Hoare introduces CSP
• Processes + channels + deterministic communication
• Strong mathematical semantics for reasoning
• Not executable — purely a formal model
        │
        ▼

1983–1990s: Occam and the Transputer Era
-----------------------------------------
• Occam brings CSP into a real programming language
• Deterministic concurrency on dedicated hardware
• Channels, parallel composition, static structure
• Occam‑π extends the model with mobility
• Limited by tooling and hardware constraints of the era
        │
        ▼

1990s–2000s: Verification & Synchronous Systems
-----------------------------------------------
• Promela/SPIN: explicit state machines + model checking
• Esterel/Lustre/SCADE: deterministic synchronous semantics
• Strong compile‑time guarantees for embedded and safety‑critical systems
• Reinforces the value of analyzable, predictable concurrency
        │
        ▼

2000s–2020s: Modern Systems Languages (C++, Rust, Go)
-----------------------------------------------------
• C++: zero‑cost abstractions, but nondeterministic concurrency
• Rust: memory safety, but async/await introduces hidden scheduling
• Go: goroutines + channels, but runtime‑scheduled and nondeterministic
• None provide deterministic, compile‑time‑verified concurrency
        │
        ▼

2020s: The Return of Determinism in Industry
--------------------------------------------
• Robotics, avionics, industrial automation demand provable behavior
• Developers hand‑roll FSMs, pipelines, and bounded queues in C/C++
• Growing recognition that nondeterministic concurrency is a liability
• Need for a modern, analyzable, deterministic systems language
        │
        ▼

CAPS: A Modern Deterministic Concurrency Language
-------------------------------------------------
• Explicit finite‑state machines
• Typed, bounded channels with compile‑time analysis
• Deterministic scheduling as a language feature
• Pipeline safety (`@pipeline_safe`) and topology verification
• Error‑propagation correctness
• Native machine‑code generation (no runtime, no GC)
• The first practical successor to CSP/Occam for modern systems

The One‑Sentence Summary
CAPS is the modern evolution of CSP and Occam — a deterministic concurrency language rebuilt for today’s systems, with compile‑time guarantees and native performance.

*****

