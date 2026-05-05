# Vending Machine LLD (C++)

This folder contains a Low-Level Design implementation of a vending machine system.

## Components

- `Item`: item metadata (name, price, type)
- `Inventory`: stock management for items
- `MoneyManager`: inserted balance handling
- `VendingMachine`: item selection, payment, dispense, and refund flow

## Features

- Item loading and stock tracking
- Item selection with stock validation
- Money insertion and balance management
- Dispense item with change return
- Cancel transaction with full refund

## Structure

- `main.cpp`
- `core/VendingMachine.h`
- `services/Inventory.h`
- `services/MoneyManager.h`
- `models/Item.h`
- `enums/ItemType.h`

## Run

```bash
g++ -std=c++17 main.cpp -o vending_app
./vending_app
```
