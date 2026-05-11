#ifndef TRADE_H
#define TRADE_H

#include "Types.h"

struct Trade {
    OrderId     buy_order_id {};
    OrderId     sell_order_id {};
    Price       price {};
    Qty         qty {};
    Timestamp   timestamp {};
};

#endif