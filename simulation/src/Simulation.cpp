//
// Created by Mark Gagarine on 2025-01-05.

#include "simulation/Simulation.h"
using namespace std;




Inputs::Inputs(int ticks, double tickStep, Price priceLevelSize, Price targetSpread, Quantity jumpSize,
        Rates& limitBuyArrivals, Rates& limitSellArrivals, Rate marketBuyArrival, Rate marketSellArrival,
        Rates& limitBuyCancellations, Rates& limitSellCancellations, unsigned int seed)
            : ticks(ticks)
            , tickStep(tickStep)
            , priceLevelSize(priceLevelSize)
            , targetSpread(targetSpread)
            , jumpSize(jumpSize)
            , limitBuyArrivals(limitBuyArrivals)
            , limitSellArrivals(limitSellArrivals)
            , marketBuyArrival(marketBuyArrival)
            , marketSellArrival(marketSellArrival)
            , limitBuyCancellations(limitBuyCancellations)
            , limitSellCancellations(limitSellCancellations)
            , seed(seed) {
    // Constructor initialization
}

/**
 * @brief LOB Simulation constructor
 * @param inputs Struct containing simulation configuration parameters
 */
// Simulation::Simulation(const Inputs* inputs)
//     : _currentPrice(inputs->startingPrice)
//     , _currentOpenOrders(inputs->initialOpenOrders)
//     , _ticks(inputs->ticks)
//     , _tickStep(inputs->tickStep)
//     , _priceLevelSize(inputs->priceLevelSize)
//     , _limitBuyArrivalRates(inputs->limitBuyArrivalRates)
//     , _limitSellArrivalRates(inputs->limitSellArrivalRates)
//     , _marketBuyArrivalRate(inputs->marketBuyArrivalRate)
//     , _marketSellArrivalRate(inputs->marketSellArrivalRate)
//     , _limitBuyCancellationRates(inputs->limitBuyCancellationRates)
//     , _limitSellCancellationRates(inputs->limitSellCancellationRates)
//     , _jumpSize(inputs->jumpSize)
//     , _seed(inputs->seed) {
//     // constructor initialization + sanity checks
//     const size_t _limitBuyArrivalRatesLength = _limitBuyArrivalRates.size();
//     const size_t _limitSellArrivalRatesLength = _limitBuyArrivalRates.size();
//     if (_limitBuyArrivalRatesLength != _limitBuyCancellationRates.size()) {
//         throw length_error("Limit buy arrival rates must have same number of cancellations");
//     }
//     if (_limitSellArrivalRatesLength != _limitSellCancellationRates.size()) {
//         throw length_error("Limit sell arrival rates must have same number of cancellations");
//     }
//     if (_limitBuyArrivalRatesLength != _limitSellArrivalRatesLength) {
//         throw length_error("Depth of orderbook must be of equal size for bids and asks");
//     }
//     // initialize values
//     _numLevels = _limitBuyArrivalRatesLength;
//
//     _book = initBook();
// }



// MassOrderCmd::MassOrderCmd(LimitOrderCmd NewLimitOrder);
// MassOrderCmd::MassOrderCmd(CancelOrderCmd NewCancelOrder);

Simulation::Simulation(Inputs& inputs)
        : _ticks(inputs.ticks)
        , _tickStep(inputs.tickStep)
        , _priceLevelSize(inputs.priceLevelSize)
        , _targetSpread(inputs.targetSpread)
        , _jumpSize(inputs.jumpSize)
        , _limitBuyArrivals(inputs.limitBuyArrivals)
        , _limitSellArrivals(inputs.limitSellArrivals)
        , _marketBuyArrival(inputs.marketBuyArrival)
        , _marketSellArrival(inputs.marketSellArrival)
        , _limitBuyCancellations(inputs.limitBuyCancellations)
        , _limitSellCancellations(inputs.limitSellCancellations)
        , _seed(inputs.seed) {
    // constructor initialization + sanity checks
    size_t _limitBuyArrivalsLength = _limitBuyArrivals.size();
    size_t _limitSellArrivalsLength = _limitBuyArrivals.size();
    if (_limitBuyArrivalsLength != _limitBuyCancellations.size()) {
        throw length_error("Limit buy arrival rates must have same number of cancellations");
    }
    if (_limitSellArrivalsLength != _limitSellCancellations.size()) {
        throw length_error("Limit sell arrival rates must have same number of cancellations");
    }
    if (_limitBuyArrivalsLength != _limitSellArrivalsLength) {
        throw length_error("Depth of orderbook must be of equal size for bids and asks");
    }
    // initialize values
    _numLevels = _limitBuyArrivalsLength * 4; // count of all limit arrival rates

    int priceGridRange = 1000;
    _priceGrid.reserve(priceGridRange);   // init first 1000 evenly spaced price levels
    for (int i = 0; i < priceGridRange; i++) {
        _priceGrid.emplace_back(i * _priceLevelSize);
    }
    cout << "Starting Price Grid Range: $" << _priceGrid[0] << " - $" << _priceGrid[999] << endl;

    _prevBestAsk = 0;
    _prevBestBid = 0;

}

