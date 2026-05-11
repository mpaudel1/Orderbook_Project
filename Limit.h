#ifndef LIMIT_H
#define LIMIT_H

#include "Order.h"

#include <list>

using OrderList         = std::list<Order>;
using OrderIterator     = std::list<Order>::iterator;


struct Limit
{
        Price       price{};
        int         num_orders{};
        Qty         total_volume{};
        OrderList   resting_orders{};
};

#endif 