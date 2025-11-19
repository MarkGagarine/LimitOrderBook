//
// Created by Mark Gagarine on 2025-11-19.
//


#include <iostream>
#include <string>
#include "cli/AsciiRenderer.h"

using namespace std;

void renderMenu() {
    cout << "Choose an option:\n"
        << "1) Basic solo LOB\n"
        << "2) Interactive simulation\n"
        << "3) Monte Carlo\n"
        << "q) Quit\n";
}

void renderBook(const map<Price, Quantity, greater<Price>>& bids, const map<Price, Quantity, less<Price>> & asks) {
    clearScreen();
    for (auto aIt = asks.rbegin(); aIt != asks.rend(); aIt++) {
        cout << "\033[31m" << "$ " << aIt->first << " x " << aIt->second << "\033[0m" << "\n";
    }
    cout << "---------------\n";
    for (auto bIt = bids.begin(); bIt != bids.end(); bIt++) {
        cout << "\033[94m" << "$ " << bIt->first << " x " << bIt->second << "\033[0m" << "\n";
    }
    cout.flush();
}

void renderOrderInput() {
    cout << "Place new [m]arket, [l]imit, [c]ancel order, or [q]uit\n";
}

Side fetchSide() {
    cout << "\033[94m[b]uy \033[0m or \033[31m [s]ell\033[0m: ";
    char side;
    std::cin >> side;
    switch (side) {
        case 'b': return Side::buy;
        case 's': return Side::sell;
    }
}

Quantity fetchQuantity() {
    Quantity quantity;
    cout << "Quantity: ";
    cin >> quantity;
    return quantity;
}

Price fetchPrice() {
    Price price;
    cout << "Price: $";
    cin >> price;
    return price;
}

void clearScreen() {
    cout << "\033[2J\033[H"; // clear + move cursor home
}

void renderHeader() {
    cout << "    __        __                __    __       ______                   __                      _______                       __" << endl;
    cout <<     "   |  \\      |  \\              |  \\  |  \\     /      \\                 |  \\                    |       \\                     |  \\" << endl;
    cout <<     "   | $$       \\$$ ______ ____   \\$$ _| $$_   |  $$$$$$\\  ______    ____| $$  ______    ______  | $$$$$$$\\  ______    ______  | $$   __" << endl;
    cout <<     "   | $$      |  \\|      \\    \\ |  \\|   $$ \\  | $$  | $$ /      \\  /      $$ /      \\  /      \\ | $$__/ $$ /      \\  /      \\ | $$  /  \\" << endl;
    cout <<     "   | $$      | $$| $$$$$$\\$$$$\\| $$ \\$$$$$$  | $$  | $$|  $$$$$$\\|  $$$$$$$|  $$$$$$\\|  $$$$$$\\| $$    $$|  $$$$$$\\|  $$$$$$\\| $$_/  $$" << endl;
    cout <<     "   | $$      | $$| $$ | $$ | $$| $$  | $$ __ | $$  | $$| $$   \\$$| $$  | $$| $$    $$| $$   \\$$| $$$$$$$\\| $$  | $$| $$  | $$| $$   $$" << endl;
    cout <<     "   | $$_____ | $$| $$ | $$ | $$| $$  | $$|  \\| $$__/ $$| $$      | $$__| $$| $$$$$$$$| $$      | $$__/ $$| $$__/ $$| $$__/ $$| $$$$$$\\" << endl;
    cout <<     "   | $$     \\| $$| $$ | $$ | $$| $$   \\$$  $$ \\$$    $$| $$       \\$$    $$ \\$$     \\| $$      | $$    $$ \\$$    $$ \\$$    $$| $$  \\$$\\" << endl;
    cout <<     "    \\$$$$$$$$ \\$$ \\$$  \\$$  \\$$ \\$$    \\$$$$   \\$$$$$$  \\$$        \\$$$$$$$  \\$$$$$$$ \\$$       \\$$$$$$$   \\$$$$$$   \\$$$$$$  \\$$   \\$$" << endl;
}

