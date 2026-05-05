#include <bits/stdc++.h>

#include "core/MovieTicketSystem.h"
#include "device/KioskTerminal.h"
#include "models/Movie.h"
#include "models/Screen.h"
#include "models/Show.h"
#include "models/User.h"

using namespace movie_ticket_booking;
using namespace std;

int main() {
  MovieTicketSystem *system = new MovieTicketSystem();
  KioskTerminal kiosk(system);
  kiosk.printSystemReady();

  Movie movie("M1", "Interstellar", 169);
  system->catalogManager().addMovie(movie);

  Screen screen("SCR_1");
  screen.addSeat(Seat("A1", 1, 1, SeatType::REGULAR));
  screen.addSeat(Seat("A2", 1, 2, SeatType::PREMIUM));
  screen.addSeat(Seat("A3", 1, 3, SeatType::RECLINER));

  Show show("S1", movie, "7:30 PM");
  show.setSeats(screen.getSeats());
  system->showManager().addShow(show);

  User user("U1", "Shubham", "shubham@mail.com");
  Show &targetShow = system->showManager().getShowById("S1");

  Booking *booking =
      system->bookingManager().createBooking(user, targetShow, {"A1", "A2"});

  cout << "Booking ID: " << booking->getId() << "\n";
  cout << "Booked Seats Count: " << booking->getSeatIds().size() << "\n";
  cout << "Total Amount: " << booking->getTotalAmount() << "\n";
  delete system;
  delete booking;
  return 0;
}
