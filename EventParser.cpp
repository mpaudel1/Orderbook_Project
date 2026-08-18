#include "EventParser.h"

#include <string>
#include <sstream>
#include <stdexcept>

Event EventParser::parse_event_line(const std::string & line) {
    std::istringstream ss(line);

    std::string event_line_type {};

    ss >> event_line_type;

    if (event_line_type == "LIMIT") {
        EventType event_type {EventType::NEW_ORDER};

        OrderId orderId {}; Side side {}; Price price {}; Qty qty {}; Timestamp timestamp {};
        std::string sside {};

        ss >> orderId >> sside >> price >> qty >> timestamp;

        if (sside == "BUY") {
            side = Side::BUY;
        } else if (sside == "SELL") {
            side = Side::SELL;
        } else {
            throw std::logic_error("Error: Side must be either 'Sell' or 'Buy'!");
        }

        Order order {
            orderId, side, Type::LIMIT, price, qty, timestamp
        };

        Event event_line {
            event_type, order
        };

        return event_line;

    } else if (event_line_type == "MARKET") {
        EventType event_type {EventType::NEW_ORDER};

        OrderId orderId {}; Side side {}; Qty qty {}; Timestamp timestamp {};
        std::string sside {};

        ss >> orderId >> sside >> qty >> timestamp;

        if (sside == "BUY") {
            side = Side::BUY;
        } else if (sside == "SELL") {
            side = Side::SELL;
        } else {
            throw std::logic_error("Error: Side must be either 'Sell' or 'Buy'!");
        }

        Order order {
            orderId, side, Type::MARKET, 0, qty, timestamp
        };

        Event event_line {
            event_type, order
        };

        return event_line;

    } else if (event_line_type == "CANCEL") {
        EventType event_type {EventType::CANCEL};

        OrderId orderId {};

        ss >> orderId;

        Event event_line {
            event_type, Order {}, orderId
        };

        return event_line;

    } else {
        throw std::logic_error("Error: Type must be either 'LIMIT', 'MARKET', or 'CANCEL");
    }
}