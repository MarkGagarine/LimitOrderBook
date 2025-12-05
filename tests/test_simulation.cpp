//
// Created by Mark Gagarine on 2025-11-21.
//


#include <simulation/Simulation.h>


int main() {
  Rates limitBuys = {1.85, 1.51, 1.09, 0.88, 0.77};
  Rates limitSells = {1.85, 1.51, 1.09, 0.88, 0.77};
  Rate marketBuy = 0.94;
  Rate marketSell = 0.94;
  Rates cancelBuys = {0.71, 0.81, 0.68, 0.56, 0.47};
  Rates cancelSells = {0.71, 0.81, 0.68, 0.56, 0.47};

  int ticks = 100;
  //Price tickStep = 0.1;
  Price tickStep = 0.05;
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

  OrderBook book;
  OrderController ctrl(book);
  std::vector<int> cts = {
     5,  7, 9,11,15,
    17, 19,11,13, 2
  };
  std::vector<Price> prices = {
    105, 104, 103, 102, 101,
    100,  99,  98,  97,  96
  };
  //std::vector<LimitOrderCmd> limits;
  std::vector<DeltaUpdates> updates;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < cts[i]; ++j) {
      //limits.emplace_back(Side::sell, prices[i], cts[i] * jumpSize);
      LimitOrderCmd cmd = {Side::sell, prices[i], cts[i] * jumpSize};
      DeltaUpdates update = ctrl.submit(cmd);
      sim.updateState(update);
      //updates.push_back(ctrl.submit(cmd));
    }
  }
  for (int i = 5; i < cts.size(); ++i) {
    for (int j = 0; j < cts[i]; ++j) {
      LimitOrderCmd cmd = {Side::buy, prices[i], cts[i] * jumpSize};
      DeltaUpdates update = ctrl.submit(cmd);
      sim.updateState(update);
    }
  }

  //sim.step();

  for (int t = 0; t < 100; ++t) {
    MassOrderEntry newOrders = sim.step();
    for (auto newOrder : newOrders) {
      DeltaUpdates update = ctrl.submit(newOrder);
      sim.updateState(update);
    }
  }

  return 0;
}