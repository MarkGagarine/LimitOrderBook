//
// Created by Mark Gagarine on 2025-01-05.

#include "simulation/Order.h"
#include <stdexcept>

/**
 * @brief Object to store order information
 * @param type market, limit, or cancel
 * @param orderId Unique order identifier
 * @param side buy or sell
 * @param price Requested price that the product is bought/sold for
 * @param quantity Requested number of units be bought/sold
 */
Order::Order(EventType type, int orderId, Side side, Price price, Quantity quantity)
        : _type(type)
        , _orderId(orderId)
        , _side(side)
        , _price(price)
        , _quantity(quantity)   {
    // Constructor initialization
    //setQuantityRemaining(_quantity);
    if (price < 0.0) {
        throw std::invalid_argument("Price cannot be negative");
    }
    if (quantity < 0.0) {
        throw std::invalid_argument("Quantity cannot be negative");
    }
    _quantityRemaining = _quantity;
    _status = OrderStatus::open;
}

/**
 * @brief 
 * @return 
 */
EventType Order::getType() const {
    return _type;
}

/**
 * @brief 
 * @return 
 */
int Order::getOrderId() const {
    return _orderId;
}

/**
 * @brief 
 * @return 
 */
Side Order::getSide() const {
    return _side;
}

/**
 * @brief 
 * @return 
 */
Price Order::getPrice() const {
    return _price;
}

/**
 * @brief 
 * @return
 */
Quantity Order::getQuantity() const {
    return _quantity;
}

/**
 * @brief
 * @return
 */
Quantity Order::getQuantityRemaining() const {
    return _quantityRemaining;
}


/**
 * @brief
 * @return
 */
OrderStatus Order::getOrderStatus() const {
    return _status;
}

/**
 * @brief
 * @param newQuantity
 */
void Order::setQuantityRemaining(Quantity newQuantity) {
    _quantityRemaining = newQuantity;
}

/**
 * @brief
 * @param fillQuantity
 */
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
