//
// Created by Mark Gagarine on 2025-08-06.
//
// File to test Order class

#include <iostream>
#include <string>
#include <cassert>
#include "simulation/Order.h"

using namespace std;

int main() {
    cout << "Testing Order class..." << endl;

    cout << "Testing getters..." << endl;
    Order test1(EventType::market, 69, Side::buy, 100.0, 3);
    assert(test1.getOrderId() == 69);
    assert(test1.getSide() == Side::buy);
    assert(test1.getPrice() == 100.0);
    assert(test1.getQuantity() == 3);
    cout << "Getters tests passed" << endl;

    cout << "Testing error handling..." << endl;
    Order* test2;
    try {
        test2 = new Order(EventType::market, 69, Side::buy, 100.0, -3000);
    }
    catch (const invalid_argument& e) {
        const string exp_msg = "Quantity cannot be negative";
        const string obs_msg = e.what();
        assert(exp_msg == obs_msg);
    }
    try {
        test2 = new Order(EventType::market, 69, Side::buy, -100.0, 3000);
    }
    catch (const invalid_argument& e) {
        const string exp_msg = "Price cannot be negative";
        const string obs_msg = e.what();
        assert(exp_msg == obs_msg);
    }
    cout << "...Finished testing Order class" << endl;
    return 0;
}