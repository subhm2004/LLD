#ifndef MOVIE_TICKET_BOOKING_SYSTEM_DEVICE_KIOSKTERMINAL_H
#define MOVIE_TICKET_BOOKING_SYSTEM_DEVICE_KIOSKTERMINAL_H

#include <iostream>
#include "../core/MovieTicketSystem.h"

namespace movie_ticket_booking {

class KioskTerminal {
public:
    explicit KioskTerminal(MovieTicketSystem* system) : system_(system) {}

    void printSystemReady() const {
        std::cout << "Kiosk ready for booking operations.\n";
    }

private:
    MovieTicketSystem* system_;
};

}

#endif
