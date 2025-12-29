# Tutorial: Your First Process

## Introduction
In this tutorial, we'll create a simple process that counts to 10.

## Code
```
module counter

group Counter {
  process Count -> () {
    state Start, Loop, Done
    var i:int = 0

    on Start { -> Loop }
    on Loop {
      do i = i + 1
      if i < 10 { -> Loop } else { -> Done }
    }
    on Done { -> Done }
  }

  schedule { step Count; repeat }
}
```

## Explanation
- `process Count -> ()`: Process with no inputs/outputs.
- States: Start, Loop, Done.
- Transitions: Conditional based on `i`.

## Running
Compile and run as usual. The process counts to 10 and terminates.