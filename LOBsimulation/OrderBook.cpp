//
// Created by Mark Gagarine on 2024-12-29.
//


#include "OrderBook.hpp"
#include <iostream>

Order::Order(EventType type, int orderId, Side side, Price price, Quantity quantity)
    : _type(type)
    , _orderId(orderId)
    , _side(side)
    , _price(price)
    , _quantity(quantity)   {

    // Constructor initialization
    setQuantityRemaining(_quantity);
    _status = OrderStatus::open;
}

EventType Order::getType() const {
    return _type;
}

int Order::getOrderId() const {
    return _orderId;
}

Side Order::getSide() const {
    return _side;
}

Price Order::getPrice() const {
    return _price;
}

Quantity Order::getQuantity() const {
    return _quantity;
}

Quantity Order::getQuantityRemaining() const {
    return _quantityRemaining;
}


OrderStatus Order::getOrderStatus() const {
    return _status;
}

void Order::setQuantityRemaining(Quantity newQuantity) {
    _quantityRemaining = newQuantity;
}

void Order::fill(Quantity fillQuantity) {

    // update remaining quantity
    _quantityRemaining -= fillQuantity;

    // update order status
    if (_quantityRemaining) {
        _status = OrderStatus::partial;
    }
    else {
        _status = OrderStatus::filled;
    }
}


OrderBook::OrderBook() {
    // Constructor initialization
}

void OrderBook::addOrder(Order *newOrder) {

    // direct order based on type
    if (newOrder->getType() == EventType::market){

        // check order side
        if (newOrder->getSide() == Side::buy) { // route market buy
            routeMarketBuy(newOrder);
        }
        else {
            routeMarketSell(newOrder);
        }
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

    else {

        // handle error???
        throw std::runtime_error("Event type not defined.\n");
    }
}

void OrderBook::matchMarketOrder(Order *newOrder, Orders ordersAtLevel) {
    for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {

        // access current order
        Order* currentOrder = *orders;

        // process current order
        Quantity fillQuantity = std::min(newOrder->getQuantityRemaining(), currentOrder->getQuantityRemaining());
        currentOrder->fill(fillQuantity);
        newOrder->fill(fillQuantity);

        // update PriceLevelData
        _priceLevelData[newOrder->getPrice()] -= fillQuantity;

        // update trade info

        // check if current limit order needs to be removed from queue (it's filled)
        if (currentOrder->getQuantityRemaining() == 0) {
            // remove the empty order
            orders = ordersAtLevel.erase(orders);
        }
        else {
            // new market order has been filled, limit orders in queue cannot be filled anymore
            break;
        }
    }
}


void OrderBook::routeMarketBuy(Order *newOrder) {
    // check market order still needs to be filled
    while ((newOrder->getQuantityRemaining() > 0) && (newOrder->getOrderStatus() != OrderStatus::filled)) {

        // sweep through all price levels
        for (auto level = _asks.begin(); level != _asks.end(); ++level) {

            // check order isn't filled
            if (newOrder->getQuantityRemaining() == 0) {
                break;
            }

            // access orders at current level
            auto& ordersAtLevel = level->second;

            // look through current level queue
            matchMarketOrder(newOrder, ordersAtLevel);
        }
    }
}

void OrderBook::routeMarketSell(Order *newOrder) {
    // check market order still needs to be filled
    while ((newOrder->getQuantityRemaining() > 0) && (newOrder->getOrderStatus() != OrderStatus::filled)) {

        // sweep through all price levels
        for (auto level = _bids.begin(); level != _bids.end(); ++level) {

            // check order isn't filled
            if (newOrder->getQuantityRemaining() == 0) {
                break;
            }

            // access orders at current level
            auto& ordersAtLevel = level->second;

            // look through current level queue
            matchMarketOrder(newOrder, ordersAtLevel);
        }
    }
}

void OrderBook::routeLimit(Order *newOrder) {
    // check order side
    if (newOrder->getSide() == Side::buy){
        // route buy limit
        _bids[newOrder->getPrice()].push_back(newOrder);

        // update price level info
        _priceLevelData[newOrder->getPrice()] += newOrder->getQuantity();
    }
    else {
        // check sell is not below minimum
        // ---!!!!


        // route sell limit
        _asks[newOrder->getPrice()].push_back(newOrder);

        // update price level info
        _priceLevelData[newOrder->getPrice()] += newOrder->getQuantity();
    }
}

void OrderBook::routeCancellation(Order *newOrder) {

}