//
// Created by Mark Gagarine on 2025-08-06.
//

// Orderbook class unit tests

#include <iostream>
#include <cassert>
#include "engine/OrderBook.hpp"
#include <exception>

using namespace std;
using op = Order*;


/**
 * Class to manage orders generated for testing
 * @note Orders not sitting on the book (market/cancel) are owned outside LOB, thus dangling order pointers are cleaned
 */
class TestOrderManager {
public:
    TestOrderManager() {}
    ~TestOrderManager() {
        for (auto mo : _marketOrders) { delete mo; }
        for (auto co: _cancelOrders) { delete co; }
    }
    Order* makeMarketOrder(Side side, Price price, Quantity quantity) {
        Order* mo = new Order(EventType::market, _oid, side, price, quantity);
        _marketOrders.push_back(mo);
        ++_oid;
        return mo;
    }
    Order* makeLimitOrder(Side side, Price price, Quantity quantity){
        Order* lo = new Order(EventType::limit, _oid, side, price, quantity);
        ++_oid;
        return lo;
    }
    Order* makeCancelOrder(Side side, Price price, Quantity quantity) {
        Order* co = new Order(EventType::cancel, _oid, side, price, quantity);
        _cancelOrders.push_back(co);
        return co;
    }
private:
    int _oid {0};
    vector<Order*> _marketOrders;
    vector<Order*> _cancelOrders;
};

/**
 * Integration test to check Market order logic - All at same price
 * @param side Limit order side
 * @return result of test
 */
bool testMarketSweep1(Side side) {
    OrderBook book = OrderBook();
    TestOrderManager om = TestOrderManager();
    Side oSide = (side == Side::buy) ? Side::sell : Side::buy;
    Quantity tstQty = 10.0;
    Price tstPx = 100.0;
    // market buys match with limit sells
    op o0 = om.makeLimitOrder(side, tstPx, tstQty);
    op o1 = om.makeLimitOrder(side, tstPx, tstQty);
    op o2 = om.makeLimitOrder(side, tstPx, tstQty);

    book.addOrder(o0);
    book.addOrder(o1);
    book.addOrder(o2);
    DeltaUpdates d0 = book.publishUpdates();
    PriceLevelData pd0 = book.getPriceLevelData();
    LevelData ld0 = pd0[tstPx];

    Quantity partialSweep1 = 0.5 * tstQty;
    op o3 = om.makeMarketOrder(side, tstPx, partialSweep1);
    book.addOrder(o3);
    DeltaUpdates d1 = book.publishUpdates();
    PriceLevelData pd1 = book.getPriceLevelData();
    LevelData ld1 = pd1[tstPx];
    // only part of first order filled, number of open orders remains unchanged
    if (ld0.orderCount != ld1.orderCount) {
        throw runtime_error("Expected to fill only part of first order, ended up filling it all");
    }

    return true;
}

/**
 * Test logic for limit orders sitting on the book - at the same price
 * @param side Buy or Sell
 */
void testLimitOrders1(Side side) {
    OrderBook book = OrderBook();
    TestOrderManager om = TestOrderManager();
    Price tstPx = 100.0;
    Quantity tstQty = 10.0;
    op o0 = om.makeLimitOrder(side, tstPx, tstQty);
    book.addOrder(o0);
    DeltaUpdates d0 = book.publishUpdates();
    if (d0.size() != 1) {
        throw runtime_error("First update delta missing info");
    }
    auto& d = d0[0];
    if (d->side != side) {
        throw runtime_error("First update side mismatch");
    }
    if (d->price != tstPx) {
        throw runtime_error("First update price mismatch");
    }
    if (d->quantity != tstQty) {
        throw runtime_error("First update quantity mismatch");
    }
    op o1 = om.makeLimitOrder(side, tstPx, tstQty);
    book.addOrder(o1);
    DeltaUpdates d1 = book.publishUpdates();
    PriceLevelData pd1 = book.getPriceLevelData();
    if (pd1.find(tstPx) == pd1.end()) {
        throw runtime_error("Price Level Data Missing");
    }
    LevelData ld1 = pd1[tstPx];
    if (ld1.quantity != 2*tstQty) {
        throw runtime_error("Price Level Data Quantity Mismatch");
    }
    if (ld1.orderCount != 2) {
        throw runtime_error("Price Level Data Order Count Mismatch");
    }
}

