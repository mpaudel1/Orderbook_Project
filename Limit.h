#ifndef LIMIT_H
#define LIMIT_H

#include "Order.h"

#include <list>
#include <cstddef>

using OrderList         = std::list<Order>;
using OrderIterator     = std::list<Order>::iterator;


struct Limit
{
        Price       price{};
        std::size_t num_orders{};
        Qty         total_volume{};
        OrderList   resting_orders{};
};

#endif 