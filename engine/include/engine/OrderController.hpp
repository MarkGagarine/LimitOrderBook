//
// Created by Mark Gagarine on 2025-11-20.
//

#ifndef ORDERCONTROLLER_HPP
#define ORDERCONTROLLER_HPP

#include "OrderBook.hpp"

struct NewOrderCmd {
    NewOrderCmd(EventType type, Side side, int orderId, Price price, Quantity quantity);
    EventType type;
    Side side;
    int orderId;
    Price price;
    Quantity quantity;
};

struct MarketOrderCmd : NewOrderCmd {
    MarketOrderCmd(Side side, Quantity quantity);
};

struct LimitOrderCmd : NewOrderCmd {
    LimitOrderCmd(Side side, Price price, Quantity quantity);
};

struct CancelOrderCmd : NewOrderCmd {
    CancelOrderCmd(Side side, int orderId, Price price);
};

// temp here for now
class OrderController {
public:
    OrderController(OrderBook& book);
    //~OrderController();

    DeltaUpdates submit(NewOrderCmd& newOrderCmd);

private:
    template <typename orderCmd>
    Order* orderFromCmd(orderCmd& cmd);

    OrderBook& _book;
    int cnt {0};
};

#endif //ORDERCONTROLLER_HPP
