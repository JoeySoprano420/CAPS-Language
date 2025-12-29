# Tutorial: Channels and Communication

## Introduction
Learn how to use channels for inter-process communication.

## Producer-Consumer Example
```
module producer_consumer

group PC {
  channel<int; 10> data

  process Producer -> () {
    state Send
    var val:int = 1

    on Send {
      do try_send val -> data
      do val = val + 1
      -> Send
    }
  }

  process Consumer -> () {
    state Receive
    var sum:int = 0

    on Receive {
      do rr = try_receive data
      do v:int = rr?
      do sum = sum + v
      -> Receive
    }
  }

  schedule { step Producer; step Consumer; repeat }
}
```

## Key Points
- Channels are typed and bounded.
- `try_send`/`try_receive` for non-blocking ops.
- Ownership transfer ensures safety.