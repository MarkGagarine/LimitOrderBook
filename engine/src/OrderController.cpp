//
// Created by Mark Gagarine on 2025-11-20.

#include "engine/OrderController.hpp"

/**
 * Base Order Command struct to generate a new order
 * @param type EventType (Limit, Market, Cancel, ...)
 * @param side Buy or Sell
 * @param orderId Unique order identifier
 * @param price Price of the order (ignored for Market orders)
 * @param quantity Quantity of the order (Ignored for Cancel orders)
 */
NewOrderCmd::NewOrderCmd(EventType type, Side side, int orderId, Price price, Quantity quantity)
        : type(type), side(side), orderId(orderId), price(price), quantity(quantity) {}

/**
 * Struct to place new market order
 * @param side Buy or Sell
 * @param quantity Quantity of the order
 */
MarketOrderCmd::MarketOrderCmd(Side side, Quantity quantity)
        : NewOrderCmd(EventType::market, side, 0, 0.0, quantity) {}

/**
 * Struct to place new limit order
 * @param side Buy or Sell
 * @param price Price of the order
 * @param quantity Quantity of the order
 */
LimitOrderCmd::LimitOrderCmd(Side side, Price price, Quantity quantity)
        : NewOrderCmd(EventType::limit, side, 0, price, quantity) {}

/**
 * Struct to place new cancel order
 * @param side Buy or Sell
 * @param orderId Unique order identifier
 * @param price Price of the order
 */
CancelOrderCmd::CancelOrderCmd(Side side, int orderId, Price price)
        : NewOrderCmd(EventType::cancel, side, orderId, price, 0.0) {}

/**
 * Limit Order Book interface
 * @param book Reference to LOB
 */
OrderController::OrderController(OrderBook& book)
        : _book(book) {}

/**
 * Submit a new order to the orderbook
 * @param newOrderCmd Order Command Struct
 * @return DeltaUpdates (change) to orderbook state made by the order command
 */
//template <typename orderCmd>
DeltaUpdates OrderController::submit(NewOrderCmd& newOrderCmd) {
        if (newOrderCmd.type == EventType::cancel) {
                newOrderCmd.orderId = _book.getActiveOrderID(newOrderCmd.price);
        }
        //if (newOrderCmd.type != EventType::cancel) {
        else {
                newOrderCmd.orderId = cnt;
                ++cnt;
        }
        Order* newOrder = orderFromCmd(newOrderCmd);
        _book.addOrder(newOrder);
        DeltaUpdates out = _book.publishUpdates();
        if (newOrderCmd.type != EventType::limit) {
                delete newOrder;
        }
        return out;
}

/**
 * Generate Order pointer for the orderbook from OrderCommand Struct
 * @tparam orderCmd Order Command Struct Type (Limit, Market, Cancel, ... )
 * @param cmd Order Command Struct
 * @return Pointer to Order object
 * @note Pointers to Order objects not sitting on the book are owned by OrderController (deleted by submit)
 */
template <typename orderCmd>
Order* OrderController::orderFromCmd(orderCmd& cmd) {
        return new Order(cmd.type, cmd.orderId, cmd.side, cmd.price, cmd.quantity);
}