# ATM LLD (C++)

Modular low-level design for an in-memory ATM system.

## Structure

- `main.cpp`
- `core/ATMSystem.h`
- `models/BankAccount.h`
- `models/Card.h`
- `models/Transaction.h`
- `services/AuthenticationService.h`
- `services/CashDispenser.h`
- `enums/TransactionType.h`
- `enums/TransactionStatus.h`

## Features

- Card + PIN authentication
- Account balance inquiry
- Cash withdrawal with validations
- Denomination-based note dispensing
- Transaction status tracking

## Run

```bash
g++ -std=c++17 main.cpp -o atm_app
./atm_app
```
