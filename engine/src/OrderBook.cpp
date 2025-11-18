//
// Created by Mark Gagarine on 2024-12-29.

#include "engine/OrderBook.hpp"
#include <iostream>

Updates::Updates() {
    cnt = 0;
}

void Updates::addUpdate(EventType type, unsigned int orderId, Quantity quantity, double price, Side side) {
    auto d = std::make_unique<Delta>();
    d->type = type;
    d->orderId = orderId;
    d->quantity = quantity;
    d->price = price;
    d->side = side;
    this->deltas.push_back(std::move(d));
    this->cnt++;
}

void Updates::addUpdate(Order* newOrder) {
    auto d = std::make_unique<Delta>();
    d->type = newOrder->getType();
    d->orderId = newOrder->getOrderId();
    d->quantity = newOrder->getQuantityFilled();   // filled quantity
    d->price = newOrder->getPrice();
    d->side = newOrder->getSide();
    this->deltas.push_back(std::move(d));
    this->cnt++;
}

/**
 * @brief The Limit Order Book
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
Price OrderBook::getBestQuote(Side side) const {    // TODO: Update empty side logic, should be opposite + spread?
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
std::map<Price, LevelData, std::greater<Price>> OrderBook::getPriceLevelData()  const {
    return _priceLevelData;
}

/**
 * @brief Retrieve all current order ID's
 * @return Set of all orderId's currently within the order book
 */
std::set<int> OrderBook::getOrderIds() const {
    return orderIds;
}

/**
 * @brief Retrieve a current order id; to be used when generating order cancellations
 * @return value of first available order id
 */
int OrderBook::getTopOrderId() const {
    if (orderIds.empty()) {
        return 0;
    }
    auto currentOrderId = orderIds.begin();
    return *currentOrderId;
}

/**
 * @brief Update the orderbook with a new order
 * @param newOrder Pointer to the new order object
 */
void OrderBook::addOrder(Order *newOrder) {

    switch (newOrder->getType()) {
        case EventType::market:
            std::cout << "Routing market order" << std::endl;
            if (newOrder->getSide() == Side::buy) {
                routeMarketBuy(newOrder);
            }
            else {
                routeMarketSell(newOrder);
            }
            break;
        case EventType::limit:
            std::cout << "Routing limit order" << std::endl;
            _updateBuffer.addUpdate(newOrder->getType(), newOrder->getOrderId(), newOrder->getQuantity(),
                newOrder->getPrice(), newOrder->getSide());
            routeLimit(newOrder);
            break;
        case EventType::cancel:
            _updateBuffer.addUpdate(newOrder->getType(), newOrder->getOrderId(), newOrder->getQuantity(),
                newOrder->getPrice(), newOrder->getSide());
            routeCancellation(newOrder);
            break;
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

/**
 * @brief Match a new market order to current open limit orders
 * @param newOrder New market order to be filled (fully or partially)
 * @param ordersAtLevel Current open limit orders
 */
void OrderBook::matchMarketOrder(Order *newOrder, Orders& ordersAtLevel) {

    for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {

        if (newOrder->getQuantityRemaining() == 0) {
            break;
        }
        Order* currentOrder = *orders;
        Quantity fillQuantity = std::min(newOrder->getQuantityRemaining(), currentOrder->getQuantityRemaining());
        currentOrder->fill(fillQuantity);
        newOrder->fill(fillQuantity);

        _updateBuffer.addUpdate(currentOrder->getType(), currentOrder->getOrderId(), fillQuantity,
            currentOrder->getPrice(), currentOrder->getSide());

        _priceLevelData[currentOrder->getPrice()].quantity -= fillQuantity;
        _priceLevelData[currentOrder->getPrice()].orderCount--;
        std::cout << "--- Filled " << fillQuantity << " @ $" << currentOrder->getPrice() << "\n";
        // check if current limit order needs to be removed from queue (it's filled)
        if (currentOrder->getQuantityRemaining() == 0) {
            orders = ordersAtLevel.erase(orders);
        }
    }
}


/**
 * @brief Execute markey buy order
 * @param newOrder Market buy order object
 */
void OrderBook::routeMarketBuy(Order *newOrder) {
    // for (auto ask : _asks){
    //     std::cout << "::: " << ask.second.size() << " quotes available @ $" << ask.first << "\n";
    // }
    for (auto level = _asks.begin(); level != _asks.end(); ) { // sweep through all price levels
        if (newOrder->getQuantityRemaining() == 0) {    // exit loop if order is filled
            break;
        }
        auto& ordersAtLevel = level->second;
        matchMarketOrder(newOrder, ordersAtLevel); // look through current level queue
        if (level->second.empty()) { // remove levels with no orders left, moving to next level (so no iter)
            level = _asks.erase(level);
        }
    }
}

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

    if (newOrder->getSide() == Side::buy){
        // route buy limit
        _bids[newOrder->getPrice()].push_back(newOrder);
        // update price level info
        _priceLevelData[newOrder->getPrice()].quantity += newOrder->getQuantity();
        _priceLevelData[newOrder->getPrice()].orderCount++;
        //std::cout<<"added order\n";
    }
    else {
        // check sell is not below minimum
        // ---!!!!
        // route sell limit
        _asks[newOrder->getPrice()].push_back(newOrder);
        // update price level info
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


/**
 * @brief Create a copy of updates made to the orderbook state since last call
 * @return vector of unique pointers containing info of changes made (fills, sells, etc)
 */
DeltaUpdates OrderBook::publishUpdates() {
    DeltaUpdates result;
    result.reserve(_updateBuffer.cnt);
    for (auto& u: _updateBuffer.deltas) {
        result.emplace_back(std::move(u));
    }
    _updateBuffer.cnt = 0;
    _updateBuffer.deltas.clear();
    return result;
}