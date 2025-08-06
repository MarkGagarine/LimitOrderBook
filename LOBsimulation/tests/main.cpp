#include <iostream>

#include "simulation/OrderBook.hpp"
#include "simulation/Simulation.h"

int main() {

/*
    OrderBook NYSE = OrderBook() ;

    //std::cout << "What would you like to trade?" << std::endl;

    //std:: cout << ""

    Order order21 = Order(EventType::limit, 21, Side::sell, 102, 7);
    NYSE.addOrder(&order21);

    Order order211 = Order(EventType::limit, 211, Side::sell, 101, 9);
    NYSE.addOrder(&order211);

    Order order2 = Order(EventType::limit, 2, Side::sell, 101, 15);
    NYSE.addOrder(&order2);

    Order order1 = Order(EventType::limit, 1, Side::buy, 100, 20);
    //std::cout << "Selling " << order1.getQuantity() << " shares @ $" << order1.getPrice() << std::endl;
    NYSE.addOrder(&order1);

    Order order12 = Order(EventType::limit, 12, Side::buy, 99, 12);
    NYSE.addOrder(&order12);

    //Order order11 = Order(EventType::cancel, 1, Side::sell, 100, 10);
    //NYSE.addOrder(&order11);

    //Order order2 = Order(EventType::market, 2, Side::sell, 100, 10);
    //NYSE.addOrder(&order2);


    std::map<Price, LevelData, std::greater<Price>> data = NYSE.getPriceLevelData();

    for (auto level : data){
        std::cout << level.second.quantity << " available @ $" << level.first << std::endl;
    }

    Order order3 = Order(EventType::market, 3, Side::buy, 100, 25);
    NYSE.addOrder(&order3);

     data = NYSE.getPriceLevelData();

    for (auto level : data){
        std::cout << level.second.quantity << " available @ $" << level.first << std::endl;
    }
    */


    // sim stars here


    Price startPrice = 100.;
    Price spread = 1.;

    /*
    std::mt19937 gen(100);

    std::vector<double> arrivalRates = {1., 1.75, 2.5};; //{.15, .2, .25};      // ---->>> smaller rate gives bigger time
    std::vector<EXP> limitBuyArrivalRates;//.reserve(arrivalRates.size());
    limitBuyArrivalRates.reserve(arrivalRates.size());

    std::transform(arrivalRates.begin(),
                   arrivalRates.end(),
                   std::back_inserter(limitBuyArrivalRates),
                   [](double lambda) {
                       return EXP(lambda);
                   });

    for (auto& j : arrivalRates) {
        EXP currRate = limitBuyArrivalRates[j];
        for (int i = 1; i <= 10; ++i) {
            std::cout << i << " -> " << currRate(gen) << "\n";
        }
    }
     comment here   */

    std::vector<double> limitBuyArrivalRates = {.15, .2, .25};
    std::vector<double> limitSellArrivalRates = {.25, .2, .15};
    double marketBuyArrivalRate = 0.5;
    double marketSellArrivalRate = 0.5;
    std::vector<double> limitBuyCancellationRates = {1., 1.75, 2.5};
    std::vector<double> limitSellCancellationRates = {2.5, 1.75, 1.};

    /*
    std::vector<EXP> _limitBuyArrivalRates;
    std::vector<EXP> _limitSellArrivalRates;
    EXP _marketBuyArrivalRate;
    EXP _marketSellArrivalRate;
    std::vector<EXP> _limitBuyCancellationRates;
    std::vector<EXP> _limitSellCancellationRates;

     \\comment here */


    Simulation sim = Simulation(startPrice, 100000, spread);

    sim.setLimitBuyArrivalRates(limitBuyArrivalRates);
    sim.setLimitSellArrivalRates(limitSellArrivalRates);
    sim.setMarketBuyArrivalRate(marketBuyArrivalRate);
    sim.setMarketSellArrivalRate(marketSellArrivalRate);
    sim.setLimitBuyCancellationRates(limitBuyCancellationRates);
    sim.setLimitSellCancellationRates(limitBuyCancellationRates);

    unsigned int seed = 100;

    sim.setSeed(seed);

    sim.runSimulation();


    return 0;
}
