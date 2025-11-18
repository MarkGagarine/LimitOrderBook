//
// Created by Mark Gagarine on 2025-01-05.

#include "simulation/Simulation.h"
using namespace std;


/**
 * @brief LOB Simulation constructor
 * @param inputs Struct containing simulation configuration parameters
 */
Simulation::Simulation(const Inputs* inputs)
    : _currentPrice(inputs->startingPrice)
    , _currentOpenOrders(inputs->initialOpenOrders)
    , _ticks(inputs->ticks)
    , _tickStep(inputs->tickStep)
    , _priceLevelSize(inputs->priceLevelSize)
    , _limitBuyArrivalRates(inputs->limitBuyArrivalRates)
    , _limitSellArrivalRates(inputs->limitSellArrivalRates)
    , _marketBuyArrivalRate(inputs->marketBuyArrivalRate)
    , _marketSellArrivalRate(inputs->marketSellArrivalRate)
    , _limitBuyCancellationRates(inputs->limitBuyCancellationRates)
    , _limitSellCancellationRates(inputs->limitSellCancellationRates)
    , _jumpSize(inputs->jumpSize)
    , _seed(inputs->seed) {
    // constructor initialization + sanity checks
    const size_t _limitBuyArrivalRatesLength = _limitBuyArrivalRates.size();
    const size_t _limitSellArrivalRatesLength = _limitBuyArrivalRates.size();
    if (_limitBuyArrivalRatesLength != _limitBuyCancellationRates.size()) {
        throw length_error("Limit buy arrival rates must have same number of cancellations");
    }
    if (_limitSellArrivalRatesLength != _limitSellCancellationRates.size()) {
        throw length_error("Limit sell arrival rates must have same number of cancellations");
    }
    if (_limitBuyArrivalRatesLength != _limitSellArrivalRatesLength) {
        throw length_error("Depth of orderbook must be of equal size for bids and asks");
    }
    // initialize values
    _numLevels = _limitBuyArrivalRatesLength;

    _book = initBook();
}

Simulation::~Simulation() {
    if (_book) {
        // cleanup
        try {
            delete _book;
        }
        catch (const exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}


OrderBook* Simulation::initBook() {
    OrderBook* book = new OrderBook();
    // fill levels
    for (auto level = _currentOpenOrders.begin(); level != _currentOpenOrders.begin(); ++level) {
        levelDataInfo* data = *level;

    }


    return book;
}



/**
 * @brief Initialize arrival rates for limit buy orders
 * @param arrivalRates Vector of arrival rates starting from closest to opposite side descending
 */
//void Simulation::setLimitBuyArrivalRates(const std::vector<double>& arrivalRates) {
  //  _limitBuyArrivalRates = arrivalRates;
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
//}

/**
 * @brief Initialize arrival rates for limit sell orders
 * @param arrivalRates Vector of arrival rates starting from closest to opposite side ascending
 */
//void Simulation::setLimitSellArrivalRates(std::vector<double> arrivalRates) {
  //  _limitSellArrivalRates = arrivalRates;
    /*_limitSellArrivalRates.reserve(arrivalRates.size());
    initEXP(arrivalRates, _limitSellArrivalRates);
*/
//}
/*
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
}
*/

//void Simulation::setLimitSellCancellationRates(std::vector<double> arrivalRates) {
  //  _limitSellCancellationRates = arrivalRates;
/*
    _limitSellCancellationRates.reserve(arrivalRates.size());
    initEXP(arrivalRates, _limitSellCancellationRates);
*/
//}

/*void Simulation::setSeed(unsigned int seed) {
    _seed = seed;
}*/

/*
void Simulation::initEXP(std::vector<double>& rates, std::vector<EXP>& exps) {

    std::transform(rates.begin(),
                   rates.end(),
                   std::back_inserter(exps),
                   [](double lambda) {
                       return EXP(lambda);
                   });
}*/

//double Simulation::EXP(double rate, std::mt19937 &gen) {
//    return -log(std::uniform_real_distribution<>(0.,1.)(gen)) / rate;
//}

//Quantity generateQuantity();

/**
 * @brief Generate vector of n uniform random variables
 * @param n size of resulting vector
 * @return vector containing n iid random uniforms
 */
std::vector<double> Simulation::generateUniforms(size_t n) {
    std::vector<double> result(n);
    for (size_t i = 0; i < n; i++) {
        result[i] = unif(gen);
    }
    return result;
}


/**
 * @brief Iterate towards next simulation step
 * @return Orders generated for next iteration
 */
Orders Simulation::step() {
    Orders orders;
    std::vector<double> rates = generateUniforms(_numLevels);
    // new limit buy

    // limit sells

    return orders;
}



/**
 * @brief Simulate Limit Order Book dynamics by sampling time steps of independent Poisson processes
 */
void Simulation::runSimulation() {
    // size_t rateCount = 4 * _limitBuyArrivalRates.size() + 2;
    // vector<double> rates;
    //
    // rates.insert(rates.end(), _limitBuyArrivalRates.begin(), _limitBuyArrivalRates.end());
    //
    //
    // for (auto& rate : rates) {
    //     cout << "rate: " << rate << endl;
    // }


    int t = 0;

    while (t < _ticks) {
        // generate next step
        // -- Tie-breaker order (Market > Cancel > Limit) ( for now -- add randomness later)
        //Orders newOrders = step();


        ++t;
    }



    /*
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
    }*/
    /*
    std::map<Price, LevelData, std::greater<Price>> dat = _OB.getPriceLevelData();
    std::cout << "----Initialized OrderBook----\n";
    for (auto level : dat){
        std::cout << level.second.orderCount << " orders with " << level.second.quantity << " available @ $" << level.first << std::endl;
    }
    std::cout << "\n The spread is $" << _OB.getSpread() << "\n";
    std::cout << "\nThe lowest ask is $" << _OB.getBestQuote(Side::buy);
    std::cout << "\nThe highest bid is $" << _OB.getBestQuote(Side::sell) << "\n";

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
*/
/*
        std::cout << " time till limit buy  " << _limitBuyTimes[i] << "\n";
        std::cout << " time till limit sell " << _limitSellTimes[i]<< "\n";
        std::cout << " time till cancel buy " << _cancelBuyTimes[i]<< "\n";
        std::cout << "time till cancel sell " << _cancelSellTimes[i]<< "\n";
    }
  */

    cout << "Simulation Complete" << endl;
}
