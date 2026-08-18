#include "OrderBook.h"

#include <iostream>
#include <optional>
#include <string>
#include <vector>

int tests_passed = 0;
int tests_failed = 0;

void expect(bool condition, const std::string& test_name) {
    if (condition) {
        ++tests_passed;
        std::cout << "[PASS] " << test_name << '\n';
    } else {
        ++tests_failed;
        std::cout << "[FAIL] " << test_name << '\n';
    }
}

void expect_invariants(OrderBook& book, const std::string& test_name) {
    expect(book.check_invariants(), test_name + " invariants");
}

std::vector<Trade> process_and_check(
    OrderBook& book,
    const Order& order,
    const std::string& test_name
) {
    std::vector<Trade> trades = book.process_order(order);
    expect_invariants(book, test_name);
    return trades;
}

void expect_price(
    const std::optional<Price>& actual,
    Price expected,
    const std::string& test_name
) {
    expect(actual.has_value() && *actual == expected, test_name);
}

void expect_no_price(
    const std::optional<Price>& actual,
    const std::string& test_name
) {
    expect(!actual.has_value(), test_name);
}

void expect_trade(
    const std::vector<Trade>& trades,
    std::size_t index,
    OrderId expected_buy_id,
    OrderId expected_sell_id,
    Price expected_price,
    Qty expected_qty,
    Timestamp expected_timestamp,
    const std::string& test_name
) {
    expect(index < trades.size(), test_name + " trade exists");

    if (index >= trades.size()) {
        return;
    }

    const Trade& trade = trades[index];

    expect(trade.buy_order_id == expected_buy_id, test_name + " buy id");
    expect(trade.sell_order_id == expected_sell_id, test_name + " sell id");
    expect(trade.price == expected_price, test_name + " price");
    expect(trade.qty == expected_qty, test_name + " qty");
    expect(trade.timestamp == expected_timestamp, test_name + " timestamp");
}

void test_limit_buy_fully_fills_one_sell() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 10000, 10, 1},
        "limit buy full fill - add sell"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 10000, 10, 2},
        "limit buy full fill - process buy"
    );

    expect(trades.size() == 1, "limit buy full fill trade count");
    expect_trade(trades, 0, 2, 1, 10000, 10, 2, "limit buy full fill");

    expect_no_price(book.best_bid(), "limit buy full fill no best bid");
    expect_no_price(book.best_ask(), "limit buy full fill no best ask");
}

void test_limit_buy_partially_fills_one_sell() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 10000, 10, 1},
        "limit buy partial fill - add sell"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 10000, 4, 2},
        "limit buy partial fill - process buy"
    );

    expect(trades.size() == 1, "limit buy partial fill trade count");
    expect_trade(trades, 0, 2, 1, 10000, 4, 2, "limit buy partial fill");

    expect_no_price(book.best_bid(), "limit buy partial fill no best bid");
    expect_price(book.best_ask(), 10000, "limit buy partial fill best ask remains");

    std::vector<Trade> cleanup_trades = process_and_check(
        book,
        Order{3, Side::BUY, Type::LIMIT, 10000, 6, 3},
        "limit buy partial fill - cleanup"
    );

    expect(cleanup_trades.size() == 1, "limit buy partial fill cleanup trade count");
    expect_trade(cleanup_trades, 0, 3, 1, 10000, 6, 3, "limit buy partial fill cleanup");

    expect_no_price(book.best_ask(), "limit buy partial fill ask removed after cleanup");
}

void test_limit_sell_partially_fills_one_buy() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::BUY, Type::LIMIT, 10000, 10, 1},
        "limit sell partial fill - add buy"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{2, Side::SELL, Type::LIMIT, 10000, 4, 2},
        "limit sell partial fill - process sell"
    );

    expect(trades.size() == 1, "limit sell partial fill trade count");
    expect_trade(trades, 0, 1, 2, 10000, 4, 2, "limit sell partial fill");

    expect_price(book.best_bid(), 10000, "limit sell partial fill best bid remains");
    expect_no_price(book.best_ask(), "limit sell partial fill no best ask");

    std::vector<Trade> cleanup_trades = process_and_check(
        book,
        Order{3, Side::SELL, Type::LIMIT, 10000, 6, 3},
        "limit sell partial fill - cleanup"
    );

    expect(cleanup_trades.size() == 1, "limit sell partial fill cleanup trade count");
    expect_trade(cleanup_trades, 0, 1, 3, 10000, 6, 3, "limit sell partial fill cleanup");

    expect_no_price(book.best_bid(), "limit sell partial fill bid removed after cleanup");
}

void test_limit_non_matching_orders_rest() {
    OrderBook book;

    std::vector<Trade> sell_trades = process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 11000, 5, 1},
        "limit non-matching - add sell"
    );

    std::vector<Trade> buy_trades = process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 10000, 5, 2},
        "limit non-matching - add buy"
    );

    expect(sell_trades.empty(), "limit non-matching sell rests");
    expect(buy_trades.empty(), "limit non-matching buy rests");

    expect_price(book.best_bid(), 10000, "limit non-matching best bid");
    expect_price(book.best_ask(), 11000, "limit non-matching best ask");
}

