# Tutorial: New Language Features

## Pattern Matching
Use `match` for type-safe branching.

```
match value {
  0 => { /* handle zero */ }
  n if n > 10 => { /* handle large */ }
  _ => { /* default */ }
}
```

## Static Arrays
Fixed-size arrays known at compile-time.

```
var arr:[int; 5] = [1, 2, 3, 4, 5]
```

## Deterministic Loops
Bounded loops with compile-time end.

```
for i in 0..10 {
  // Loop body
}
```

## Compile-Time Functions
Functions evaluated at compile-time.

```
const fn square(x:int) -> int {
  x * x
}
```