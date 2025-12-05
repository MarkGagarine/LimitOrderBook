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

using PriceLevelData = std::map<Price, LevelData, std::greater<Price>>;

/**
* @brief A structure storing changes made to the order book state by the order
 */
struct Delta {
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
    void addUpdate(Order* newOrder);
    void addUpdate(EventType type, unsigned int orderId, Quantity quantity, double price, Side side);
    size_t cnt;
    DeltaUpdates deltas;
};

class OrderBook {
public:

    OrderBook();
    ~OrderBook();

    DeltaUpdates publishUpdates();
    void addOrder(Order* newOrder);

    Price getBestQuote(Side side) const;
    Price getSpread() const;
    PriceLevelData getPriceLevelData() const;
    std::set<int> getOrderIds() const;
    int getActiveOrderID(Price price) const;
    int getTopOrderId() const;

private:

    template <typename BookSideMap>
    void routeMarketOrder(Order* newOrder, BookSideMap& bookSide);
    void matchMarketOrder(Order* newOrder, Orders& ordersAtLevel);

    template <typename BookSideMap>
    void routeLimitOrder(Order* newOrder, BookSideMap& bookSide);

    template <typename BookSideMap>
    void routeCancelOrder(Order* newOrder, BookSideMap& bookSide);

    PriceLevelData  _priceLevelData;
    std::map<Price, Orders, std::greater<Price>> _bids;
    std::map<Price, Orders, std::less<Price>> _asks;

    std::map<Price, std::set<int>> _activeOrders;
    std::set<int> orderIds;
    //std::map<int, Orders::iterator> _orderIndex;
    Updates _updateBuffer;
};

//#endif //LOBSIMULATION_ORDERBOOK_HPP
