//
// Created by Mark Gagarine on 2024-12-29.


#include "simulation/OrderBook.hpp"
#include <iostream>


/**
 * @brief 
 */
OrderBook::OrderBook() {
    // Constructor initialization
}

/**
 * @brief Retrieve the best quoted price for a given side
 * @param side Request buy or sell side quote
 * @return Price of Lowest ask (for buy) or Highest bid (for sell)
 * @note An empty side returns a best price of 0
 */
Price OrderBook::getBestQuote(Side side) const {
    if (side == Side::buy) {
        // return ask price closest to bids
        if (_asks.empty()) {
            return 0.;
        }
        else {
            auto lowestAsk = _asks.begin();
            return lowestAsk->first;
        }
    }
    else {
        // return bid price closest to asks
        if (_bids.empty()) {
            return 0.;
        }
        else {
            auto highestBid = _bids.begin();
            return highestBid->first;
        }
    }
}

/**
 * @brief Calculate the spread of the current orderbook state
 * @return Price difference between the lowest ask and highest bid
 */
Price OrderBook::getSpread() const {
    return getBestQuote(Side::buy) - getBestQuote(Side::sell);
}

/**
 * @brief Return the current state of the orderbook
 * @return Ordered map of price level and its open order data
 */
std::map<Price, LevelData, std::greater<Price>> OrderBook::getPriceLevelData() {//} const {
    return _priceLevelData;
}

//Trade* OrderBook::addOrder(Order *newOrder, int tradeId) {
/**
 * @brief Update the orderbook with a new order
 * @param newOrder Pointer to the new order object
 */
void OrderBook::addOrder(Order *newOrder) {
    /*
    // Initialize return result
    Trade newTrade = Trade(tradeId);
    Trade* tradeResult = &newTrade;
*/
    // TODO: replace if else for switch case
    // direct order based on type
    if (newOrder->getType() == EventType::market){
        std::cout << "Routing market order" << std::endl;
        // check order side
        if (newOrder->getSide() == Side::buy) { // route market buy
            //routeMarketBuy(newOrder, tradeResult);
            routeMarketBuy(newOrder);
        }
        else {
            //routeMarketSell(newOrder, tradeResult);
            routeMarketSell(newOrder);
        }
        //return tradeResult;
    }
    else if (newOrder->getType() == EventType::limit){
        std::cout << "Routing limit order" << std::endl;
        routeLimit(newOrder);
    }
    else if (newOrder->getType() == EventType::cancel){
        // route order cancellation
        routeCancellation(newOrder);
    }
    // remove any empty price levels from data
    for (auto level = _priceLevelData.begin(); level != _priceLevelData.end(); ) {
        if (level->second.quantity == 0) {
            level = _priceLevelData.erase(level);
        }
        else {
            ++level;
        }
    }
}

//void OrderBook::matchMarketOrder(Order *newOrder, Orders ordersAtLevel, Trade* trade) {
/**
 * @brief Match a new market order to current open limit orders
 * @param newOrder New market order to be filled (fully or partially)
 * @param ordersAtLevel Current open limit orders
 */
void OrderBook::matchMarketOrder(Order *newOrder, Orders& ordersAtLevel) {
    for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {
        // check order isn't filled
        if (newOrder->getQuantityRemaining() == 0) {
            break;
        }
        // access current order
        Order* currentOrder = *orders;
        // process current order
        Quantity fillQuantity = std::min(newOrder->getQuantityRemaining(), currentOrder->getQuantityRemaining());
        currentOrder->fill(fillQuantity);
        newOrder->fill(fillQuantity);
        // update PriceLevelData
        _priceLevelData[currentOrder->getPrice()].quantity -= fillQuantity;
        _priceLevelData[currentOrder->getPrice()].orderCount--;
        std::cout << "--- Filled " << fillQuantity << " @ $" << currentOrder->getPrice() << "\n";
        /*
        // update trade info
        trade->updateTradeData(newOrder->getOrderId(), fillQuantity, newOrder->getPrice(), newOrder->getSide());
        trade->updateTradeData(currentOrder->getOrderId(), fillQuantity, currentOrder->getPrice(), currentOrder->getSide());
        */
        // check if current limit order needs to be removed from queue (it's filled)
        if (currentOrder->getQuantityRemaining() == 0) {
            // remove the empty order
            orders = ordersAtLevel.erase(orders);
        }
        /*else {
            // new market order has been filled, limit orders in queue cannot be filled anymore
            break;
        }*/
    }
}


