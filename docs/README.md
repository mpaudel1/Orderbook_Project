# Orderbook Project

A C++ limit order book and matching engine focused on correctness and clear systems design.

## Features

- Limit order support 
- Market order support 
- Buy and sell order books 
- Price-time priority
- FIFO matching within each price level
- Best bid / best ask lookup
- Multi-price matching 
- Partial fills
- Order cancellation by ID
- Trade generation
- Duplicate order ID rejection
- Event replay from `events.txt`
- CSV trade logging to `trades.csv`
- Invariant checks for internal book correctness 
- Separate test and benchmark targets 

## Build

```bash
make
```

## Run Event Replay

```bash
make run
```

Reads events from `events.txt` and writes trades to `trades.csv`

## Event Format

Supported replay commands:

```
LIMIT order_id side price qty timestamp
MARKET order_id side qty timestamp
CANCEL order_id
```

Supported sides:

```
BUY
SELL
```

## Run Tests

```bash
make test
```

The test suite covers:

- full fills
- partial fills
- multi-price matching
- market orders
- limit orders
- cancellation
- duplicate ID rejection
- empty-book behavior
- invariant checks

## Run Benchmarks 

```bash
make bench
```

Current benchmark scenarios:

- Add-only benchmark
- Market-match benchmark
- Cancel benchmark 

Benchmark results vary by machine, compiler flags, and build settings.

## Useful Commands 

```bash
make run    # run replay mode 
make test   # run tests 
make bench  # run benchmarks 
make clean  # remove generated build files 
```

## Status

Current version implements the basic matching engine.

Planned future work:

- modify / replace orders
- better parse error messages 
- support for comments and blank lines in replay files
- optimized benchmark build flags 
- project restructuring into `include/`, `src/`, and `tests/`
- more realistic exchange-style event messages 
- deeper performance optimization 