void test_limit_multi_price_buy_matching() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 10100, 5, 1},
        "limit multi-price buy - add ask 10100"
    );

    process_and_check(
        book,
        Order{2, Side::SELL, Type::LIMIT, 10200, 10, 2},
        "limit multi-price buy - add ask 10200"
    );

    process_and_check(
        book,
        Order{3, Side::SELL, Type::LIMIT, 10300, 20, 3},
        "limit multi-price buy - add ask 10300"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{4, Side::BUY, Type::LIMIT, 10200, 12, 4},
        "limit multi-price buy - process buy"
    );

    expect(trades.size() == 2, "limit multi-price buy trade count");
    expect_trade(trades, 0, 4, 1, 10100, 5, 4, "limit multi-price buy first trade");
    expect_trade(trades, 1, 4, 2, 10200, 7, 4, "limit multi-price buy second trade");

    expect_price(book.best_ask(), 10200, "limit multi-price buy best ask remains 10200");
    expect_no_price(book.best_bid(), "limit multi-price buy no bid rests");
}

void test_limit_multi_price_sell_matching() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::BUY, Type::LIMIT, 10000, 5, 1},
        "limit multi-price sell - add bid 10000"
    );

    process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 9900, 10, 2},
        "limit multi-price sell - add bid 9900"
    );

    process_and_check(
        book,
        Order{3, Side::BUY, Type::LIMIT, 9800, 20, 3},
        "limit multi-price sell - add bid 9800"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{4, Side::SELL, Type::LIMIT, 9900, 12, 4},
        "limit multi-price sell - process sell"
    );

    expect(trades.size() == 2, "limit multi-price sell trade count");
    expect_trade(trades, 0, 1, 4, 10000, 5, 4, "limit multi-price sell first trade");
    expect_trade(trades, 1, 2, 4, 9900, 7, 4, "limit multi-price sell second trade");

    expect_price(book.best_bid(), 9900, "limit multi-price sell best bid remains 9900");
    expect_no_price(book.best_ask(), "limit multi-price sell no ask rests");
}

void test_duplicate_incoming_id_rejected() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 10000, 5, 1},
        "duplicate id - add sell"
    );

    std::vector<Trade> duplicate_trades = process_and_check(
        book,
        Order{1, Side::BUY, Type::LIMIT, 10000, 5, 2},
        "duplicate id - reject duplicate"
    );

    expect(duplicate_trades.empty(), "duplicate id produces no trades");
    expect_price(book.best_ask(), 10000, "duplicate id original ask remains");

    std::vector<Trade> valid_trades = process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 10000, 5, 3},
        "duplicate id - valid order fills"
    );

    expect(valid_trades.size() == 1, "duplicate id valid order trade count");
    expect_trade(valid_trades, 0, 2, 1, 10000, 5, 3, "duplicate id valid order trade");
}

void test_cancel_order_removes_resting_order() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::BUY, Type::LIMIT, 10000, 5, 1},
        "cancel order - add first buy"
    );

    process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 10000, 5, 2},
        "cancel order - add second buy"
    );

    book.cancel_order(1);
    expect_invariants(book, "cancel order - after cancel");

    std::vector<Trade> trades = process_and_check(
        book,
        Order{3, Side::SELL, Type::MARKET, 0, 5, 3},
        "cancel order - market sell after cancel"
    );

    expect(trades.size() == 1, "cancel order trade count");
    expect_trade(trades, 0, 2, 3, 10000, 5, 3, "cancel order remaining order fills");

    expect_no_price(book.best_bid(), "cancel order no best bid");
    expect_no_price(book.best_ask(), "cancel order no best ask");
}

void test_market_buy_fills_across_asks() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 10100, 5, 1},
        "market buy across asks - add ask 10100"
    );

    process_and_check(
        book,
        Order{2, Side::SELL, Type::LIMIT, 10200, 10, 2},
        "market buy across asks - add ask 10200"
    );

    process_and_check(
        book,
        Order{3, Side::SELL, Type::LIMIT, 10300, 20, 3},
        "market buy across asks - add ask 10300"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{4, Side::BUY, Type::MARKET, 0, 20, 4},
        "market buy across asks - process market buy"
    );

    expect(trades.size() == 3, "market buy across asks trade count");
    expect_trade(trades, 0, 4, 1, 10100, 5, 4, "market buy across asks first trade");
    expect_trade(trades, 1, 4, 2, 10200, 10, 4, "market buy across asks second trade");
    expect_trade(trades, 2, 4, 3, 10300, 5, 4, "market buy across asks third trade");

    expect_price(book.best_ask(), 10300, "market buy across asks remaining ask price");
    expect_no_price(book.best_bid(), "market buy across asks no bid rests");

    std::vector<Trade> cleanup_trades = process_and_check(
        book,
        Order{5, Side::BUY, Type::MARKET, 0, 15, 5},
        "market buy across asks - cleanup"
    );

    expect(cleanup_trades.size() == 1, "market buy across asks cleanup trade count");
    expect_trade(cleanup_trades, 0, 5, 3, 10300, 15, 5, "market buy across asks cleanup trade");

    expect_no_price(book.best_ask(), "market buy across asks no ask after cleanup");
}

