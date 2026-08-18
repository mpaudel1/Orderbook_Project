#ifndef TRADELOGGER_H
#define TRADELOGGER_H

#include "Trade.h"

#include <fstream>
#include <string>
#include <vector>

class TradeLogger {
    private:
        std::ofstream outFile;
    
        public:
        TradeLogger(const std::string & file_name);

        void log_trades(const std::vector<Trade> & trades);

};

#endif