//
// Created by Mark Gagarine on 2025-01-05.
//

#include "simulation/Order.h"


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
