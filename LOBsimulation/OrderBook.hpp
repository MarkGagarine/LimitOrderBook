//
// Created by Mark Gagarine on 2024-12-28.
//

#ifndef LOBSIMULATION_ORDERBOOK_HPP
#define LOBSIMULATION_ORDERBOOK_HPP

#include <vector>
#include <map>

using Price = double;

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

struct PriceLevel {
    Price price;
    int quantity;
};

using LevelData = std::vector<PriceLevel>;

class PriceLevelData {

public:

    PriceLevelData(const LevelData& bids, const LevelData& asks);

    const LevelData& getBids() const;
    const LevelData& getAsks() const;

private:

    const LevelData _bids;
    const LevelData _asks;

};

class Order {
public:
    Order(EventType type, int orderId, Side side, Price price, int quantity);

    EventType getType() const;
    int getOrderId() const;
    Side getSide() const;
    Price getPrice() const;
    int getQuantity() const;
    int getQuantityRemaining() const;
    OrderStatus getOrderStatus() const;

    void fill(int fillQuantity);
    void setQuantityRemaining(int newQuantity);

private:


    EventType _type;
    int _orderId;
    Side _side;
    Price _price;
    int _quantity;
    int _quantityRemaining;
    OrderStatus _status;

};

using Orders = std::vector<Order*>;

class OrderBook {
public:

    OrderBook();

    // getters
    Price getSpread() const;


    // update methods

    void addOrder(Order* newOrder);

    void updatePriceLevel();


private:

    void routeMarketBuy(Order* newOrder);
    void routeMarketSell(Order* newOrder);
    void matchMarketOrder(Order* newOrder, Orders ordersAtLevel);

    void routeLimit(Order* newOrder);
    void routeCancellation(Order* newOrder);

    std::unordered_map<Price, LevelData> _data;
    std::map<Price, Orders, std::greater<Price>> _bids;
    std::map<Price, Orders, std::less<Price>> _asks;


};


class Trade {
public:

    Trade(int tradeId);

private:

    int _tradeId;    // or tradeNo

    // store order ids + qtys as vector
    // begin with market order, then vector[0] can modify big market orders, and append limits to end
    // somehow store avg price of market order filled??
    std::vector<int> _orderIds;
    std::vector<int> _orderQuantities;
    std::vector<Price> _orderPrices;

};

using Trades = std::vector<Trade*>;




#endif //LOBSIMULATION_ORDERBOOK_HPP
