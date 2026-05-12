# kv_store

A Redis-compatible key-value store server written in C++20. Speaks the [RESP protocol](https://redis.io/docs/reference/protocol-spec/), so any Redis client works out of the box.

## Architecture

The server is single-threaded with a non-blocking event loop — the same model Redis uses. The layers are cleanly separated:

```
┌─────────────────────────────────────────┐
│              Application                │  top-level orchestrator
├─────────────┬───────────────────────────┤
│   Network   │          Core             │
│  ─────────  │  ───────────────────────  │
│  Reactor    │  Controller               │
│  Epoll      │  CommandDispatcher        │
│  Listener   │  EventBus                 │
│  Connection │  Session                  │
│  RespParser │                           │
│  RespValue  │                           │
├─────────────┴───────────────────────────┤
│               Storage                   │  hash map + TTL
└─────────────────────────────────────────┘
```

**Network layer** — `Reactor` owns the epoll loop and manages connections. `RespParser` is an incremental state machine that feeds one byte at a time and emits a complete `RespValue` when a message is fully received.

**Core layer** — `Controller` routes incoming RESP arrays to `CommandDispatcher`, which looks up the registered `Command` object by name, parses arguments, and executes. `Session` represents a connected client and abstracts away the send/disconnect callbacks. `EventBus` is a typed pub/sub bus for internal events.

**Storage** — An `std::unordered_map<string, Entry>` where each entry holds a `std::variant<string, StringList>` plus an optional expiration timestamp (milliseconds since epoch). Expiry is checked lazily on access.

## Supported commands

| Command | Syntax | Notes |
|---------|--------|-------|
| `GET` | `GET key` | Returns bulk string or nil |
| `SET` | `SET key value [EX s] [PX ms] [KEEPTTL] [NX\|XX]` | Full option set |
| `LPUSH` | `LPUSH key value [value ...]` | Prepends; returns new length |
| `RPUSH` | `RPUSH key value [value ...]` | Appends; returns new length |
| `LLEN` | `LLEN key` | Returns 0 for missing keys |
| `LRANGE` | `LRANGE key start stop` | Negative indices supported |

## Building

Requires GCC with C++20 support and [spdlog](https://github.com/gabime/spdlog) installed as a sibling directory (`../spdlog`).

```bash
# build the server
make

# build and run tests (Catch2)
make run_tests
```

The server binary is `program.out` and listens on port **5555**.

## Running

```bash
./program.out
```

Connect with any Redis client:

```bash
redis-cli -p 5555
```

## Benchmarking

The included script builds the server, starts it, runs `redis-benchmark`, and kills the process when done:

```bash
./run-benchmark.sh
```

Default benchmark: 400,000 SET operations, 100 concurrent connections.

## Project structure

```
src/
├── main.cpp
├── core/
│   ├── Application.{hpp,cpp}       # wires reactor ↔ sessions ↔ controller
│   ├── Controller.{hpp,cpp}        # routes commands, owns EventBus
│   ├── Session.{hpp,cpp}           # per-connection state
│   ├── command/
│   │   ├── Command.{hpp,cpp}       # base class with parse/execute template
│   │   ├── CommandDispatcher.{hpp,cpp}
│   │   ├── parsing_helpers.hpp     # flag parsing utilities
│   │   ├── string/                 # GET, SET
│   │   └── list/                   # LPUSH, RPUSH, LLEN, LRANGE
│   └── event/
│       ├── EventBus.{hpp,cpp}
│       └── Subscription.{hpp,cpp}
├── network/
│   ├── Reactor.{hpp,cpp}           # event loop, connection lifetime
│   ├── Epoll.{hpp,cpp}             # Linux epoll wrapper
│   ├── Listener.{hpp,cpp}          # TCP accept loop
│   ├── Connection.{hpp,cpp}        # per-fd read/write buffers
│   └── resp/
│       ├── RespParser.{hpp,cpp}    # incremental RESP parser
│       └── RespValue.{hpp,cpp}     # typed RESP value + encoder
└── storage/
    ├── Storage.{hpp,cpp}           # hash map with TTL
    └── utils.hpp

tests/
├── controller/event/EventBus_test.cpp
└── network/resp/RespParser_test.cpp
                    RespValue_test.cpp
```
