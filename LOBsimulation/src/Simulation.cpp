//
// Created by Mark Gagarine on 2025-01-05.
//

#include "Simulation.h"


Simulation::Simulation(Price startingPrice, int ticks, Price priceLevelSize)
    : _startingPrice(startingPrice)
    , _ticks(ticks)
    //, _priceLevelsCnt(priceLevelsCnt)
    , _priceLevelSize(priceLevelSize)   {
    // constructor initialization

}

void Simulation::setLimitBuyArrivalRates(std::vector<double> arrivalRates) {
    _limitBuyArrivalRates = arrivalRates;

/*
    _limitBuyArrivalRates.reserve(arrivalRates.size());
    std::transform(arrivalRates.begin(),
                   arrivalRates.end(),
                   std::back_inserter(_limitBuyArrivalRates),
                   [](double lambda) {
                       return EXP(lambda);
                   });
    initEXP(arrivalRates, _limitBuyArrivalRates);
*/
}

void Simulation::setLimitSellArrivalRates(std::vector<double> arrivalRates) {
    _limitSellArrivalRates = arrivalRates;
    /*_limitSellArrivalRates.reserve(arrivalRates.size());
    initEXP(arrivalRates, _limitSellArrivalRates);
*/
}

void Simulation::setMarketBuyArrivalRate(double arrivalRate) {
    _marketBuyArrivalRate = arrivalRate;

    //_marketBuyArrivalRate = EXP(arrivalRate);
}

void Simulation::setMarketSellArrivalRate(double arrivalRate) {
    _marketSellArrivalRate = arrivalRate;
    //_marketSellArrivalRate = EXP(arrivalRate);
}

void Simulation::setLimitBuyCancellationRates(std::vector<double> arrivalRates) {
    _limitBuyCancellationRates = arrivalRates;
/*
    _limitBuyCancellationRates.reserve(arrivalRates.size());
    initEXP(arrivalRates, _limitBuyCancellationRates);
*/
}

void Simulation::setLimitSellCancellationRates(std::vector<double> arrivalRates) {
    _limitSellCancellationRates = arrivalRates;
/*
    _limitSellCancellationRates.reserve(arrivalRates.size());
    initEXP(arrivalRates, _limitSellCancellationRates);
*/
}

void Simulation::setSeed(unsigned int seed) {
    _seed = seed;
}

/*
void Simulation::initEXP(std::vector<double>& rates, std::vector<EXP>& exps) {

    std::transform(rates.begin(),
                   rates.end(),
                   std::back_inserter(exps),
                   [](double lambda) {
                       return EXP(lambda);
                   });
}*/

double Simulation::EXP(double rate, std::mt19937 &gen) {
    return -log(std::uniform_real_distribution<>(0.,1.)(gen)) / rate;
}

//Quantity generateQuantity();


// sample double,
// look at int values
// if multiple orders arrive at same int value -> rank by actual arrival times
// execute arrival times with int 0
// decrement observed arrival time by ticks


void Simulation::runSimulation() {
    // set seed
    std::mt19937 gen(_seed);

    // initialize order Id's
    _orderCounter = 0;

    // init simulation time step
    int time = 0;

    // init Orderbook
    OrderBook _OB = OrderBook();

    // initialize price levels
    std::vector<int> initLevels = {1,2,3,3,2,1};
    /*      top 3 are best asks , bottom 3 are best bids
    for (int i = 0; i < 6; ++i){
        initLevels[i] = UNIF(1, 3 + initLevels[i])(gen) * initLevels[i];
        std::cout << initLevels[i] << "\n";
    }   */

    for (int level_size = 0; level_size < 6; ++level_size) {

        if (level_size < 3) {
            // add asks             // start price + spread + level depth
            Price currOrderPrice = _startingPrice + _priceLevelSize + (3 - initLevels[level_size]);

            // sample sample number of orders
            int ordercnt = UNIF(1, 3 + initLevels[level_size])(gen) * initLevels[level_size];

            for (int k = 0; k < ordercnt; ++k) {
                // generate order quantity
                Quantity qty = 5 * UNIF(1,5)(gen);
                // add the order to LOB
                Order newOrder = Order(EventType::limit, _orderCounter, Side::sell, currOrderPrice, qty);
                _OB.addOrder(&newOrder);
                std::cout << "$" << currOrderPrice  << " ,QTY: " << qty << "\n";
                ++_orderCounter;
            }
        }
        else {
            Price currOrderPrice = _startingPrice - (3 - initLevels[level_size]);
            // sample sample number of orders
            int ordercnt = UNIF(1, 3 + initLevels[level_size])(gen) * initLevels[level_size];

            for (int k = 0; k < ordercnt; ++k) {
                // generate order quantity
                Quantity qty = 5 * UNIF(1, 5)(gen);
                // add the order to LOB

                Order newOrder = Order(EventType::limit, _orderCounter, Side::buy, currOrderPrice, qty);
                _OB.addOrder(&newOrder);
                std::cout << "$" << currOrderPrice << " ,QTY: " << qty << "\n";
                ++_orderCounter;
            }
        }
    }


    std::map<Price, LevelData, std::greater<Price>> dat = _OB.getPriceLevelData();

    std::cout << "----Initialized OrderBook----\n";
    for (auto level : dat){
        std::cout << level.second.orderCount << " orders with " << level.second.quantity << " available @ $" << level.first << std::endl;
    }

    std::cout << "\n The spread is $" << _OB.getSpread() << "\n";

    std::cout << "\nThe lowest ask is $" << _OB.getBestQuote(Side::buy);
    std::cout << "\nThe highest bid is $" << _OB.getBestQuote(Side::sell) << "\n";

    /*for (int i = 0; i < 10; ++i) {
        std::cout << EXP(3., gen) << "\n";
    }*/

    _limitBuyTimes.reserve(3);
    _limitSellTimes.reserve(3);
    _cancelBuyTimes.reserve(3);
    _cancelSellTimes.reserve(3);

    // initial sweep of sampling times
    for (int i = 0; i < 3; ++i) {

        _limitBuyTimes[i] = EXP(_limitBuyArrivalRates[i], gen);
        _limitSellTimes[i] = EXP(_limitSellArrivalRates[i], gen);
        _cancelBuyTimes[i] = EXP(_limitBuyCancellationRates[i], gen);        // make rate propotional to size of current ask queue
        _cancelSellTimes[i] = EXP(_limitSellCancellationRates[i], gen);
/*
        std::cout << " time till limit buy  " << _limitBuyTimes[i] << "\n";
        std::cout << " time till limit sell " << _limitSellTimes[i]<< "\n";
        std::cout << " time till cancel buy " << _cancelBuyTimes[i]<< "\n";
        std::cout << "time till cancel sell " << _cancelSellTimes[i]<< "\n";
  */
    }

    _marketBuyTime = EXP(_marketBuyArrivalRate, gen);
    _marketSellTime = EXP(_marketSellArrivalRate, gen);

    // initialize ordered map to store orders to execute (ordered by time)
    // next

    /*
    while (time < _ticks) {

        // check which arrival times < 1 -> exectuted within this time step

        for (int i = 0; i < 3; ++i) {
            if(_limitBuyTimes[i] <= 1) {

                // sample next time step
                _limitBuyTimes[i] = EXP(_limitBuyArrivalRates[i], gen);

                // generate new limit buy order
            }

            --_limitSellTimes[i];
            --_cancelBuyTimes[i];
            --_cancelSellTimes[i];

        }

        // increment to next time step
        ++time;
    }
        */


}
