#include <bits/stdc++.h>
#include <iomanip>
#include <iostream>
#include <string>

#include "core/StockExchangeSystem.h"
#include "enums/OrderSide.h"
#include "enums/OrderStatus.h"
#include "enums/OrderType.h"

using namespace std;
using namespace stock_exchange_lld;

static string orderStatusToString(OrderStatus status) {
  switch (status) {
  case OrderStatus::OPEN:
    return "OPEN";
  case OrderStatus::PARTIAL:
    return "PARTIAL";
  case OrderStatus::FILLED:
    return "FILLED";
  case OrderStatus::CANCELLED:
    return "CANCELLED";
  }
  return "UNKNOWN";
}

static void printBook(const string &symbol, StockExchangeSystem &exchange) {
  cout << "  Order book " << symbol << ":\n";
  cout << "    ASKS (low -> high):\n";
  auto asks = exchange.getAskBook(symbol, 5);
  if (asks.empty()) {
    cout << "      (empty)\n";
  }
  for (const OrderBookLevel &level : asks) {
    cout << "      " << fixed << setprecision(2) << level.price << " x "
         << level.totalQuantity << " (" << level.orderCount << " orders)\n";
  }
  cout << "    BIDS (high -> low):\n";
  auto bids = exchange.getBidBook(symbol, 5);
  if (bids.empty()) {
    cout << "      (empty)\n";
  }
  for (const OrderBookLevel &level : bids) {
    cout << "      " << fixed << setprecision(2) << level.price << " x "
         << level.totalQuantity << " (" << level.orderCount << " orders)\n";
  }
}

int main() {
  cout << "====== Stock Exchange LLD Demo ======\n\n";

  StockExchangeSystem exchange;
  const string alice = exchange.registerUser("Alice");
  const string bob = exchange.registerUser("Bob");
  const string carol = exchange.registerUser("Carol");

  exchange.addSymbol("RELIANCE", "Reliance Industries");
  exchange.addSymbol("TCS", "Tata Consultancy Services");

  cout << "--- Seed resting sell orders on RELIANCE ---\n";
  auto sell1 = exchange.placeOrder(bob, "RELIANCE", OrderSide::SELL,
                                   OrderType::LIMIT, 2850.0, 10);
  cout << "  " << sell1.order->orderId << " SELL 10 @ 2850 -> "
       << orderStatusToString(sell1.order->status)
       << " trades=" << sell1.trades.size() << "\n";

  auto sell2 = exchange.placeOrder(carol, "RELIANCE", OrderSide::SELL,
                                   OrderType::LIMIT, 2845.0, 5);
  cout << "  " << sell2.order->orderId << " SELL 5 @ 2845 -> "
       << orderStatusToString(sell2.order->status) << "\n";

  printBook("RELIANCE", exchange);

  cout << "\n--- Limit buy crosses best ask (price-time match) ---\n";
  auto buy1 = exchange.placeOrder(alice, "RELIANCE", OrderSide::BUY,
                                  OrderType::LIMIT, 2850.0, 7);
  cout << "  " << buy1.order->orderId << " BUY 7 @ 2850 -> "
       << orderStatusToString(buy1.order->status)
       << " filled=" << buy1.order->filledQuantity << "\n";
  for (const Trade &t : buy1.trades) {
    cout << "    Trade " << t.tradeId << " " << t.quantity << " @ " << t.price
         << " (buy=" << t.buyOrderId << " sell=" << t.sellOrderId << ")\n";
  }

  printBook("RELIANCE", exchange);

  cout << "\n--- Market buy takes remaining liquidity ---\n";
  auto mktBuy = exchange.placeOrder(alice, "RELIANCE", OrderSide::BUY,
                                    OrderType::MARKET, 0, 6);
  cout << "  " << mktBuy.order->orderId << " MARKET BUY 6 -> "
       << orderStatusToString(mktBuy.order->status)
       << " trades=" << mktBuy.trades.size() << "\n";

  cout << "\n--- Cancel open order ---\n";
  auto resting = exchange.placeOrder(bob, "RELIANCE", OrderSide::SELL,
                                     OrderType::LIMIT, 2900.0, 4);
  cout << "  Placed " << resting.order->orderId
       << " status=" << orderStatusToString(resting.order->status) << "\n";
  exchange.cancelOrder(resting.order->orderId, bob);
  cout << "  After cancel: "
       << orderStatusToString(exchange.getOrder(resting.order->orderId)->status)
       << "\n";

  cout << "\n--- TCS: partial fill example ---\n";
  exchange.placeOrder(carol, "TCS", OrderSide::SELL, OrderType::LIMIT, 4100.0,
                      20);
  auto tcsBuy = exchange.placeOrder(alice, "TCS", OrderSide::BUY,
                                    OrderType::LIMIT, 4100.0, 8);
  cout << "  " << tcsBuy.order->orderId
       << " filled=" << tcsBuy.order->filledQuantity << "/"
       << tcsBuy.order->quantity
       << " status=" << orderStatusToString(tcsBuy.order->status) << "\n";
  printBook("TCS", exchange);

  cout << "\n--- All trades ---\n";
  for (const Trade &t : exchange.getAllTrades()) {
    cout << "  " << t.tradeId << " " << t.symbol << " " << t.quantity << " @ "
         << t.price << "\n";
  }

  cout << "\nDemo complete.\n";
  return 0;
}
