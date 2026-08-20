CXX = g++
CXXFLAGS = -std=c++17 -Wall 

TARGET = orderbook
TEST = orderbook_test
BENCH = benchmark

SRCS = main.cpp OrderBook.cpp EventParser.cpp TradeLogger.cpp
OBJS = $(SRCS:.cpp=.o)

TEST_SRCS = main_tests.cpp OrderBook.cpp EventParser.cpp TradeLogger.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

BENCH_SRCS = benchmark.cpp OrderBook.cpp
BENCH_OBJS = $(BENCH_SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(TEST): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(TEST_OBJS) -o $(TEST)

$(BENCH): $(BENCH_OBJS)
	$(CXX) $(CXXFLAGS) $(BENCH_OBJS) -o $(BENCH)

main.o: main.cpp OrderBook.h Limit.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c main.cpp

OrderBook.o: OrderBook.cpp OrderBook.h Limit.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c OrderBook.cpp

EventParser.o: EventParser.cpp EventParser.h Event.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c EventParser.cpp

TradeLogger.o: TradeLogger.cpp TradeLogger.h Trade.h
	$(CXX) $(CXXFLAGS) -c TradeLogger.cpp

run: $(TARGET)
	./$(TARGET)

test: $(TEST)
	./$(TEST)

bench: $(BENCH)
	./$(BENCH)

clean:
	rm -f *.o $(TARGET) $(TEST) $(BENCH)