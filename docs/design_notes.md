# Design Notes

## Project Goal

This is a c++ limit order book and matching engine. Current version focus is on correctness, then later I will move towards optimizations.

---

## Current Features

- Resting limit orders
- Buy and sell sides
- Price levels
- FIFO ordering within each price level
- Fast lookup by order id
- Order cancellation
- Best bid / best ask lookup
- Incoming matching order
- Partial fills
- Multi-order FIFO fills
- Leftover quantity resting in the book
- Trade record generation
- Duplicate order ID rejection

---

## Core Objects

### 'Order'

Represents one order

Fields:

- id
- side -> BUY | SELL
- price
- qty
- timestamp

### 'Limit'

Represents one price level.

Fields:

- price
- num_orders
- total_volume
- resting_orders (std::list)

---

### 'Trade'

Represents one execution between an incoming order and a resting order.

Fields:

- buy_order_id
- sell_order_id
- price
- qty
- timestamp

---

### 'Orderbook'

Owns the full book.

- bid price levels
- ask price levels
- order id lookup

## Data Structures

### Price Levels

```cpp
std::map<Price, Limit>
```

Used for bids and asks.

### FIFO Orders

```cpp
std::unordered_map<OrderId, Location>
```

Maps an order id to where that order lives in the book. 

### OrderLocation

fields:

- side
- price 
- order_it (iterator to the order in the list)

This allows fast cancels without scanning the book.