int main() {
    cout << "Testing OrderBook class..." << endl;
    try {
        cout << "\ttestLimitOrders1: Checking Buys..." << endl;
        testLimitOrders1(Side::buy);
    }
    catch (const exception& e) {
        cerr << "Test failed: Limit Buys 1, Buys" << endl;
        cerr << e.what() << endl;
    }
    try {
        cout << "\ttestLimitOrders1: Checking Sells..." << endl;
        testLimitOrders1(Side::sell);
    }
    catch (const exception& e) {
        cerr << "Test failed: Limit Buys 1, Sells" << endl;
        cerr << e.what() << endl;
    }
    cout << "Testing Limit Order Logic Complete" << endl;
    try {
        cout << "\ttestMarketSweep1: Checking Buys..." << endl;
        testMarketSweep1(Side::buy);
    }
    catch (const exception& e) {
        cerr << "Test failed: Market Buys 1, Buys" << endl;
        cerr << e.what() << endl;
    }
    try {
        cout << "\ttestMarketSweep1: Checking Sells..." << endl;
        testLimitOrders1(Side::sell);
    }
    catch (const exception& e) {
        cerr << "Test failed: Market Buys 1, Sells" << endl;
        cerr << e.what() << endl;
    }
    cout << "Testing Market Order Logic Complete" << endl;


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

    Order* order1 = new Order(EventType::limit, 1, Side::buy, 99.0, 10);
    Order* order2 = new Order(EventType::limit, 2, Side::buy, 100.0, 10);
    Order* order3 = new Order(EventType::limit, 3, Side::buy, 100.0, 10);
    Order* sell1 = new Order(EventType::limit, 4, Side::sell, 101.0, 10);
    AAPL.addOrder(order1);
    AAPL.addOrder(order2);
    AAPL.addOrder(order3);
    AAPL.addOrder(sell1);

    //std::cout << AAPL.getBestQuote(Side::sell) << std::endl;

    DeltaUpdates state = AAPL.publishUpdates();
    for (auto& s: state) {
        std::cout << " " << s->quantity << " @ $" << s->price << std::endl;
    }

    for (auto& d: AAPL.getPriceLevelData()) {
        std::cout << d.second.quantity << " shares in " << d.second.orderCount << " orders @ $" << d.first << std::endl;
    }

    Order* sweep = new Order(EventType::market, 4, Side::sell, 1.0, 11);
    AAPL.addOrder(sweep);

    state = AAPL.publishUpdates();
    for (auto& s: state) {
        std::cout << " " << s->quantity << " @ $" << s->price << std::endl;
    }

    for (auto& d: AAPL.getPriceLevelData()) {
        std::cout << d.second.quantity << " shares in " << d.second.orderCount << " orders @ $" << d.first << std::endl;
    }

    Order* cancel2 = new Order(EventType::cancel, 2, Side::buy, 100.0, 10);
    AAPL.addOrder(cancel2);

    state = AAPL.publishUpdates();
    for (auto& s: state) {
        std::cout << "Cancel " << s->quantity << " @ $" << s->price << std::endl;
    }

    Order* fill = new Order(EventType::market, 5, Side::sell, 1.0, 12);
    AAPL.addOrder(fill);

    state = AAPL.publishUpdates();
    for (auto& s: state) {
        std::cout << " " << s->quantity << " @ $" << s->price << std::endl;
    }

    std::cout << AAPL.getBestQuote(Side::sell) << std::endl;
    std::cout << AAPL.getBestQuote(Side::buy) << std::endl;


    cout << "...Finished testing OrderBook class" << endl;
    return 0;
}