void test_market_buy_insufficient_liquidity_does_not_rest() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::SELL, Type::LIMIT, 10100, 5, 1},
        "market buy insufficient - add ask"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{2, Side::BUY, Type::MARKET, 0, 12, 2},
        "market buy insufficient - process market buy"
    );

    expect(trades.size() == 1, "market buy insufficient trade count");
    expect_trade(trades, 0, 2, 1, 10100, 5, 2, "market buy insufficient trade");

    expect_no_price(book.best_bid(), "market buy insufficient leftover does not rest as bid");
    expect_no_price(book.best_ask(), "market buy insufficient ask side empty");
}

void test_market_sell_fills_across_bids() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::BUY, Type::LIMIT, 10000, 5, 1},
        "market sell across bids - add bid 10000"
    );

    process_and_check(
        book,
        Order{2, Side::BUY, Type::LIMIT, 9900, 10, 2},
        "market sell across bids - add bid 9900"
    );

    process_and_check(
        book,
        Order{3, Side::BUY, Type::LIMIT, 9800, 20, 3},
        "market sell across bids - add bid 9800"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{4, Side::SELL, Type::MARKET, 0, 12, 4},
        "market sell across bids - process market sell"
    );

    expect(trades.size() == 2, "market sell across bids trade count");
    expect_trade(trades, 0, 1, 4, 10000, 5, 4, "market sell across bids first trade");
    expect_trade(trades, 1, 2, 4, 9900, 7, 4, "market sell across bids second trade");

    expect_price(book.best_bid(), 9900, "market sell across bids remaining bid price");
    expect_no_price(book.best_ask(), "market sell across bids no ask rests");

    std::vector<Trade> cleanup_trades = process_and_check(
        book,
        Order{5, Side::SELL, Type::MARKET, 0, 3, 5},
        "market sell across bids - cleanup"
    );

    expect(cleanup_trades.size() == 1, "market sell across bids cleanup trade count");
    expect_trade(cleanup_trades, 0, 2, 5, 9900, 3, 5, "market sell across bids cleanup trade");

    expect_price(book.best_bid(), 9800, "market sell across bids next best bid after cleanup");
}

void test_market_sell_insufficient_liquidity_does_not_rest() {
    OrderBook book;

    process_and_check(
        book,
        Order{1, Side::BUY, Type::LIMIT, 10000, 5, 1},
        "market sell insufficient - add bid"
    );

    std::vector<Trade> trades = process_and_check(
        book,
        Order{2, Side::SELL, Type::MARKET, 0, 12, 2},
        "market sell insufficient - process market sell"
    );

    expect(trades.size() == 1, "market sell insufficient trade count");
    expect_trade(trades, 0, 1, 2, 10000, 5, 2, "market sell insufficient trade");

    expect_no_price(book.best_bid(), "market sell insufficient bid side empty");
    expect_no_price(book.best_ask(), "market sell insufficient leftover does not rest as ask");
}

void test_market_order_on_empty_book_does_not_rest() {
    OrderBook book;

    std::vector<Trade> buy_trades = process_and_check(
        book,
        Order{1, Side::BUY, Type::MARKET, 0, 10, 1},
        "market empty book - buy"
    );

    expect(buy_trades.empty(), "market empty book buy no trades");
    expect_no_price(book.best_bid(), "market empty book buy does not rest bid");
    expect_no_price(book.best_ask(), "market empty book buy no ask");

    std::vector<Trade> sell_trades = process_and_check(
        book,
        Order{2, Side::SELL, Type::MARKET, 0, 10, 2},
        "market empty book - sell"
    );

    expect(sell_trades.empty(), "market empty book sell no trades");
    expect_no_price(book.best_bid(), "market empty book sell no bid");
    expect_no_price(book.best_ask(), "market empty book sell does not rest ask");
}

int main() {
    test_limit_buy_fully_fills_one_sell();
    test_limit_buy_partially_fills_one_sell();
    test_limit_sell_partially_fills_one_buy();
    test_limit_non_matching_orders_rest();
    test_limit_multi_price_buy_matching();
    test_limit_multi_price_sell_matching();
    test_duplicate_incoming_id_rejected();
    test_cancel_order_removes_resting_order();

    test_market_buy_fills_across_asks();
    test_market_buy_insufficient_liquidity_does_not_rest();
    test_market_sell_fills_across_bids();
    test_market_sell_insufficient_liquidity_does_not_rest();
    test_market_order_on_empty_book_does_not_rest();

    std::cout << "\nTests passed: " << tests_passed << '\n';
    std::cout << "Tests failed: " << tests_failed << '\n';

    if (tests_failed == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }

    return 1;
}