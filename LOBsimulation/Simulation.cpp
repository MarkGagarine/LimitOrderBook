//
// Created by Mark Gagarine on 2025-01-05.
//

#include "Simulation.h"


Simulation::Simulation(int ticks, int priceLevelsCnt, Price priceLevelSize)
    : _ticks(ticks)
    , _priceLevelsCnt(priceLevelsCnt)
    , _priceLevelSize(priceLevelSize)   {
    // constructor initialization

}


void Simulation::runSimulation() {
    int time = 0;

    // initial sweep of sampling times

    while (time < _ticks) {

    }
}