Simulation::~Simulation() {
    _asks.clear();
    _bids.clear();
}

template <typename sideQueue>
void updateStateQueue(const unique_ptr<Delta>& update, sideQueue& queue) {
    if (update->type == EventType::limit) {
    queue[update->price].orderCount += 1;
        queue[update->price].quantity += update->quantity;
    }
    else {
        queue[update->price].orderCount -= 1;
        queue[update->price].quantity -= update->quantity;
    }
}

/**
 * Update Simulation State with new OrderBook Updates
 * @param updates change in orderbook state
 */
void Simulation::updateState(const DeltaUpdates& updates) {
    for (const auto& update : updates) {
        if (update->side == Side::buy) {
            updateStateQueue(update, _bids);
            // temp for now
            _prevBestBid = _bids.begin()->first;
        }
        else {
            updateStateQueue(update, _asks);
            _prevBestAsk = _asks.begin()->first;
        }
    }
}


/**
 * @brief Generate vector of n uniform random variables
 * @param n size of resulting vector
 * @return vector containing n iid random uniforms
 */
vector<double> Simulation::sampleUniform(size_t n) {
    vector<double> result(n);
    for (size_t i = 0; i < n; i++) {
        result[i] = _unif(gen);
    }
    return result;
}

/**
 * @brief Iterate towards next simulation step
 * @return Orders generated for next iteration
 */
MassOrderEntry Simulation::step() {
    // check spread first
    // ...
    MassOrderEntry newOrders;

    auto aIt = _asks.begin();
    auto bIt = _bids.begin();

    Price ithAsk = aIt->first;
    Price ithBid = bIt->first;

    Quantity askQty;
    Quantity bidQty;
    Price askJump = ithAsk - _prevBestAsk;
    Price bidJump = _prevBestBid - ithBid;

    if (askJump >= _priceLevelSize) {
        int askLevelJumps =  std::floor(askJump / _priceLevelSize);
        ithBid += askLevelJumps * _priceLevelSize;
        bidQty = 0;
    }
    if (bidJump >= _priceLevelSize) {
        int askLevelJumps =  std::floor(bidJump / _priceLevelSize);
        ithAsk -= askLevelJumps * _priceLevelSize;
        askQty = 0;
    }
    // handle spread > target??
    Price spread = ithAsk - ithBid;

    //cout << "Bid: " << ithBid << " ,Ask: " << ithAsk << endl;

    vector<double> U = sampleUniform(_numLevels + 2);
    for (int i = 0; i < _numLevels; i += 4) {
        if ((ithAsk < _prevBestAsk) || (aIt == _asks.end())) {    // jump down occured
            askQty = 0;
        }
        else {
            ithAsk = aIt->first;
            askQty = aIt->second.quantity;
            ++aIt;
        }
        if (U[i] < _limitSellArrivals[i] * _tickStep) {  // U < \lambda \delta_t
            // generate limit buy at ith price level opposite of best ask
            newOrders.push_back(LimitOrderCmd(Side::sell, ithAsk, _jumpSize));
            cout << "LIMIT SELL @ $" << ithAsk << ";";
        }
        if ((askQty> 0) && (U[i + 1] < _limitSellCancellations[i] * _tickStep)) {
            // generate limit buy at ith price level opposite of best ask
            newOrders.push_back(CancelOrderCmd(Side::sell, 0, _jumpSize)); // NEED ORDER ID!!
            cout << "CANCEL SELL @ $" << ithAsk << ";";
        }

        if ((ithBid > _prevBestBid) || (bIt == _bids.end())) {    // jump up occured
            bidQty = 0;
        }
        else {
            ithBid = bIt->first;
            bidQty = bIt->second.quantity;
            ++bIt;
        }
        if (U[i + 2] < _limitBuyArrivals[i] * _tickStep) {  // U < \lambda \delta_t
            // generate limit buy at ith price level opposite of best ask
            newOrders.push_back(LimitOrderCmd(Side::buy, ithBid, _jumpSize)); // NEED ORDER ID!!
            cout << "LIMIT BUY @ $" << ithBid << ";";
        }
        if ((bidQty > 0) && (U[i + 3] < _limitBuyCancellations[i] * _tickStep)) {
            // generate limit buy at ith price level opposite of best ask
            newOrders.push_back(CancelOrderCmd(Side::buy, 0, _jumpSize)); // NEED ORDER ID!!
            cout << "CANCEL BUY @ $" << ithBid << ";";
        }
        if (ithAsk < _prevBestAsk) {
            ithAsk += _priceLevelSize;
        }
        if (ithBid > _prevBestBid) {
            ithBid -= _priceLevelSize;
        }
    }
    if (U[_numLevels] < _marketBuyArrival * _tickStep) {
        newOrders.push_back(MarketOrderCmd(Side::buy, _jumpSize));
        cout << "MARKET BUY"  << ";";
    }
    if (U[_numLevels + 1] < _marketSellArrival * _tickStep) {
        newOrders.push_back(MarketOrderCmd(Side::sell, _jumpSize));
        cout << "MARKET SELL" << ";";
    }
    cout << "\n";
    return newOrders;
}