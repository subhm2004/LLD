# ISP Followed (Interface Segregation Principle)

## Overview
This program demonstrates the **Interface Segregation Principle (ISP)** by splitting shape behavior into focused interfaces:

- `Two_Dimensional_Shape` for 2D area computation
- `Three_Dimensional_Shape` for 3D area and volume computation

Clients/classes only depend on methods they actually use.

## Structure
- `Two_Dimensional_Shape`
  - `area()`
- `Three_Dimensional_Shape`
  - `area()`
  - `volume()`
- `Square` implements only `Two_Dimensional_Shape`
- `Rectangle` implements only `Two_Dimensional_Shape`
- `Cube` implements `Three_Dimensional_Shape`

## Why This Follows ISP
- 2D classes (`Square`, `Rectangle`) are not forced to implement `volume()`.
- 3D class (`Cube`) implements both area and volume as needed.
- Interfaces remain small and role-specific, reducing unnecessary coupling.

## Flow in `main()`
1. Create `Square`, `Rectangle`, and `Cube` through interface pointers.
2. Print area for all shapes.
3. Print volume only for the 3D shape (`Cube`).

## Benefit
This design keeps contracts clean, prevents dummy/unused method implementations, and improves maintainability.
