//
// Created by Mark Gagarine on 2025-01-05.
//

//#ifndef LOBSIMULATION_SIMULATION_H
//#define LOBSIMULATION_SIMULATION_H

#include "engine/OrderBook.hpp"
//#include "simulation/Order.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <algorithm> // for std::transform
#include <iterator>  // for std::back_inserter
#include <cmath>

//using OB_ptr = std::unique_ptr<OrderBook>;

// pairs containing price side, with quantity and count
struct levelDataInfo {
    std::pair<Price,LevelData> buys;
    std::pair<Price,LevelData> sells;
};

using openOrderData = std::vector<levelDataInfo*>; // output these as csv?? too much memory??
// or map of all current orders?? maybe not, would be difficult to keep track of mutliple locations

struct message {    // output these as csv?
    //unsigned int time;
    double time;
    EventType type;
    Side side;
    int orderId;
    double price;
    double volume;
};

// remoce _startingPrice
// replace jump size for average order size???

/**
 * @brief Inputs to initialize and run LOB simulation
 */
struct Inputs {
    // Constructors
    Inputs();
    /**
     * @brief Configurable simulation inputs
     * @param _startingPrice Starting mid-price? Overridden by init vector?
     * @param _initialOpenOrders Starting mid-price? Overridden by init vector?
     * @param _ticks number of iterations
     * @param _tickStep size of time steps
     * @param _priceLevelSize number of price levels
     * @param _limitBuyArrivalRates
     * @param _limitSellArrivalRates
     * @param _marketBuyArrivalRate
     * @param _marketSellArrivalRate
     * @param _limitBuyCancellationRate
     * @param _limitSellCancellationRate
     * @param _jumpSize distance between order levels
     * @param _seed RNG init seed
     */
    Inputs(
        const Price& _startingPrice,    // address??
        openOrderData& _initialOpenOrders, // const?? address??
        const int& _ticks,
        const double& _tickStep,
        const Price& _priceLevelSize,
        const std::vector<double>& _limitBuyArrivalRates,
        const std::vector<double>& _limitSellArrivalRates,
        const double& _marketBuyArrivalRate,
        const double& _marketSellArrivalRate,
        const std::vector<double>& _limitBuyCancellationRate,
        const std::vector<double>& _limitSellCancellationRate,
        const int& _jumpSize,
        const unsigned int& _seed
    )
        : startingPrice(_startingPrice)
        , initialOpenOrders(_initialOpenOrders)
        , ticks(_ticks)
        , tickStep(_tickStep)
        , priceLevelSize(_priceLevelSize)
        , limitBuyArrivalRates(_limitBuyArrivalRates)
        , limitSellArrivalRates(_limitSellArrivalRates)
        , marketBuyArrivalRate(_marketBuyArrivalRate)
        , marketSellArrivalRate(_marketSellArrivalRate)
        , limitBuyCancellationRates(_limitBuyCancellationRate)
        , limitSellCancellationRates(_limitSellCancellationRate)
        , jumpSize(_jumpSize)
        , seed(_seed) {}

    Price startingPrice;
    openOrderData initialOpenOrders;
    const int ticks;
    const double tickStep;
    const Price priceLevelSize;
    const std::vector<double> limitBuyArrivalRates;
    const std::vector<double> limitSellArrivalRates;
    const double marketBuyArrivalRate;
    const double marketSellArrivalRate;
    const std::vector<double> limitBuyCancellationRates;
    const std::vector<double> limitSellCancellationRates;
    const int jumpSize;
    const unsigned int seed;
};

/* limit buys          limit sells
 *          0            0
 *      0   0            0   0
 * 0    0   0   spread   0   0   0
 * ---------------------------------
 * 98   99  100        101  102 103
 *  bids                     asks
 */

class Simulation {

public:

    explicit Simulation(const Inputs* inputs);
    ~Simulation();
    void runSimulation();
    Orders step();


private:

    std::vector<double> generateUniforms(size_t n);

    OrderBook* initBook();

    Price _currentPrice;
    openOrderData _currentOpenOrders;
    const int _ticks;
    const double _tickStep;
    const Price _priceLevelSize;
    const std::vector<double> _limitBuyArrivalRates;
    const std::vector<double> _limitSellArrivalRates;
    const double _marketBuyArrivalRate;
    const double _marketSellArrivalRate;
    const std::vector<double> _limitBuyCancellationRates;
    const std::vector<double> _limitSellCancellationRates;
    const int _jumpSize;
    const unsigned int _seed;
    std::mt19937 gen{_seed};

    size_t _numLevels;

    std::uniform_real_distribution<double> unif{0.0, 1.0};
    unsigned int _orderCounter{0};

    OrderBook* _book;

};
//#endif //LOBSIMULATION_SIMULATION_H
