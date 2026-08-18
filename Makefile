CXX = g++
CXXFLAGS = -std=c++17 -Wall 

TARGET = orderbook
TEST = orderbook_test

SRCS = main.cpp OrderBook.cpp EventParser.cpp
OBJS = $(SRCS:.cpp=.o)

TEST_SRCS = main_tests.cpp OrderBook.cpp EventParser.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(TEST): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(TEST_OBJS) -o $(TEST)

main.o: main.cpp OrderBook.h Limit.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c main.cpp

OrderBook.o: OrderBook.cpp OrderBook.h Limit.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c OrderBook.cpp

EventParser.o: EventParser.cpp EventParser.h Event.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c EventParser.cpp

run: $(TARGET)
	./$(TARGET)

test: $(TEST)
	./$(TEST)

clean:
	rm -f *.o $(TARGET) $(TEST)