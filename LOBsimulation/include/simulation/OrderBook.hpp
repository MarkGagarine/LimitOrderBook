//
// Created by Mark Gagarine on 2024-12-28.
//

#ifndef LOBSIMULATION_ORDERBOOK_HPP
#define LOBSIMULATION_ORDERBOOK_HPP

#include "Order.h"
//#include "Trade.h"

#include <vector>
#include <map>

using Orders = std::vector<Order*>;

/**
 * @brief Store outstanding order data for a given price level
 */
struct LevelData {
    Quantity quantity;
    int orderCount;
};


class OrderBook {
public:

    OrderBook();

    // getters
    Price getBestQuote(Side side) const;
    Price getSpread() const;
    std::map<Price, LevelData, std::greater<Price>> getPriceLevelData();// const;

    // update methods
    //Trade* addOrder(Order* newOrder, int tradeId);

    void addOrder(Order* newOrder);
    void updatePriceLevel();


private:
/*
    void routeMarketBuy(Order* newOrder, Trade* trade);
    void routeMarketSell(Order* newOrder, Trade* trade);
    void matchMarketOrder(Order* newOrder, Orders ordersAtLevel, Trade* trade);
*/
    void routeMarketBuy(Order* newOrder);
    void routeMarketSell(Order* newOrder);
    void matchMarketOrder(Order* newOrder, Orders& ordersAtLevel);

    void routeLimit(Order* newOrder);
    void routeCancellation(Order* newOrder);

    //std::map<Price, Quantity, std::greater<Price>>  _priceLevelData;
    std::map<Price, LevelData, std::greater<Price>>  _priceLevelData;
    std::map<Price, Orders, std::greater<Price>> _bids;
    std::map<Price, Orders, std::less<Price>> _asks;

};


#endif //LOBSIMULATION_ORDERBOOK_HPP
