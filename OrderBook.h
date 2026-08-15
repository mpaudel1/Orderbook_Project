#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Limit.h"
#include "Trade.h"

#include <map>
#include <unordered_map>
#include <vector>
#include <optional>

class OrderBook
{
    private:
        struct OrderLocation {
            Side            side;
            Price           price;
            OrderIterator   order_it;
        };

        std::map<Price, Limit>                      bids;
        std::map<Price, Limit>                      asks;
        std::unordered_map<OrderId, OrderLocation>  orderid_lookup;

        void match_buy_once(Order & incoming, std::vector<Trade> & trades);
        void match_sell_once(Order & incoming, std::vector<Trade> & trades);

        bool can_match(const Order & order) const;
    
    public:
        void add_order(const Order & order);
        void cancel_order(OrderId order_id);

        void print_book();

        std::optional<Price> best_bid() const;
        std::optional<Price> best_ask() const;
        
        bool would_match(const Order & order) const;
        bool check_invariants() const;

        std::vector<Trade> process_order(const Order & order);
};

#endif