//
// Created by Mark Gagarine on 2024-12-28.
//

//#ifndef LOBSIMULATION_ORDERBOOK_HPP
//#define LOBSIMULATION_ORDERBOOK_HPP
#pragma once

#include "Order.hpp"

#include <vector>
#include <map>
#include <set>

using Orders = std::vector<Order*>;

/**
 * @brief Store outstanding order data for a side at a given price level
 */
struct LevelData {
    Quantity quantity;
    int orderCount;
};

/**
* @brief A structure storing changes made to the order book state by the order
 */
struct Delta {
    //Delta();
    //Delta(EventType type, unsigned int orderId, Quantity quantity, double price, Side side)
      //  : type(type), orderId(orderId), quantity(quantity), price(price), side(side) {}
    // time??
    EventType type;
    unsigned int orderId;
    Quantity quantity;
    double price;
    Side side;
};

using DeltaUpdates = std::vector<std::unique_ptr<Delta>>;

struct Updates {
    Updates();
    //void addUpdate(const Delta* delta);
    void addUpdate(Order* newOrder);
    void addUpdate(EventType type, unsigned int orderId, Quantity quantity, double price, Side side);
    size_t cnt;
    DeltaUpdates deltas;
};

class OrderBook {
public:

    OrderBook();

    DeltaUpdates publishUpdates();
    void addOrder(Order* newOrder);

    Price getBestQuote(Side side) const;
    Price getSpread() const;
    std::map<Price, LevelData, std::greater<Price>> getPriceLevelData() const;
    std::set<int> getOrderIds() const;
    int getTopOrderId() const;

private:

    void routeMarketBuy(Order* newOrder);
    void routeMarketSell(Order* newOrder);
    void matchMarketOrder(Order* newOrder, Orders& ordersAtLevel);

    void routeLimit(Order* newOrder);
    void routeCancellation(Order* newOrder);

    std::map<Price, LevelData, std::greater<Price>>  _priceLevelData;
    std::map<Price, Orders, std::greater<Price>> _bids;
    std::map<Price, Orders, std::less<Price>> _asks;

    std::set<int> orderIds;
    Updates _updateBuffer;
};


//#endif //LOBSIMULATION_ORDERBOOK_HPP
