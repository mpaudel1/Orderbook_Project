CXX = g++
CXXFLAGS = -std=c++17 -Wall 

TARGET = orderbook

SRCS = main.cpp OrderBook.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp OrderBook.h Limit.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c main.cpp

OrderBook.o: OrderBook.cpp OrderBook.h Limit.h Order.h Types.h
	$(CXX) $(CXXFLAGS) -c OrderBook.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)