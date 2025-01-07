//
// Created by Mark Gagarine on 2025-01-05.
//

#ifndef LOBSIMULATION_SIMULATION_H
#define LOBSIMULATION_SIMULATION_H

#include <vector>
#include <map>
#include <set>

using Price = double;
using Quantity = int;


class Simulation {

public:

    Simulation(int ticks, int _priceLevelsCnt, Price priceLevelSize);

    void runSimulation();

private:

    int _ticks;
    int _priceLevelsCnt;
    Price _priceLevelSize;

    std::set<Price> _priceLevels;

    std::vector<int> _arrivals;
    std::vector<Quantity> _tradeSizes;

    std::vector<double> _limitBuyArrivalRates;
    std::vector<double> _limitSellArrivalRates;
    double _marketBuyArrivalRate;
    double _marketSellArrivalRate;
    std::vector<double> _limitBuyCancellationRates;
    std::vector<double> _limitSellCancellationRates;
    double _marketBuyCancellationRate;
    double _marketSellCancellationRate;

    double _cancellationProportion;

};


#endif //LOBSIMULATION_SIMULATION_H
