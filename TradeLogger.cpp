#include "TradeLogger.h"

#include <stdexcept>

TradeLogger::TradeLogger(const std::string & file_name)
    : outFile(file_name)
{
    if (!outFile.is_open()) {
        throw std::logic_error("Error: could not open trade log file");
    }

    outFile << "buy_order_id,sell_order_id,price,qty,timestamp\n";
}

void TradeLogger::log_trades(const std::vector<Trade> & trades) {
    for (const Trade & trade : trades) {
        outFile
            << trade.buy_order_id << ','
            << trade.sell_order_id << ','
            << trade.price << ','
            << trade.qty << ','
            << trade.timestamp << '\n';
    }
}