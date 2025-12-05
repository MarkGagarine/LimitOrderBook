//
// Created by Mark Gagarine on 2025-01-05.
//

#ifndef ORDER_HPP
#define ORDER_HPP

#include <cstdint>  // uint8_t

using Price = double;
using Quantity = int;

enum class Side : uint8_t {
    buy,
    sell
};

enum class EventType : uint8_t {
    market,
    limit,
    cancel
};

enum class OrderStatus : uint8_t {
    open,
    partial,
    filled
};

class Order {
public:
    Order(EventType type, int orderId, Side side, Price price, Quantity quantity);

    EventType getType() const;
    int getOrderId() const;
    Side getSide() const;
    Price getPrice() const;
    Quantity getQuantity() const;
    Quantity getQuantityRemaining() const;
    Quantity getQuantityFilled() const;
    OrderStatus getOrderStatus() const;

    void fill(Quantity fillQuantity);
    void setQuantityRemaining(Quantity newQuantity);

private:

    Price _price;
    Quantity _quantity;
    Quantity _quantityRemaining;
    int _orderId;
    EventType _type;
    Side _side;
    OrderStatus _status;

};

#endif //ORDER_HPP