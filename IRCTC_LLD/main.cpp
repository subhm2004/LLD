#include <atomic>
#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "core/IRCTCSystem.h"
#include "enums/BookingStatus.h"
#include "models/Train.h"

using namespace std;
using namespace irctc_lld;

static string bookingStatusToString(BookingStatus status) {
  return status == BookingStatus::CONFIRMED ? "CONFIRMED" : "CANCELLED";
}

static void printTrain(const Train &train) {
  cout << "  " << train.trainNumber << " " << train.name << " on "
       << train.runDate << " | route: ";
  for (size_t i = 0; i < train.stations.size(); ++i) {
    if (i > 0) {
      cout << " -> ";
    }
    cout << train.stations[i];
  }
  cout << "\n";
}

int main() {
  cout << "====== IRCTC LLD Demo ======\n\n";

  IRCTCSystem irctc;
  const string date = "2026-05-25";

  Train rajdhani{"12951",
                 "Mumbai Rajdhani",
                 date,
                 {"BOM", "ST", "BRC", "RTM", "KOTA", "NDLS"}};
  Train rajdhaniReturn{"12952",
                       "Delhi Mumbai Rajdhani",
                       date,
                       {"NDLS", "KOTA", "RTM", "BRC", "ST", "BOM"}};

  irctc.addTrainRun(rajdhani, 4);
  irctc.addTrainRun(rajdhaniReturn, 4);

  const string alice = irctc.registerUser("Alice");
  const string bob = irctc.registerUser("Bob");
  const string carol = irctc.registerUser("Carol");

  cout << "--- Search by source, destination, date ---\n";
  for (const Train *train : irctc.searchByRoute("BOM", "NDLS", date)) {
    printTrain(*train);
  }

  cout << "\n--- Search by train number + date ---\n";
  const Train *byNumber = irctc.searchByTrainNumber("12951", date);
  if (byNumber) {
    printTrain(*byNumber);
  }

  cout << "\n--- Available seats (BOM -> NDLS) before booking ---\n";
  cout << "  Count: "
       << irctc.getAvailableSeatCount("12951", date, "BOM", "NDLS") << "\n";
  for (const string &seat :
       irctc.listAvailableSeats("12951", date, "BOM", "NDLS")) {
    cout << "  " << seat << "\n";
  }

  cout << "\n--- Seat reusability on same physical seat ---\n";
  TicketBooking b1 = irctc.bookTicket(alice, "12951", date, "BOM", "BRC", "S1");
  cout << "  Alice booked " << b1.seatId << " BOM->BRC (" << b1.bookingId
       << ")\n";

  TicketBooking b2 = irctc.bookTicket(bob, "12951", date, "RTM", "NDLS", "S1");
  cout << "  Bob booked same " << b2.seatId << " RTM->NDLS (" << b2.bookingId
       << ")\n";

  cout << "  Available BOM->BRC now: "
       << irctc.getAvailableSeatCount("12951", date, "BOM", "BRC")
       << " (S1 still free for ST-BRC leg)\n";
  cout << "  Available ST->BRC now: "
       << irctc.getAvailableSeatCount("12951", date, "ST", "BRC") << "\n";

  cout << "\n--- Overlapping segment rejected ---\n";
  try {
    irctc.bookTicket(carol, "12951", date, "ST", "KOTA", "S1");
    cout << "  ERROR: overlap should fail\n";
  } catch (const exception &ex) {
    cout << "  Expected: " << ex.what() << "\n";
  }

  cout << "\n--- Concurrent booking (1 seat left on full route) ---\n";
  IRCTCSystem concurrentIrctc;
  Train express{"22691", "Test Express", date, {"BOM", "NDLS"}};
  concurrentIrctc.addTrainRun(express, 1);
  vector<string> racers;
  racers.reserve(8);
  for (int i = 0; i < 8; ++i) {
    racers.push_back(concurrentIrctc.registerUser("Racer_" + to_string(i)));
  }

  atomic<int> successCount{0};
  atomic<int> failureCount{0};
  vector<thread> threads;
  for (int i = 0; i < 8; ++i) {
    threads.emplace_back([&, i]() {
      try {
        concurrentIrctc.bookTicket(racers[static_cast<size_t>(i)], "22691",
                                   date, "BOM", "NDLS");
        ++successCount;
      } catch (...) {
        ++failureCount;
      }
    });
  }
  for (thread &t : threads) {
    t.join();
  }
  cout << "  Success: " << successCount.load()
       << " | Rejected: " << failureCount.load() << " (expected 1 success)\n";

  cout << "\n--- Cancel ticket ---\n";
  irctc.cancelTicket(b1.bookingId, alice);
  const TicketBooking &cancelled = irctc.getBooking(b1.bookingId);
  cout << "  " << b1.bookingId
       << " status=" << bookingStatusToString(cancelled.status) << "\n";
  cout << "  Seats available BOM->BRC after cancel: "
       << irctc.getAvailableSeatCount("12951", date, "BOM", "BRC") << "\n";

  cout << "\nDemo complete ho gya bye bye.\n";
  return 0;
}
