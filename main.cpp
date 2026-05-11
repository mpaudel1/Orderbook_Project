#include "OrderBook.h"

#include <iostream>
#include <vector>

int tests_passed = 0;
int tests_failed = 0;

void expect(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "[PASS] " << test_name << "\n";
        ++tests_passed;
    } else {
        std::cout << "[FAIL] " << test_name << "\n";
        ++tests_failed;
    }
}

void test_buy_fully_fills_one_sell() {
    OrderBook book;

    book.process_order(Order{1, SELL, 10100, 10, 1});

    std::vector<Trade> trades = book.process_order(Order{2, BUY, 10200, 10, 2});

    expect(trades.size() == 1, "BUY full fill: one trade");
    expect(trades[0].buy_order_id == 2, "BUY full fill: buy id correct");
    expect(trades[0].sell_order_id == 1, "BUY full fill: sell id correct");
    expect(trades[0].price == 10100, "BUY full fill: trade price correct");
    expect(trades[0].qty == 10, "BUY full fill: trade qty correct");
    expect(!book.best_ask(), "BUY full fill: ask removed");
    expect(!book.best_bid(), "BUY full fill: incoming buy did not rest");
}

void test_buy_partially_fills_one_sell() {
    OrderBook book;

    book.process_order(Order{1, SELL, 10100, 15, 1});

    std::vector<Trade> trades = book.process_order(Order{2, BUY, 10200, 10, 2});

    expect(trades.size() == 1, "BUY partial fill: one trade");
    expect(trades[0].sell_order_id == 1, "BUY partial fill: sell id correct");
    expect(trades[0].qty == 10, "BUY partial fill: trade qty correct");
    expect(book.best_ask() && *book.best_ask() == 10100, "BUY partial fill: ask still exists");
    expect(!book.best_bid(), "BUY partial fill: incoming buy fully filled");

    trades = book.process_order(Order{3, BUY, 10100, 5, 3});

    expect(trades.size() == 1, "BUY finishes remaining sell: one trade");
    expect(trades[0].sell_order_id == 1, "BUY finishes remaining sell: same sell order");
    expect(trades[0].qty == 5, "BUY finishes remaining sell: remaining qty correct");
    expect(!book.best_ask(), "BUY finishes remaining sell: ask removed");
}

void test_buy_consumes_multiple_sells_fifo() {
    OrderBook book;

    book.process_order(Order{10, SELL, 10100, 7, 1});
    book.process_order(Order{11, SELL, 10100, 5, 2});
    book.process_order(Order{12, SELL, 10100, 20, 3});

    std::vector<Trade> trades = book.process_order(Order{20, BUY, 10200, 20, 4});

    expect(trades.size() == 3, "BUY FIFO: three trades");
    expect(trades[0].sell_order_id == 10 && trades[0].qty == 7, "BUY FIFO: first sell filled first");
    expect(trades[1].sell_order_id == 11 && trades[1].qty == 5, "BUY FIFO: second sell filled second");
    expect(trades[2].sell_order_id == 12 && trades[2].qty == 8, "BUY FIFO: third sell partially filled");
    expect(book.best_ask() && *book.best_ask() == 10100, "BUY FIFO: ask level still exists");

    trades = book.process_order(Order{21, BUY, 10100, 12, 5});

    expect(trades.size() == 1, "BUY FIFO: remaining sell finished");
    expect(trades[0].sell_order_id == 12 && trades[0].qty == 12, "BUY FIFO: remaining sell qty correct");
    expect(!book.best_ask(), "BUY FIFO: ask level removed");
}

