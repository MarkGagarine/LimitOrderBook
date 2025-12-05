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

OrderBook::~OrderBook() {
    for (auto bIt = _bids.begin(); bIt != _bids.end(); bIt++) {
        auto& [price, openBids] = *bIt;
        for (auto bid: openBids) { delete bid; }
    }
    for (auto aIt = _asks.begin(); aIt != _asks.end(); aIt++) {
        auto& [price, openAsks] = *aIt;
        for (auto ask: openAsks) { delete ask; }
    }
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
PriceLevelData OrderBook::getPriceLevelData()  const {
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

int OrderBook::getActiveOrderID(Price price) const {
    int res = -1;
    auto oidIt = _activeOrders.find(price);
    if (oidIt != _activeOrders.end()) {
        res = oidIt->first;
    }
    return res;
}


/**
 * @brief Update the orderbook with a new order
 * @param newOrder Pointer to the new order object
 */
void OrderBook::addOrder(Order *newOrder) {
    switch (newOrder->getType()) {
        case EventType::market:
            if (newOrder->getSide() == Side::buy) {
                routeMarketOrder(newOrder, _asks);
            }
            else {
                routeMarketOrder(newOrder, _bids);
            }
            break;
        case EventType::limit:
            _updateBuffer.addUpdate(newOrder->getType(), newOrder->getOrderId(), newOrder->getQuantity(),
                newOrder->getPrice(), newOrder->getSide());
            if (newOrder->getSide() == Side::buy) {
                routeLimitOrder(newOrder, _bids);
            }
            else {
                routeLimitOrder(newOrder, _asks);
            }
            break;
        case EventType::cancel:
            //routeCancellation(newOrder);
            if (newOrder->getSide() == Side::buy) {
                routeCancelOrder(newOrder, _bids);
            }
            else {
                routeCancelOrder(newOrder, _asks);
            }
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
 *
 * @tparam BookSideMap Bid or Ask maps
 * @param newOrder
 * @param bookSide
 */
template <typename BookSideMap>
void OrderBook::routeMarketOrder(Order* newOrder, BookSideMap& bookSide) {
    for (auto level = bookSide.begin(); level != bookSide.end(); ) { // sweep through all price levels
        if (newOrder->getQuantityRemaining() == 0) {    // exit loop if order is filled
            break;
        }
        auto& ordersAtLevel = level->second;
        matchMarketOrder(newOrder, ordersAtLevel); // look through current level queue
        if (level->second.empty()) { // remove levels with no orders left, moving to next level (so no iter)
            level = bookSide.erase(level);
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

        Price priceKey = currentOrder->getPrice();
        int currOrderID = currentOrder->getOrderId();

        _updateBuffer.addUpdate(newOrder->getType(), currOrderID, fillQuantity,
            currentOrder->getPrice(), currentOrder->getSide());

        _priceLevelData[priceKey].quantity -= fillQuantity;
        //std::cout << "--- Filled " << fillQuantity << " @ $" << currentOrder->getPrice() << "\n";
        if (currentOrder->getQuantityRemaining() == 0) {
            _priceLevelData[priceKey].orderCount--;
            ordersAtLevel.erase(orders);
            //orderIds.erase(currentOrder->getOrderId());
            _activeOrders[priceKey].erase(currOrderID);
            delete currentOrder;
        }
    }
}

/**
 * @brief Execute markey buy order
 * @param newOrder Market buy order object
 */
// void OrderBook::routeMarketBuy(Order *newOrder) {
//     for (auto level = _asks.begin(); level != _asks.end(); ) { // sweep through all price levels
//         if (newOrder->getQuantityRemaining() == 0) {    // exit loop if order is filled
//             break;
//         }
//         auto& ordersAtLevel = level->second;
//         matchMarketOrder(newOrder, ordersAtLevel); // look through current level queue
//         if (level->second.empty()) { // remove levels with no orders left, moving to next level (so no iter)
//             level = _asks.erase(level);
//         }
//     }
// }

/**
 * @brief Execute markey buy sell
 * @param newOrder Market sell order object
 */
// void OrderBook::routeMarketSell(Order *newOrder) {
//     for (auto level = _bids.begin(); level != _bids.end(); ) {
//         // check order isn't filled
//         if (newOrder->getQuantityRemaining() == 0) {
//             break;
//         }
//         auto& ordersAtLevel = level->second;
//         matchMarketOrder(newOrder, ordersAtLevel);
//         // remove levels with no orders left
//         if (level->second.size() == 0) {
//             level = _bids.erase(level);
//         }
//     }
// }

/**
 *
 * @tparam BookSideMap map<price, orderQueue>
 * @param newOrder pointer to new limit order (owned by the book)
 * @param bookSide Ask or Bid Queue Map
 */
template <typename BookSideMap>
void OrderBook::routeLimitOrder(Order* newOrder, BookSideMap& bookSide) {
    Price priceKey = newOrder->getPrice();
    bookSide[priceKey].push_back(newOrder);
    _activeOrders[priceKey].insert(newOrder->getOrderId());
    //orderIds.insert(newOrder->getOrderId());
    _priceLevelData[priceKey].quantity  += newOrder->getQuantity();
    _priceLevelData[priceKey].orderCount++;
}

/**
 * @brief Execute limit order
 * @param newOrder Limit order object
 */
// void OrderBook::routeLimit(Order *newOrder) {
//     if (newOrder->getSide() == Side::buy){
//         // route buy limit
//         auto& q = _bids[newOrder->getPrice()];
//         q.push_back(newOrder);
//         auto it = std::prev(q.end());
//         _orderIndex[newOrder->getOrderId()] = it;
//     }
//     else {
//         auto& q = _asks[newOrder->getPrice()];
//         q.push_back(newOrder);
//         auto it = std::prev(q.end());
//         _orderIndex[newOrder->getOrderId()] = it;
//     }
//     _priceLevelData[newOrder->getPrice()].quantity  += newOrder->getQuantity();
//     _priceLevelData[newOrder->getPrice()].orderCount++;
// }

template <typename BookSideMap>
void OrderBook::routeCancelOrder(Order* newOrder, BookSideMap& bookSide) {
    Price orderPriceKey = newOrder->getPrice();
    int orderIdKey = newOrder->getOrderId();
    _activeOrders[orderPriceKey].erase(orderIdKey);
    Orders& ordersAtLevel = bookSide[orderPriceKey];
    for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {
        Order* order = *orders;
        // delete desired order from queue
        if (order->getOrderId() == orderIdKey) {// update price level data
            _priceLevelData[orderPriceKey].quantity -= newOrder->getQuantityRemaining();
            _priceLevelData[orderPriceKey].orderCount--;
            // update order book
            ordersAtLevel.erase(orders);
            orderIds.erase(orderIdKey);
            delete order;
            break;
        }
        else {
            ++orders;
        }
    }
}

/**
 * @brief Execute order cancellation
 * @param newOrder
 */
// void OrderBook::routeCancellation(Order *newOrder) {
//     if (newOrder->getSide() == Side::buy) {
//         Orders& ordersAtLevel = _bids[newOrder->getPrice()];
//         // iterate through bids until desired order is found
//         for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {
//             Order* order = *orders;
//             // delete desired order from queue
//             if (order->getOrderId() == newOrder->getOrderId()) {
//                 std::cout << "Size before " << ordersAtLevel.size() << "\n";
//                 std::cout<<"Deleting order " << order->getOrderId()<<std::endl;
//                 // update price level data
//                 _priceLevelData[newOrder->getPrice()].quantity -= newOrder->getQuantityRemaining();
//                 _priceLevelData[newOrder->getPrice()].orderCount--;
//                 // update order book
//                 ordersAtLevel.erase(orders);
//                 std::cout << "Size before " << ordersAtLevel.size() << "\n";
//                 // break out of loop
//                 break;
//             }
//             else {
//                 ++orders;
//             }
//         }
//     }
//     else {
//         Orders& ordersAtLevel = _asks[newOrder->getPrice()];
//         // iterate through bids until desired order is found
//         for (auto orders = ordersAtLevel.begin(); orders != ordersAtLevel.end(); ) {
//             Order* order = *orders;
//             // delete desired order from queue
//             if (order->getOrderId() == newOrder->getOrderId()) {
//                 // update price level data
//                 _priceLevelData[newOrder->getPrice()].quantity -= newOrder->getQuantityRemaining();
//                 _priceLevelData[newOrder->getPrice()].orderCount--;
//                 // update order book
//                 orders = ordersAtLevel.erase(orders);
//                 // break out of loop
//                 break;
//             }
//             else {
//                 ++orders;
//             }
//         }
//     }
// }

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