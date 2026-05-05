# Abstract Factory Pattern (`AbstractFactory.cpp`)

## Intent
Provide an interface for creating families of related products without specifying their concrete classes.

## Structure In This File
- **Abstract products**
  - `Burger`
  - `GarlicBread`
- **Concrete products**
  - Regular family:
    - Burgers: `BasicBurger`, `StandardBurger`, `PremiumBurger`
    - Breads: `BasicGarlicBread`, `CheeseGarlicBread`
  - Wheat family:
    - Burgers: `BasicWheatBurger`, `StandardWheatBurger`, `PremiumWheatBurger`
    - Breads: `BasicWheatGarlicBread`, `CheeseWheatGarlicBread`
- **Abstract factory**
  - `MealFactory` with:
    - `createBurger(type)`
    - `createGarlicBread(type)`
- **Concrete factories**
  - `SinghBurger` creates regular family products.
  - `KingBurger` creates wheat family products.

## Flow
1. Client selects a concrete `MealFactory` implementation.
2. Client requests multiple related products through the same factory.
3. Factory returns products from the same family.
4. Client uses abstract interfaces (`Burger*`, `GarlicBread*`).

## Pros
- Guarantees consistency of related products.
- Strong decoupling from concrete classes.
- Easy to switch full product families at runtime.

## Cons
- More complex hierarchy.
- Adding a new product category (new abstract product) requires changing all concrete factories.

## Mermaid
See: `AbstractFactory.mmd`
