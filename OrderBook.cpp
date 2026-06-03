#include "OrderBook.h"

#include <iostream>
#include <iterator>
#include <stdexcept>

void OrderBook::add_order(const Order & order)
{
    auto found = orderid_lookup.find(order.id);
    if (found != orderid_lookup.end()) {
        std::cout << "Order with id " << order.id << " already exists in the order book." << std::endl;
        return;
    }

    if (order.side == Side::BUY) {
        Limit &level = bids[order.price];
        level.price = order.price;
        level.num_orders += 1;
        level.total_volume += order.qty;
        level.resting_orders.push_back(order);

        OrderIterator it = std::prev(level.resting_orders.end());
        orderid_lookup[order.id] = {order.side, order.price, it};
    } else if (order.side == Side::SELL) {
        Limit &level = asks[order.price];
        level.price = order.price;
        level.num_orders += 1;
        level.total_volume += order.qty;
        level.resting_orders.push_back(order);

        OrderIterator it = std::prev(level.resting_orders.end());
        orderid_lookup[order.id] = {order.side, order.price, it};
    } else {
        std::cout << "Invalid order side for order with id " << order.id << "." << std::endl;
        return;
    }
}

void OrderBook::cancel_order(OrderId orderId) {
    auto found = orderid_lookup.find(orderId);
    if (found == orderid_lookup.end()) {
        std::cout << "Order with id" << orderId << " does not exist in the order book." << std::endl;
        return;
    }

    OrderLocation order_loc = found->second;

    if (order_loc.side == Side::BUY) {
        auto level_it = bids.find(order_loc.price);
        if (level_it == bids.end()) {
            std::cout << "Limit related to " << orderId << " does not exist in bids." << std::endl;
            return;
        }

        Limit &level = level_it->second;

        Qty qty = order_loc.order_it->qty;

        level.resting_orders.erase(order_loc.order_it);

        --level.num_orders;
        level.total_volume -= qty;

        if (level.resting_orders.empty()) {
            bids.erase(level_it);
        }

        orderid_lookup.erase(orderId);
    } else if (order_loc.side == Side::SELL) {
        auto level_it = asks.find(order_loc.price);
        if (level_it == asks.end()) {
            std::cout << "Limit related to " << orderId << " does not exist in asks." << std::endl;
            return;
        }

        Limit &level = level_it->second;

        Qty qty = order_loc.order_it->qty;

        level.resting_orders.erase(order_loc.order_it);

        --level.num_orders;
        level.total_volume -= qty;

        if (level.resting_orders.empty()) {
            asks.erase(level_it);
        }

        orderid_lookup.erase(orderId);
    } else {
        std::cout << "Invalid order side for order with id " << orderId << "." << std::endl;
        return;
    }
}


void OrderBook::print_book() {
    std::cout << "BIDS:\n";

    for (auto price_level{bids.begin()}; price_level != bids.end(); ++price_level) {
        std::cout << "Price: "<< price_level->first << "\n";

        Limit &level = price_level->second;
        for (auto order_it {level.resting_orders.begin()}; order_it != level.resting_orders.end(); ++order_it) {
            std::cout << "Order ID: " << order_it->id << "\n";
            std::cout << "Quantity: " << order_it->qty << "\n";
        }
    }

    std::cout << "ASKS:\n";

    for (auto price_level{asks.begin()}; price_level != asks.end(); ++price_level) {
        std::cout << "Price: "<< price_level->first << "\n";

        Limit &level = price_level->second;
        for (auto order_it {level.resting_orders.begin()}; order_it != level.resting_orders.end(); ++order_it) {
            std::cout << "Order ID: " << order_it->id << "\n";
            std::cout << "Quantity: " << order_it->qty << "\n";
        }
    }

    std::cout << "\n";
}

std::optional<Price> OrderBook::best_bid() {
    if (bids.empty()) {
        return {};
    } else {
        auto it = bids.rbegin();
        return it->first;
    }
}

