//
// Created by Mark Gagarine on 2025-11-19.
//

#ifndef ASCIIRENDERER_H
#define ASCIIRENDERER_H

#include "engine/OrderBook.hpp"
//#include <map>

//// Common Visualization functions
void renderMenu();
void renderBook(const std::map<Price, Quantity, std::greater<Price>>& bids, const std::map<Price, Quantity, std::less<Price>>&  asks);
void renderOrderInput();

Side fetchSide();
Quantity fetchQuantity();
Price fetchPrice();

void clearScreen();
void renderHeader();


#endif //ASCIIRENDERER_H
