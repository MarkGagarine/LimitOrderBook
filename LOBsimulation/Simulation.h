//
// Created by Mark Gagarine on 2025-01-05.
//

#ifndef LOBSIMULATION_SIMULATION_H
#define LOBSIMULATION_SIMULATION_H

#include "OrderBook.hpp"

#include <iostream>

#include <vector>
#include <map>
#include <set>

#include <random>
#include <algorithm> // for std::transform
#include <iterator>  // for std::back_inserter

using Price = double;
using Quantity = int;
using Time = double;

//using EXP = std::exponential_distribution<double>;
using NORM = std::normal_distribution<double>;
using UNIF = std::uniform_int_distribution<>;

using queue = std::vector<Time>;

class Simulation {

public:

    Simulation(Price startingPrice, int ticks, Price priceLevelSize);

    void runSimulation();
    Quantity generateQuantity();

    void setLimitBuyArrivalRates(std::vector<double> arrivalRates);
    void setLimitSellArrivalRates(std::vector<double> arrivalRates);
    void setMarketBuyArrivalRate(double arrivalRate);
    void setMarketSellArrivalRate(double arrivalRate);
    void setLimitBuyCancellationRates(std::vector<double> arrivalRates);
    void setLimitSellCancellationRates(std::vector<double> arrivalRates);

    void setSeed(unsigned int seed);

    double EXP(double rate, std::mt19937 &gen); // std lib EXP does not work for rate >= 3??????



private:

    //void initEXP(std::vector<double>& rates, std::vector<EXP>& exps);

    Price _startingPrice;
    int _ticks;
   // int _priceLevelsCnt;
    Price _priceLevelSize;


    std::set<Price> _priceLevels;

    std::vector<int> _arrivals;
    std::vector<Quantity> _tradeSizes;

    /*
     */
    std::vector<double> _limitBuyArrivalRates;
    std::vector<double> _limitSellArrivalRates;
    double _marketBuyArrivalRate;
    double _marketSellArrivalRate;
    std::vector<double> _limitBuyCancellationRates;
    std::vector<double> _limitSellCancellationRates;
    double _marketBuyCancellationRate;
    double _marketSellCancellationRate;

    /*
    double _cancellationProportion;

    std::vector<EXP> _limitBuyArrivalRates;
    std::vector<EXP> _limitSellArrivalRates;
    EXP _marketBuyArrivalRate;
    EXP _marketSellArrivalRate;
    std::vector<EXP> _limitBuyCancellationRates;    // these need to incorporate size of level queue
    std::vector<EXP> _limitSellCancellationRates;
*/

    int _orderCounter;

    unsigned int _seed;

    /* limit buys          limit sells
     *          0           0
     *      0   0           0   0
     * 0    0   0   spread  0   0   0
     * ---------------------------------
     * 98   99  100       101  102 103
     *  bids                    asks
     */

    // vector of length 3 holding level queue times
    queue _limitBuyTimes;  // furthest -> spread
    queue _limitSellTimes; // spread -> furthest

    queue _cancelBuyTimes;
    queue _cancelSellTimes;

    Time _marketBuyTime;
    Time _marketSellTime;

    /*
    std::vector<queue> _limitBuyTimes;  // furthest -> spread
    std::vector<queue> _limitSellTimes; // spread -> furthest

    std::vector<queue> _cancelBuyTimes;
    std::vector<queue> _cancelSellTimes;

    queue _limitBuyLevel1Times;
    queue _limitBuyLevel2Times;
    queue _limitBuyLevel3Times;
    queue _limitSellLevel1Times;
    queue _limitSellLevel2Times;
    queue _limitSellLevel3Times;

    queue _cancelBuyLevel1Times;
    queue _cancelBuyLevel2Times;
    queue _cancelBuyLevel3Times;
    queue _cancelSellLevel1Times;
    queue _cancelSellLevel2Times;
    queue _cancelSellLevel3Times;
*/



};


#endif //LOBSIMULATION_SIMULATION_H
