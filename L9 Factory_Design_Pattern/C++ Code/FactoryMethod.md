# Factory Method Pattern (`FactoryMethod.cpp`)

## Intent
Define a product creation interface in a base factory and let subclasses decide which concrete product family to create.

## Structure In This File
- `Burger` is the abstract product.
- Product variants:
  - Regular line: `BasicBurger`, `StandardBurger`, `PremiumBurger`
  - Wheat line: `BasicWheatBurger`, `StandardWheatBurger`, `PremiumWheatBurger`
- `BurgerFactory` is the creator abstraction with factory method `createBurger(type)`.
- `SinghBurger` and `KingBurger` are concrete creators implementing different creation policies.
- Client (`main`) works through `BurgerFactory*`.

## Flow
1. Client picks concrete factory (`SinghBurger` or `KingBurger`).
2. Client calls `createBurger(type)` on abstract factory reference.
3. Concrete factory returns corresponding concrete burger variant.
4. Client uses returned `Burger*` via `prepare()`.

## Pros
- Good Open/Closed behavior at factory level.
- Different product families can evolve independently.
- Client code remains polymorphic and clean.

## Cons
- More classes than simple factory.
- New product type often requires updates across creator hierarchy.

## Mermaid
See: `FactoryMethod.mmd`
