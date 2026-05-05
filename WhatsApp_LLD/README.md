# WhatsApp LLD

## Overview
This module contains Low Level Design for a WhatsApp-like messaging system.

## Scope
- 1:1 messaging
- Group messaging
- Message delivery states
- Basic media/message abstractions

## Project Structure
- `models/` : core entities
- `services/` : business logic
- `enums/` : enums/constants
- `core/` : orchestrators/system-level classes

## Build & Run
```bash
g++ -std=c++17 main.cpp -o whatsapp
./whatsapp
```
