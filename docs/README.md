# Orderbook Project

A C++ limit order book and matching engine focused on correctness and clear systems design.

## Features

- Resting limit orders
- Buy and sell price levels
- Price-time priority
- FIFO matching within a price level
- Best bid / best ask lookup
- Partial fills
- Order cancellation by ID
- Trade generation
- Duplicate order ID rejection

## Build and Run

```bash
make
make run
make clean
```

## Status

Current version implements the basic matching engine.

Planned future work:

- event replay
- market orders
- modify / replace orders
- benchmark tests
- project restructuring
