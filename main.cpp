#include "OrderBook.h"
#include "EventParser.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

void print_trades(const std::vector<Trade>& trades) {
    for (const Trade& trade : trades) {
        std::cout
            << "TRADE "
            << "buy=" << trade.buy_order_id
            << " sell=" << trade.sell_order_id
            << " price=" << trade.price
            << " qty=" << trade.qty
            << " timestamp=" << trade.timestamp
            << '\n';
    }
}

int main() {
    OrderBook book {}; EventParser parser {};

    std::ifstream inFile("events.txt");

    std::string line {};

    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open input file\n";
        return 1;
    }

    while (std::getline(inFile, line)) {
        Event event = parser.parse_event_line(line);
        
        if (event.event_type == EventType::NEW_ORDER) {
            std::vector<Trade> trades = book.process_order(event.order);
            print_trades(trades);
        } else if (event.event_type == EventType::CANCEL) {
            book.cancel_order(event.cancel_order_id);
        } else {
            throw std::logic_error("Error: Type must be either 'LIMIT', 'MARKET', or 'CANCEL'");
        }

        if (!book.check_invariants()) {
            throw std::logic_error("Error: Invariant check failed");
        }
    }

    inFile.close();
    

    book.print_book();

    return 0;
}