std::optional<Price> OrderBook::best_ask() {
    if (asks.empty()) {
        return {};
    } else {
        auto it = asks.begin();
        return it->first;
    }
}

bool OrderBook::would_match(const Order & order) {
    if (order.side == Side::BUY) {
        auto best_ask_r = best_ask();
        if (!best_ask_r) return false;

        return order.price >= *best_ask_r;

    } else if (order.side == Side::SELL) {
        auto best_bid_r = best_bid();
        if (!best_bid_r) return false;

        return order.price <= *best_bid_r;
    } 

    return false;
} 

std::vector<Trade> OrderBook::process_order(const Order & order) {
    Order incoming = order;
    std::vector<Trade> trades {};

    auto found = orderid_lookup.find(order.id);
    if (found != orderid_lookup.end()) return trades;

    while (incoming.qty > 0  && would_match(incoming)) {
        if (incoming.side == BUY) {
            auto best_ask_r = best_ask();

            auto ask_it = asks.find(*best_ask_r);
            Limit &level = ask_it->second;

            auto resting_order = level.resting_orders.begin();
            Qty trade_qty = (incoming.qty < resting_order->qty) ? incoming.qty : resting_order->qty;

            Trade record = {incoming.id, resting_order->id, resting_order->price, trade_qty, incoming.timestamp};

            incoming.qty            -= trade_qty;
            resting_order->qty      -= trade_qty;
            level.total_volume      -= trade_qty;

            if (resting_order->qty == 0) {
                orderid_lookup.erase(resting_order->id);
                level.resting_orders.erase(resting_order);
                --level.num_orders;
            }

            if (level.resting_orders.empty()) {
                asks.erase(ask_it);
            }

            trades.push_back(record);

        } else if (incoming.side == SELL) {
            auto best_bid_r = best_bid();

            auto bid_it = bids.find(*best_bid_r);
            Limit &level = bid_it->second;

            auto resting_order = level.resting_orders.begin();
            Qty trade_qty = (incoming.qty < resting_order->qty) ? incoming.qty : resting_order->qty;

            Trade record = {resting_order->id, incoming.id, resting_order->price, trade_qty, incoming.timestamp};

            incoming.qty            -= trade_qty;
            resting_order->qty      -= trade_qty;
            level.total_volume      -= trade_qty;

            if (resting_order->qty == 0) {
                orderid_lookup.erase(resting_order->id);
                level.resting_orders.erase(resting_order);
                --level.num_orders;
            }

            if (level.resting_orders.empty()) {
                bids.erase(bid_it);
            }

            trades.push_back(record);
        } else {
            throw std::logic_error("Error: Side must be either 'Sell' or 'Buy'!");
        }
    }

    if (incoming.qty > 0) add_order(incoming);

    return trades;
}

bool OrderBook::check_invariants() const {
    for (auto limit_it { bids.begin() }; limit_it != bids.end(); ++limit_it) {
        const Limit &level = limit_it->second;

        if (limit_it->first != level.price) return false;
        if (level.resting_orders.empty()) return false;
        if (level.resting_orders.size() != level.num_orders) return false;

        Qty actual_volume {};
        for (auto order_it { level.resting_orders.begin() }; order_it != level.resting_orders.end(); ++order_it) {
            actual_volume += order_it->qty;
        }

        if (actual_volume != level.total_volume) return false;
    }

    for (auto limit_it { asks.begin() }; limit_it != asks.end(); ++limit_it) {
        const Limit &level = limit_it->second;

        if (limit_it->first != level.price) return false; 
        if (level.resting_orders.empty()) return false;
        if (level.resting_orders.size() != level.num_orders) return false;

        Qty actual_volume {};
        for (auto order_it { level.resting_orders.begin() }; order_it != level.resting_orders.end(); ++order_it) {
            actual_volume += order_it->qty;
        }

        if (actual_volume != level.total_volume) return false;
    }

    return true; 
}