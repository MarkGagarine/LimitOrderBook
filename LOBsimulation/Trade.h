//
// Created by Mark Gagarine on 2025-01-05.
//

#ifndef LOBSIMULATION_TRADE_H
#define LOBSIMULATION_TRADE_H

#include <vector>

using Price = double;
using Quantity = int;

enum class Side {
    buy,
    sell
};

struct TradeData {
    std::vector<int> orderIds;
    std::vector<Quantity> orderQuantities;
    std::vector<Price> orderPrices;
};


class Trade {
public:

    Trade(int tradeId);

    std::vector<TradeData> getBuys() const;
    std::vector<TradeData> getSells() const;

    //void updateTradeData(TradeData newTrade, Side side);
    void updateTradeData(int orderId, Quantity quantity, Price price, Side side);

private:

    int _tradeId;    // or tradeNo

    // store order ids + qtys as vector
    // begin with market order, then vector[0] can modify big market orders, and append limits to end
    // somehow store avg price of market order filled??

    std::vector<TradeData> _buySideData;    // make these TradeData pointers???
    std::vector<TradeData> _sellSideData;   // these too??

};

using Trades = std::vector<Trade*>;




#endif //LOBSIMULATION_TRADE_H
