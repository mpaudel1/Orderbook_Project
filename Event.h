#ifndef EVENT_H
#define EVENT_H

#include "Types.h"
#include "Order.h"

enum class EventType {
    NEW_ORDER,
    CANCEL
};

struct Event {
    EventType   event_type{};
    Order       order{};
    OrderId     cancel_order_id{};
};

#endif