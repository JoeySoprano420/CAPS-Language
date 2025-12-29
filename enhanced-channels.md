# Tutorial: Enhanced Channels

## Introduction
CAPS supports advanced channel types for complex communication.

## Priority Channels
Channels that prioritize messages.

```
priority_channel<int; 10> pri_chan

process Sender -> () {
  on Send {
    do priority_send 1 -> pri_chan  // High priority
    do send 2 -> pri_chan  // Normal
    -> Send
  }
}
```

## Timeout Channels
Channels with automatic expiration.

```
timeout_channel<int; 10; 1000> timeout_chan

process Receiver -> () {
  on Receive {
    do rr = try_receive timeout_chan
    // Handle timeout
    -> Receive
  }
}
```