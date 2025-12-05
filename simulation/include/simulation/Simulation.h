//
// Created by Mark Gagarine on 2025-01-05.
//

#ifndef SIMULATION_H
#define SIMULATION_H

//#include "engine/OrderBook.hpp"
#include "engine/OrderController.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <algorithm> // for std::transform
#include <iterator>  // for std::back_inserter
#include <cmath>

// Number of outstanding orders sitting on the book - different from others which track outstanding quantities
using askQueue = std::map<Price, LevelData, std::less<Price>>;
using bidQueue = std::map<Price, LevelData, std::greater<Price>>;

// struct LevelData {
//     Quantity quantity;
//     int orderCount;
// };

using RNG = std::mt19937;
using UNIF = std::uniform_real_distribution<double>;

using Rate = double;
using Rates = std::vector<Rate>;

/**
 * @brief Inputs to initialize and run LOB simulation
 */
struct Inputs {
    //Inputs();
    Inputs(int ticks, double tickStep, Price priceLevelSize, Price targetSpread, Quantity jumpSize,
        Rates& limitBuyArrivals, Rates& limitSellArrivals, Rate marketBuyArrival, Rate marketSellArrival,
        Rates& limitBuyCancellations, Rates& limitSellCancellations, unsigned int seed);
    int ticks;
    double tickStep;
    Price priceLevelSize;
    Price targetSpread;
    Quantity jumpSize;
    Rates& limitBuyArrivals;
    Rates& limitSellArrivals;
    Rate marketBuyArrival;
    Rate marketSellArrival;
    Rates& limitBuyCancellations;
    Rates& limitSellCancellations;
    unsigned int seed;
//     // Constructors
//     Inputs();
//     /**
//      * @brief Configurable simulation inputs
//      * @param _startingPrice Starting mid-price? Overridden by init vector?
//      * @param _initialOpenOrders Starting mid-price? Overridden by init vector?
//      * @param _ticks number of iterations
//      * @param _tickStep size of time steps
//      * @param _priceLevelSize number of price levels
//      * @param _limitBuyArrivalRates
//      * @param _limitSellArrivalRates
//      * @param _marketBuyArrivalRate
//      * @param _marketSellArrivalRate
//      * @param _limitBuyCancellationRate
//      * @param _limitSellCancellationRate
//      * @param _jumpSize distance between order levels
//      * @param _seed RNG init seed
//      */
//     Inputs(
//         const Price& _startingPrice,    // address??
//         openOrderData& _initialOpenOrders, // const?? address??
//         const int& _ticks,
//         const double& _tickStep,
//         const Price& _priceLevelSize,
//         const std::vector<double>& _limitBuyArrivalRates,
//         const std::vector<double>& _limitSellArrivalRates,
//         const double& _marketBuyArrivalRate,
//         const double& _marketSellArrivalRate,
//         const std::vector<double>& _limitBuyCancellationRate,
//         const std::vector<double>& _limitSellCancellationRate,
//         const int& _jumpSize,
//         const unsigned int& _seed
//     )
//         : startingPrice(_startingPrice)
//         , initialOpenOrders(_initialOpenOrders)
//         , ticks(_ticks)
//         , tickStep(_tickStep)
//         , priceLevelSize(_priceLevelSize)
//         , limitBuyArrivalRates(_limitBuyArrivalRates)
//         , limitSellArrivalRates(_limitSellArrivalRates)
//         , marketBuyArrivalRate(_marketBuyArrivalRate)
//         , marketSellArrivalRate(_marketSellArrivalRate)
//         , limitBuyCancellationRates(_limitBuyCancellationRate)
//         , limitSellCancellationRates(_limitSellCancellationRate)
//         , jumpSize(_jumpSize)
//         , seed(_seed) {}
//
//     Price startingPrice;
//     openOrderData initialOpenOrders;
//     const int ticks;
//     const double tickStep;
//     const Price priceLevelSize;
//     const std::vector<double> limitBuyArrivalRates;
//     const std::vector<double> limitSellArrivalRates;
//     const double marketBuyArrivalRate;
//     const double marketSellArrivalRate;
//     const std::vector<double> limitBuyCancellationRates;
//     const std::vector<double> limitSellCancellationRates;
//     const int jumpSize;
//     const unsigned int seed;
};

/* limit buys          limit sells
 *          0            0
 *      0   0            0   0
 * 0    0   0   spread   0   0   0
 * ---------------------------------
 * 98   99  100        101  102 103
 *  bids                     asks
 */

using MassOrderEntry = std::vector<NewOrderCmd>;


class Simulation {

public:

    //explicit Simulation(const Inputs* inputs);
    explicit Simulation(Inputs& inputs);
    ~Simulation();
    MassOrderEntry step();
    void updateState(const DeltaUpdates& updates);

    // for testing now
    void addToQ(LevelData data, Price price, Side side) {
        if (side == Side::buy) {
            _bids[price] = data;
            std::cout << _bids.size() << std::endl;
        }
        else {
            _asks[price] = data;
            std::cout << _asks.size() << std::endl;
        }
    }

private:

    std::vector<double> sampleUniform(size_t n);
    Rates sampleRates(std::vector<double>& U);

    //Price getSpread() const;

    askQueue _asks;
    bidQueue _bids;
    const int _ticks;
    const double _tickStep;
    const Price _priceLevelSize;
    const Price _targetSpread;
    const Quantity _jumpSize;
    const Rates& _limitBuyArrivals;
    const Rates& _limitSellArrivals;
    const Rate _marketBuyArrival;
    const Rate _marketSellArrival;
    const Rates& _limitBuyCancellations;
    const Rates& _limitSellCancellations;
    const unsigned int _seed;
    RNG gen{_seed};
    size_t _numLevels;

    std::vector<Price> _priceGrid;

    Price _prevBestAsk;     // keep track of price movement directions
    Price _prevBestBid;

    UNIF _unif{0.0, 1.0};
    unsigned int _orderCounter{0};

};
#endif //SIMULATION_H
