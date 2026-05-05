# Problem Statement: Ride Sharing App

Design a Low-Level Ride Sharing application similar to Uber/Ola.

The system should support:

- Rider and driver onboarding
- Ride request from pickup to drop location
- Driver discovery and assignment
- Ride lifecycle transitions (accepted, started, completed)
- Fare calculation

The objective is to design a modular C++ object-oriented model that clearly separates entity state, matching logic, pricing, and ride orchestration.

## Scope

- In-memory implementation
- Single process simulation
- Focus on LLD and core ride flow

## Key Design Objective

Keep matching, pricing, and lifecycle management decoupled so surge pricing, cancellations, ratings, and trip history can be added later.
