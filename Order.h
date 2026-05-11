#ifndef ORDER_H
#define ORDER_H

#include "Types.h"

struct Order
{
    OrderId     id;
    Side        side;
    Price       price;
    Qty         qty;
    Timestamp   timestamp;
};

#endif