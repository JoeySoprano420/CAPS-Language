# Tutorial: Clean, Deterministic Type System

## Generics
Parameterize types for reusability.

```
struct Pair<T, U> { first: T, second: U }
let p: Pair<int, text> = {first: 42, second: "hello"}
```

## Dependent Types
Types that depend on values.

```
fn array_len<T, N>(arr: [T; N]) -> int { N }  // N is compile-time
```

## Refinement Types
Subtypes with predicates.

```
fn positive(x: {int | x > 0}) -> int { x }  // Only positive ints
```

## Union Types
Sum types for variants.

```
union Value { Int(int), Text(text) }
let v = Value::Int(42)
```

## Ownership and Borrowing
Automatic memory management.

```
fn borrow(v: &int) -> int { *v }  // Borrowing
fn move(v: int) { }  // Ownership transfer
```

## Type Inference
Automatic type deduction.

```
let x = 42;  // Inferred as int
let y = [1, 2, 3];  // Inferred as [int; 3]
```