# Tutorial: Compiler Optimizations

## Introduction
CAPS includes advanced optimizations for performance.

## Dead Code Elimination
Removes unused code.

```
@optimize
group Optimized {
  // Unused processes removed
}
```

## Channel Fusion
Merges compatible channels.

```
channel<int; 10> a
channel<int; 10> b
// Fused into single channel
```

## Process Inlining
Inlines small processes.

```
process Small -> () {
  // Inlined if small
}
```

## Static Memory Layout
Pre-allocates memory.

```
@static_memory
group Static {
  // Memory pre-allocated
}
```

## Zero-Copy Message Passing
Passes messages without copying.

```
@zero_copy
group ZeroCopy {
  // Messages passed by reference
}
```

## Enabling Optimizations
Use `--optimize-aggressive` or specific flags.