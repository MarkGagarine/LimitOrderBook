//
// Created by Mark Gagarine on 2025-11-19.
//

//#include <map>
#include <iostream>
#include <string>
#include "cli/CliApp.h"
#include "cli/AsciiRenderer.h"
#include "engine/OrderBook.hpp"

struct NewOrderCmd {
    NewOrderCmd(EventType type, Side side, int orderId, Price price, Quantity quantity) {
        this->type = type;
        this->side = side;
        this->orderId = orderId;
        this->price = price;
        this->quantity = quantity;
    }
    EventType type;
    Side side;
    int orderId;
    Price price;
    Quantity quantity;
};

struct MarketOrderCmd : NewOrderCmd {
    MarketOrderCmd(Side side, Quantity quantity)
        : NewOrderCmd(EventType::market, side, 0, 0.0, quantity) {
    };
};

struct LimitOrderCmd : NewOrderCmd {
    LimitOrderCmd(Side side, Price price, Quantity quantity)
        : NewOrderCmd(EventType::limit, side, 0, price, quantity) {
    }
};

struct CancelOrderCmd : NewOrderCmd {
    CancelOrderCmd(Side side, int orderId, Price price)
        : NewOrderCmd(EventType::cancel, side, orderId, price, 0.0) {
    }
};

// temp here for now
class OrderController {
public:
    OrderController(OrderBook& book)
        : _book(book) {
    };
    //~OrderController();

    template <typename orderCmd>
    DeltaUpdates submit(const orderCmd& newOrderCmd) {
        Order* newOrder = orderFromCmd(newOrderCmd);
        _book.addOrder(newOrder);
        DeltaUpdates out = _book.publishUpdates();
        if (newOrderCmd.type != EventType::limit) {
            delete newOrder;
        }
        ++cnt;
        return out;
    }

private:
    template <typename orderCmd>
    Order* orderFromCmd(orderCmd& cmd) {
        return new Order(cmd.type, cmd.orderId, cmd.side, cmd.price, cmd.quantity);
    }
    OrderBook& _book;
    int cnt {0};
};


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

void CliApp::runSimulatedInteractive() {
    clearScreen();
    std::cout << "Running Interactive LOB" << std::endl;
}

void CliApp::runMonteCarlo() {
    clearScreen();
    std::cout << "Running Monte Carlo" << std::endl;
}
