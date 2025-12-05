//
// Created by Mark Gagarine on 2025-11-19.
//

//#include <map>
#include <iostream>
#include "cli/CliApp.h"
#include "cli/AsciiRenderer.h"
//#include "engine/OrderBook.hpp"
#include "engine/OrderController.hpp"
#include "simulation/Simulation.h"



CliApp::CliApp() {
}

CliApp::~CliApp() {
}

void CliApp::run() {
    renderHeader();
    bool shouldQuit = false;
    while (!shouldQuit) {
        renderMenu();
        char choice;
        std::cin >> choice;
        switch (choice) {
            case '1': runBasicInteractive();        break;
            case '2': runSimulatedInteractive();    break;
            case '3': runMonteCarlo();              break;
            case 'q': shouldQuit = true;            break;
        }
    }
}

void CliApp::runBasicInteractive() {
    Delta d0;
    DeltaUpdates d00;
    //clearScreen();
    OrderBook book;         // don't need additional stuff???
    OrderController ctrl(book);
    // init with example?? OrderController ctrl(book, initOrders);
    //PriceLevelData pld = book.getPriceLevelData();
    std::map<Price, Quantity, std::less<Price>> asks;
    std::map<Price, Quantity, std::greater<Price>> bids;
    Side side;
    Price price;
    Quantity quantity;
    bool shouldQuit = false;
    while (!shouldQuit) {
        renderBook(bids, asks);
        renderOrderInput();
        char choice;
        std::cin >> choice;
        switch (choice) {
            case 'q': shouldQuit = true; break;
            case 'm': {
                side = fetchSide();
                quantity = fetchQuantity();
                MarketOrderCmd o(side, quantity);
                DeltaUpdates deltas = ctrl.submit(o);
                for (auto& d: deltas) {
                    if (side == Side::buy) {
                        asks[d->price] -= d->quantity;
                    }
                    else {
                        bids[d->price] -= d->quantity;
                    }
                }
                break;
            }
            case 'l': {
                side = fetchSide();
                quantity = fetchQuantity();
                price = fetchPrice();
                LimitOrderCmd o(side, price, quantity);
                DeltaUpdates deltas = ctrl.submit(o);
                for (auto& d: deltas) {
                    if (side == Side::buy) {
                        bids[d->price] += d->quantity;
                    }
                    else {
                        asks[d->price] += d->quantity;
                    }
                }
                break;
            }
            case 'c':
                std::cout << "Order ID: ";
                int oId;
                std::cin >> oId;
                break;
        }
    }
    clearScreen();
}

template <typename sideQueue>
void updateStateQueue(const std::unique_ptr<Delta>& update, sideQueue& queue) {
    if (update->type == EventType::limit) {
        queue[update->price] += update->quantity;
    }
    else {
        queue[update->price] -= update->quantity;
    }
}

void CliApp::runSimulatedInteractive() {
    clearScreen();
    std::cout << "Running Interactive LOB" << std::endl;

    OrderBook book;         // don't need additional stuff???
    OrderController ctrl(book);

    Rates limitBuys = {1.85, 1.51, 1.09, 0.88, 0.77};
    Rates limitSells = {1.85, 1.51, 1.09, 0.88, 0.77};
    Rate marketBuy = 0.94;
    Rate marketSell = 0.94;
    Rates cancelBuys = {0.71, 0.81, 0.68, 0.56, 0.47};
    Rates cancelSells = {0.71, 0.81, 0.68, 0.56, 0.47};

    int ticks = 100;
    double tickStep = 0.1;
    //Price tickStep = 0.05;
    Price priceSize = 1.0;
    Price targetSpread = 1.0;
    Quantity jumpSize = 10;
    unsigned int seed = 420;

    Inputs inputs(ticks,
      tickStep,
      priceSize,
      targetSpread,
      jumpSize,
      limitBuys,
      limitSells,
      marketBuy,
      marketSell,
      cancelBuys,
      cancelSells,
      seed
      );
    Simulation sim(inputs);

    std::map<Price, Quantity, std::less<Price>> asks;
    std::map<Price, Quantity, std::greater<Price>> bids;

    std::vector<int> cts = {
        5,  7, 9,11,15,
       17, 19,11,13, 2
     };
    std::vector<Price> prices = {
        105, 104, 103, 102, 101,
        100,  99,  98,  97,  96
      };
    std::vector<DeltaUpdates> updates;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < cts[i]; ++j) {
            LimitOrderCmd cmd = {Side::sell, prices[i], cts[i] * jumpSize};
            DeltaUpdates update = ctrl.submit(cmd);
            sim.updateState(update);
            asks[prices[i]] = cts[i] * jumpSize;
        }
    }
    for (int i = 5; i < cts.size(); ++i) {
        for (int j = 0; j < cts[i]; ++j) {
            LimitOrderCmd cmd = {Side::buy, prices[i], cts[i] * jumpSize};
            DeltaUpdates update = ctrl.submit(cmd);
            sim.updateState(update);
            bids[prices[i]] = cts[i] * jumpSize;
        }
    }
    renderBook(bids, asks);
    bool quit = false;
    int t = 0;
    while (t < ticks) {
        std::cin >> quit;
        if (quit) {
            break;
        }
        clearScreen();
        MassOrderEntry newOrders = sim.step();
        for (auto newOrder : newOrders) {
            DeltaUpdates updates = ctrl.submit(newOrder);
            sim.updateState(updates);
            for (const auto& update : updates) {
                if (update->side == Side::buy) {
                    updateStateQueue(update, bids);
                }
                else {
                    updateStateQueue(update, asks);
                }
            }
        }
        renderBook(bids, asks);
    }
}

void CliApp::runMonteCarlo() {
    clearScreen();
    std::cout << "Running Monte Carlo" << std::endl;
}
