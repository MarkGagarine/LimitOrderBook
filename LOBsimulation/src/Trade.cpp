//
// Created by Mark Gagarine on 2025-01-05.
//

#include "simulation/Trade.h"

Trade::Trade(int tradeId)
        : _tradeId(tradeId) {
    // Constructor initialization
}

std::vector<TradeData> Trade::getBuys() const {
    return _buySideData;
}

std::vector<TradeData> Trade::getSells() const {
    return _sellSideData;
}

void Trade::updateTradeData(int orderId, Quantity quantity, Price price, Side side) {

    // initialize new trade data struct
    TradeData newTrade;
    newTrade.orderIds.push_back(orderId);
    newTrade.orderQuantities.push_back(quantity);
    newTrade.orderPrices.push_back(price);

    if (side == Side::buy) {
        _buySideData.push_back(newTrade);
    }
    else {
        _sellSideData.push_back(newTrade);
    }
}