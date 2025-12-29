# Tutorial: Advanced Static Analyses

## Introduction
CAPS provides powerful static analyses to prove program properties at compile-time.

## Deadlock Detection
Use `@deadlock_free` to detect potential deadlocks.

```
@deadlock_free
group Safe {
  // Compiler checks for cycles in process dependencies
}
```

## Bounded Memory Proofs
Use `@bounded_memory` to prove memory limits.

```
@bounded_memory
group Limited {
  channel<int; 100> chan  // Proved bounded
}
```

## Channel Graph Analysis
Automatically analyzes communication graphs for bottlenecks.

## Process Lifecycle Verification
Use `@lifecycle_verified` to ensure termination.

```
@lifecycle_verified
process Worker -> () {
  // Must have terminal state
}
```

## Determinism Proofs
Use `@deterministic` to prove deterministic execution.

```
@deterministic
group Predictable {
  // No non-deterministic constructs
}
```

## Running Analyses
Use command-line flags:
- `--detect-deadlocks`
- `--prove-memory-bounds`
- `--verify-lifecycles`
- `--prove-determinism`