//void OrderBook::routeMarketBuy(Order *newOrder, Trade* trade) {
/**
 * @brief Execute markey buy order
 * @param newOrder Market buy order object
 */
void OrderBook::routeMarketBuy(Order *newOrder) {
    // check market order still needs to be filled
    //while ((newOrder->getQuantityRemaining() > 0) && (newOrder->getOrderStatus() != OrderStatus::filled)) {

        //std::cout << "ran here\n";
        for (auto ask : _asks){
            std::cout << "::: " << ask.second.size() << " quotes available @ $" << ask.first << "\n";
        }
        // Check if any open orders available
        /*if (_asks.empty()) {
            std::cout << "ran here\n";
            //break;
        }*/
        // sweep through all price levels
        for (auto level = _asks.begin(); level != _asks.end(); ) {
            // exit loop if order is filled
            if (newOrder->getQuantityRemaining() == 0) {
                break;
            }
            // access orders at current level
            auto& ordersAtLevel = level->second;
            // look through current level queue
            //matchMarketOrder(newOrder, ordersAtLevel, trade);
            matchMarketOrder(newOrder, ordersAtLevel);
            // remove levels with no orders left, moving to next level (so no iter)
            if (level->second.empty()) {
                level = _asks.erase(level);
            }
        }
    }
//}

//void OrderBook::routeMarketSell(Order *newOrder, Trade* trade) {

/**
 * @brief Execute markey buy sell
 * @param newOrder Market sell order object
 */
void OrderBook::routeMarketSell(Order *newOrder) {

    // check market order still needs to be filled
    while ((newOrder->getQuantityRemaining() > 0) && (newOrder->getOrderStatus() != OrderStatus::filled)) {

        // Check if any open orders available
        if (_bids.empty()) {
            break;
        }

        // sweep through all price levels
        for (auto level = _bids.begin(); level != _bids.end(); ++level) {

            // check order isn't filled
            if (newOrder->getQuantityRemaining() == 0) {
                break;
            }

            // access orders at current level
            auto& ordersAtLevel = level->second;

            // look through current level queue
            //matchMarketOrder(newOrder, ordersAtLevel, trade);
            matchMarketOrder(newOrder, ordersAtLevel);

            // remove levels with no orders left
            if (level->second.size() == 0) {
                level = _bids.erase(level);
            }
        }
    }
}

/**
 * @brief Execute limit order
 * @param newOrder Limit order object
 */
void OrderBook::routeLimit(Order *newOrder) {
    // check order side
    if (newOrder->getSide() == Side::buy){
        // route buy limit
        _bids[newOrder->getPrice()].push_back(newOrder);
        // update price level info
        _priceLevelData[newOrder->getPrice()].quantity += newOrder->getQuantity();
        _priceLevelData[newOrder->getPrice()].orderCount++;
        std::cout<<"added order\n";
    }
    else {
        // check sell is not below minimum
        // ---!!!!
        // route sell limit
        _asks[newOrder->getPrice()].push_back(newOrder);
        // update price level info
    std::cout << "works here\n";
        _priceLevelData[newOrder->getPrice()].quantity  += newOrder->getQuantity();
        _priceLevelData[newOrder->getPrice()].orderCount++;
    }
}

/**
 * @brief Execute order cancellation
 * @param newOrder
 */
void OrderBook::routeCancellation(Order *newOrder) {
    // check order side
    if (newOrder->getSide() == Side::buy) {
        // find price level
        Orders ordersAtLevel = _bids[newOrder->getPrice()];
        // iterate through bids until desired order is found
        for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {
            Order* order = *orders;
            // delete desired order from queue
            if (order->getOrderId() == newOrder->getOrderId()) {
                // update price level data
                _priceLevelData[newOrder->getPrice()].quantity -= newOrder->getQuantityRemaining();
                _priceLevelData[newOrder->getPrice()].orderCount--;
                // update order book
                orders = ordersAtLevel.erase(orders);
                // break out of loop
                break;
            }
            else {
                ++orders;
            }
        }
    }
    else {
        // find price level
        Orders ordersAtLevel = _asks[newOrder->getPrice()];

        // iterate through bids until desired order is found
        for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {

            Order* order = *orders;

            // delete desired order from queue
            if (order->getOrderId() == newOrder->getOrderId()) {

                // update price level data
                _priceLevelData[newOrder->getPrice()].quantity -= newOrder->getQuantityRemaining();
                _priceLevelData[newOrder->getPrice()].orderCount--;

                // update order book
                orders = ordersAtLevel.erase(orders);

                // break out of loop
                break;
            }
            else {
                ++orders;
            }
        }
    }
}

