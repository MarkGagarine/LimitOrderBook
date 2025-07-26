//
// Created by Mark Gagarine on 2024-12-29.
//


#include "OrderBook.hpp"
#include <iostream>


OrderBook::OrderBook() {
    // Constructor initialization
}

Price OrderBook::getBestQuote(Side side) const {

    // check side
    if (side == Side::buy) {
        // return ask price closest to bids
        auto lowestAsk = _asks.begin();
        return lowestAsk->first;
    }
    else {
        // return bid price closest to asks
        auto highestBid = _bids.begin();
        return highestBid->first;
    }
}

Price OrderBook::getSpread() const {
    return getBestQuote(Side::buy) - getBestQuote(Side::sell);
}

std::map<Price, LevelData, std::greater<Price>> OrderBook::getPriceLevelData(){//} const {
    return _priceLevelData;
}

//Trade* OrderBook::addOrder(Order *newOrder, int tradeId) {
void OrderBook::addOrder(Order *newOrder) {
    /*
    // Initialize return result
    Trade newTrade = Trade(tradeId);
    Trade* tradeResult = &newTrade;
*/
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

        // route limit order
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
        //for (auto level = _asks.begin(); level != _asks.end(); ++level) {
        for (auto level = _asks.begin(); level != _asks.end(); ) {

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
                level = _asks.erase(level);
            }
        }
    }
//}

//void OrderBook::routeMarketSell(Order *newOrder, Trade* trade) {
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

