//
// Created by Mark Gagarine on 2025-01-05.
//

#ifndef LOBSIMULATION_ORDER_H
#define LOBSIMULATION_ORDER_H

#include <vector>

using Price = double;
using Quantity = int;

enum class Side {
    buy,
    sell
};

enum class EventType {
    market,
    limit,
    cancel
};

enum class OrderStatus {
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
    OrderStatus getOrderStatus() const;

    void fill(Quantity fillQuantity);
    void setQuantityRemaining(Quantity newQuantity);

private:


    EventType _type;
    int _orderId;
    Side _side;
    Price _price;
    Quantity _quantity;
    Quantity _quantityRemaining;
    OrderStatus _status;

};

using Orders = std::vector<Order*>;



#endif //LOBSIMULATION_ORDER_H
