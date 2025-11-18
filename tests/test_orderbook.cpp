//
// Created by Mark Gagarine on 2025-08-06.
//

// Orderbook class unit tests

#include <iostream>
#include <cassert>
#include "engine/OrderBook.hpp"

using namespace std;

int main() {

    cout << "Testing OrderBook class..." << endl;
    OrderBook AAPL = OrderBook();
    assert(AAPL.getBestQuote(Side::buy) == 0);
    assert(AAPL.getBestQuote(Side::sell) == 0);
    assert(AAPL.getSpread() == 0);
    assert(AAPL.getPriceLevelData().empty() == 1);

    Order* order0 = new Order(EventType::market, 0, Side::buy, 100.0, 10);
    AAPL.addOrder(order0);
    delete order0;

    assert(AAPL.getBestQuote(Side::buy) == 0);
    assert(AAPL.getBestQuote(Side::sell) == 0);
    assert(AAPL.getSpread() == 0);
    assert(AAPL.getPriceLevelData().empty() == 1);

    Order* order1 = new Order(EventType::limit, 1, Side::buy, 100.0, 10);
    AAPL.addOrder(order1);
    delete order1;

    std::cout << AAPL.getBestQuote(Side::sell) << std::endl;

    DeltaUpdates state = AAPL.publishUpdates();

    for (auto& s: state) {
        std::cout << " " << s->quantity << " @ $" << s->price << std::endl;
    }

    cout << "...Finished testing OrderBook class" << endl;
    return 0;
}