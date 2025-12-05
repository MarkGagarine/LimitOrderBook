//
// Created by Mark Gagarine on 2025-12-04.
//

#include "engine/OrderController.hpp"


int main() {

  OrderBook book;
  OrderController ctrl(book);

  LimitOrderCmd order_0 = (LimitOrderCmd(Side::buy, 100, 10));
  ctrl.submit(order_0);  // oid = 0



  return 0;
}