void test_sell_partially_fills_one_buy() {
    OrderBook book;

    book.process_order(Order{1, BUY, 10000, 15, 1});

    std::vector<Trade> trades = book.process_order(Order{2, SELL, 9900, 10, 2});

    expect(trades.size() == 1, "SELL partial fill: one trade");
    expect(trades[0].buy_order_id == 1, "SELL partial fill: buy id correct");
    expect(trades[0].sell_order_id == 2, "SELL partial fill: sell id correct");
    expect(trades[0].price == 10000, "SELL partial fill: trade price correct");
    expect(trades[0].qty == 10, "SELL partial fill: trade qty correct");
    expect(book.best_bid() && *book.best_bid() == 10000, "SELL partial fill: bid still exists");
    expect(!book.best_ask(), "SELL partial fill: incoming sell fully filled");

    trades = book.process_order(Order{3, SELL, 10000, 5, 3});

    expect(trades.size() == 1, "SELL finishes remaining buy: one trade");
    expect(trades[0].buy_order_id == 1, "SELL finishes remaining buy: same buy order");
    expect(trades[0].qty == 5, "SELL finishes remaining buy: remaining qty correct");
    expect(!book.best_bid(), "SELL finishes remaining buy: bid removed");
}

void test_sell_consumes_multiple_buys_fifo() {
    OrderBook book;

    book.process_order(Order{10, BUY, 10000, 7, 1});
    book.process_order(Order{11, BUY, 10000, 5, 2});
    book.process_order(Order{12, BUY, 10000, 20, 3});

    std::vector<Trade> trades = book.process_order(Order{20, SELL, 9900, 20, 4});

    expect(trades.size() == 3, "SELL FIFO: three trades");
    expect(trades[0].buy_order_id == 10 && trades[0].qty == 7, "SELL FIFO: first buy filled first");
    expect(trades[1].buy_order_id == 11 && trades[1].qty == 5, "SELL FIFO: second buy filled second");
    expect(trades[2].buy_order_id == 12 && trades[2].qty == 8, "SELL FIFO: third buy partially filled");
    expect(book.best_bid() && *book.best_bid() == 10000, "SELL FIFO: bid level still exists");

    trades = book.process_order(Order{21, SELL, 10000, 12, 5});

    expect(trades.size() == 1, "SELL FIFO: remaining buy finished");
    expect(trades[0].buy_order_id == 12 && trades[0].qty == 12, "SELL FIFO: remaining buy qty correct");
    expect(!book.best_bid(), "SELL FIFO: bid level removed");
}

void test_leftover_incoming_rests() {
    OrderBook book;

    book.process_order(Order{1, SELL, 10100, 5, 1});

    std::vector<Trade> trades = book.process_order(Order{2, BUY, 10100, 10, 2});

    expect(trades.size() == 1, "Leftover BUY rests: one trade");
    expect(trades[0].qty == 5, "Leftover BUY rests: trade qty correct");
    expect(!book.best_ask(), "Leftover BUY rests: ask removed");
    expect(book.best_bid() && *book.best_bid() == 10100, "Leftover BUY rests: leftover bid exists");
}

void test_non_matching_order_rests() {
    OrderBook book;

    std::vector<Trade> trades = book.process_order(Order{1, BUY, 10000, 10, 1});

    expect(trades.empty(), "Non-matching BUY rests: no trades");
    expect(book.best_bid() && *book.best_bid() == 10000, "Non-matching BUY rests: best bid exists");
    expect(!book.best_ask(), "Non-matching BUY rests: no ask exists");
}

void test_duplicate_incoming_id_rejected() {
    OrderBook book;

    book.process_order(Order{1, BUY, 10000, 5, 1});

    std::vector<Trade> trades = book.process_order(Order{1, SELL, 9000, 5, 2});

    expect(trades.empty(), "Duplicate ID rejected: no trades");
    expect(book.best_bid() && *book.best_bid() == 10000, "Duplicate ID rejected: original bid still exists");
    expect(!book.best_ask(), "Duplicate ID rejected: duplicate sell did not rest");
}

int main() {
    test_buy_fully_fills_one_sell();
    test_buy_partially_fills_one_sell();
    test_buy_consumes_multiple_sells_fifo();
    test_sell_partially_fills_one_buy();
    test_sell_consumes_multiple_buys_fifo();
    test_leftover_incoming_rests();
    test_non_matching_order_rests();
    test_duplicate_incoming_id_rejected();

    std::cout << "\nTests passed: " << tests_passed << "\n";
    std::cout << "Tests failed: " << tests_failed << "\n";

    return tests_failed == 0 ? 0 : 1;
}