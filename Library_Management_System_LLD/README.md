# Library Management System LLD (C++)

Modular low-level design for a simple in-memory library system.

## Folder Structure

- `main.cpp`
- `core/LibraryManagementSystem.h`
- `services/CatalogService.h`
- `services/FineService.h`
- `models/Book.h`
- `models/Member.h`
- `models/Loan.h`
- `enums/BookStatus.h`
- `enums/AccountStatus.h`

## Highlights

- Custom namespace: `library_mgmt`
- Header guards using `#ifndef / #define / #endif`
- Book issue and return flow
- Late fine calculation
- Search books by title

## Run

```bash
g++ -std=c++17 main.cpp -o library_app
./library_app
```
