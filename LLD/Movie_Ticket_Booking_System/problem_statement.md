# Problem Statement: Movie Ticket Booking System

Design a Low-Level Movie Ticket Booking System that allows users to browse shows and book seats for a movie.

The system should support:

- Movie and show management
- Seat layout and seat availability tracking
- Seat selection and booking creation
- Payment processing integration
- Booking confirmation notification

The objective is to build a modular C++ design with clear separation of concerns across models, managers, strategies, and external integrations.

## Scope

- In-memory implementation
- Single process simulation
- Focus on LLD and booking flow correctness

## Key Design Objective

Design the booking pipeline so seat locking, pricing, payment, and notifications can evolve independently without affecting core entities.
