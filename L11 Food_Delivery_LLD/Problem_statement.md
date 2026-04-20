# Problem Statement: Online Food Ordering System (Tomato App)

## 1. Context
The current food delivery market requires a robust, scalable, and efficient system to handle the lifecycle of an order—from restaurant discovery to final delivery/pickup and payment. The goal of this project is to implement a **Low-Level Design (LLD)** for an application named 'Tomato' that manages these complexities using Object-Oriented Programming (OOP) principles and Design Patterns.

## 2. Core Problems to Solve

### A. Diversity in Order Types
The system must support different types of orders, specifically:
* **Delivery Orders**: Requires a delivery address and logistics management.
* **Pickup Orders**: Requires the restaurant's location for the user to collect the food.
* **Scheduled Orders**: Orders placed for a future time slot.
* **Now Orders**: Orders to be processed immediately.

### B. Scalable Payment Processing
Users should have multiple payment options (Credit Card, UPI, NetBanking). The system needs to be flexible enough to add new payment methods in the future without modifying existing order logic (**Open/Closed Principle**).

### C. Restaurant and Menu Management
Efficiently managing a large catalog of restaurants, each with its own menu, pricing, and location. The system must allow users to search and select items seamlessly.

### D. System Orchestration (The "Fat" Controller Problem)
Preventing the main application logic from becoming a "God Object" that is too complex to maintain. The system needs a clear entry point (**Facade Pattern**) to coordinate between Managers, Services, and Factories.

## 3. Functional Requirements
1.  **Search**: Users can search for restaurants by name or location.
2.  **Cart Management**: Users can add/remove items from a cart linked to a specific restaurant.
3.  **Order Placement**: Support for creating both 'Now' and 'Scheduled' orders for 'Delivery' or 'Pickup'.
4.  **Payment**: Integration of various payment strategies.
5.  **Notifications**: Notify users about order status updates.

## 4. Design Constraints & Patterns Used
* **Creational**: Factory Pattern for generating different types of Orders (Delivery vs Pickup) based on timing (Now vs Scheduled).
* **Structural**: Facade Pattern (Tomato Class) to simplify interactions with the underlying subsystem (OrderManager, RestaurantManager, etc.).
* **Behavioral**: Strategy Pattern for interchangeable payment methods.
* **Concurrency**: Handling multiple tasks simultaneously using C++ Multi-threading (e.g., processing notifications while handling UI).

## 5. Technical Stack
* **Language**: C++17 or higher.
* **Concurrency**: `std::thread`, `std::mutex`, `std::atomic` for thread-safe operations.
* **Architecture**: Modular LLD with clear separation between Models, Managers, Strategies, and Factories.