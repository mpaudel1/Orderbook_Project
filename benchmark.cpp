#include "OrderBook.h"

#include <chrono>
#include <iostream>

void run_add_only_benchmark() {
    OrderBook book {};

    constexpr int resting_orders = 1'000'000;
    constexpr int total_events = resting_orders;

    auto start = std::chrono::steady_clock::now();

    for (int i { 0 }; i < resting_orders; ++i) {
        OrderId id {i};
        Side side {Side::SELL};
        Type type {Type::LIMIT};
        Price price {10'000 + (i % 100)};
        Qty qty {1};
        Timestamp timestamp = i;

        Order order {
            id, side, type, price, qty, timestamp
        };

        book.process_order(order);
    }
    
    auto end = std::chrono::steady_clock::now();

    auto elapsed_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (!book.check_invariants()) {
        std::cout << "Invariant check failed!\n";
        return; 
    }

    double elapsed_s = elapsed_ms.count() / 1000.0;
    double events_per_s = total_events / elapsed_s;

    std::cout << "Add-only Benchmark\n";
    std::cout << "Processed: " << total_events << " events\n";
    std::cout << "Time: " << elapsed_ms.count() << " ms\n";
    std::cout << "Throughput: " << events_per_s << " events/sec\n\n";
}

void run_market_match_benchmark() {
    OrderBook book {};

    constexpr int resting_orders = 500'000;
    constexpr int market_orders = 500'000;
    constexpr int total_events = resting_orders + market_orders;

    auto start = std::chrono::steady_clock::now();

    for (int i { 0 }; i < resting_orders; ++i) {
        OrderId id {i};
        Side side {Side::SELL};
        Type type {Type::LIMIT};
        Price price {10'000 + (i % 100)};
        Qty qty {1};
        Timestamp timestamp = i;

        Order order {
            id, side, type, price, qty, timestamp
        };

        book.process_order(order);
    }

    for (int i { 0 }; i < market_orders; ++i) {
        OrderId id {resting_orders + i};
        Side side {Side::BUY};
        Type type {Type::MARKET};
        Price price {0};
        Qty qty {1};
        Timestamp timestamp = resting_orders + i;

        Order order {
            id, side, type, price, qty, timestamp
        };

        book.process_order(order);
    }

    auto end = std::chrono::steady_clock::now();

    auto elapsed_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (!book.check_invariants()) {
        std::cout << "Invariant check failed!\n";
        return; 
    }

    double elapsed_s = elapsed_ms.count() / 1000.0;
    double events_per_s = total_events / elapsed_s;

    std::cout << "Market-match Benchmark\n";
    std::cout << "Processed: " << total_events << " events\n";
    std::cout << "Time: " << elapsed_ms.count() << " ms\n";
    std::cout << "Throughput: " << events_per_s << " events/sec\n\n";
}

void run_cancel_benchmark() {
    OrderBook book {};

    constexpr int resting_orders = 1'000'000;
    constexpr int total_events = resting_orders;

    for (int i { 0 }; i < resting_orders; ++i) {
        OrderId id {i};
        Side side {Side::SELL};
        Type type {Type::LIMIT};
        Price price {10'000 + (i % 100)};
        Qty qty {1};
        Timestamp timestamp = i;

        Order order {
            id, side, type, price, qty, timestamp
        };

        book.process_order(order);
    }

    auto start = std::chrono::steady_clock::now();
    
    for (int i { 0 }; i < resting_orders; ++i) {
        book.cancel_order(i);
    }

    auto end = std::chrono::steady_clock::now();

    auto elapsed_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (!book.check_invariants()) {
        std::cout << "Invariant check failed!\n";
        return; 
    }

    double elapsed_s = elapsed_ms.count() / 1000.0;
    double events_per_s = total_events / elapsed_s;

    std::cout << "Cancel Benchmark\n";
    std::cout << "Processed: " << total_events << " cancels\n";
    std::cout << "Time: " << elapsed_ms.count() << " ms\n";
    std::cout << "Throughput: " << events_per_s << " cancels/sec\n\n";
}

int main() {
    run_add_only_benchmark();
    run_market_match_benchmark();
    run_cancel_benchmark();

    return 0;
}