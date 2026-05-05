# Simple Factory Pattern (`SimpleFactory.cpp`)

## Intent
Encapsulate object creation logic for related products (`Burger` variants) inside a single factory class so client code does not instantiate concrete product classes directly.

## Structure In This File
- `Burger` is an abstract product interface with `prepare()`.
- `BasicBurger`, `StandardBurger`, `PremiumBurger` are concrete products.
- `BurgerFactory` owns the decision logic in `createBurger(type)`.
- Client (`main`) asks factory for a `Burger*` using a string type.

## Flow
1. Client sets `type`.
2. Client calls `BurgerFactory::createBurger(type)`.
3. Factory chooses and returns matching concrete `Burger`.
4. Client calls `prepare()` polymorphically.

## Pros
- Centralized creation logic.
- Client is decoupled from concrete classes.
- Easy to add creation-time validation.

## Cons
- Factory grows as product variants increase.
- Violates Open/Closed principle if every new variant requires editing the same factory.

## Mermaid
See: `SimpleFactory.mmd`
