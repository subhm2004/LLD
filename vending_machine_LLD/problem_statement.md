# Problem Statement: Vending Machine System

Design a Low-Level Vending Machine System that can dispense products based on user selection and payment.

The system should support:

- Product inventory management
- Product selection by user
- Payment collection
- Product dispensing
- Change/refund handling

The objective is to design a clean and extensible C++ object-oriented model that can later support multiple payment modes, restocking workflows, and machine states.

## Scope

- In-memory implementation
- Single vending machine instance
- Focus on LLD class design and core transaction flow

## Key Design Objective

Keep inventory, payment, and transaction orchestration loosely coupled so each module can evolve independently.
