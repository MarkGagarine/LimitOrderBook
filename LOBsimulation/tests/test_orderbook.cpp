//
// Created by Mark Gagarine on 2025-08-06.
//

// Orderbook class unit tests

#include <iostream>
#include <cassert>
#include "simulation/OrderBook.hpp"

using namespace std;

int main() {

    cout << "Testing OrderBook class..." << endl;
    OrderBook AAPL = OrderBook();
    assert(AAPL.getBestQuote(Side::buy) == 0);
    assert(AAPL.getBestQuote(Side::sell) == 0);
    assert(AAPL.getSpread() == 0);
    assert(AAPL.getPriceLevelData().empty() == 1);

    Order* order1 = new Order(EventType::market, 1, Side::buy, 100.0, 10);
    AAPL.addOrder(order1);

    assert(AAPL.getBestQuote(Side::buy) == 0);
    assert(AAPL.getBestQuote(Side::sell) == 0);
    assert(AAPL.getSpread() == 0);
    assert(AAPL.getPriceLevelData().empty() == 1);


    cout << "...Finished testing OrderBook class" << endl;
    return 0;
}