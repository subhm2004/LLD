# Elevator System LLD — Design Diagrams

> Codebase padh ke banaya. Do dilchasp cheezein: **ElevatorCar ek state machine hai**
> (IDLE/UP/DOWN + door OPEN/CLOSED) aur **ElevatorScheduler** SCAN-jaisa scoring karke
> best elevator chunta hai.

---

## 1. Class Diagram

```mermaid
classDiagram
    direction TB

    class ElevatorSystemController {
        <<Facade / Orchestrator>>
        -ElevatorCar[] elevators_
        -ElevatorInsidePanel[] insidePanels_
        -FloorPanel[] floorPanels_
        -ElevatorScheduler scheduler_
        +requestElevator(ExternalRequest)
        +selectFloorInsideElevator(InternalRequest)
        +pressFloorButton(floor, direction)
        +boardPassenger(id, weight) bool
        +runOneTick()
    }

    class ElevatorCar {
        <<state machine>>
        -int currentFloor_
        -Direction direction_
        -Door door_
        -int peopleCount_
        -int currentWeightKg_
        -Display insideDisplay_
        -Display outsideDisplay_
        -set upStops_
        -set downStops_
        +addStop(floor)
        +step()
        +openDoor() bool
        +boardPassenger(weight) bool
        +exitPassenger(weight) bool
        +isIdle() bool
        +isFull() bool
    }

    class Door {
        <<state machine>>
        -DoorState state_
        +open(elevatorIsIdle) bool
        +close()
        +forceClose() bool
    }

    class Display {
        -DisplayType type_
        -int floor_
        -Direction direction_
        +update(floor, dir, people, weight)
        +show(label)
    }

    class ElevatorScheduler {
        +selectBestElevator(elevators, floor, dir) ElevatorCar*
        -scoreFor(elevator, floor, dir) int
    }

    class ElevatorInsidePanel {
        -ElevatorCar* elevator_
        +pressFloorButton(floor)
        +pressOpenDoorButton()
    }

    class FloorPanel {
        -int floorNumber_
        -RequestHandler handler_
        -Display display_
        +pressUp()
        +pressDown()
    }

    class ExternalRequest {
        <<struct>>
        +int floor
        +Direction direction
    }
    class InternalRequest {
        <<struct>>
        +int elevatorId
        +int destinationFloor
    }

    ElevatorSystemController *-- ElevatorCar : owns
    ElevatorSystemController *-- ElevatorInsidePanel
    ElevatorSystemController *-- FloorPanel
    ElevatorSystemController *-- ElevatorScheduler
    ElevatorScheduler ..> ElevatorCar : scores & picks
    ElevatorCar *-- Door : has
    ElevatorCar *-- Display : inside + outside
    ElevatorInsidePanel o-- ElevatorCar : controls (ptr)
    FloorPanel *-- Display
    ElevatorSystemController ..> ExternalRequest
    ElevatorSystemController ..> InternalRequest
    FloorPanel ..> ElevatorSystemController : callback handler
```

---

## 2. ⭐ ElevatorCar — Direction state machine (LOOK algorithm)

Elevator do sorted sets rakhta hai — `upStops_` (chhota→bada) aur `downStops_`
(bada→chhota). Har `step()` pe ek floor move karta hai aur direction manage karta hai:

```mermaid
stateDiagram-v2
    [*] --> IDLE
    IDLE --> UP : upStops hai (ya nearer)
    IDLE --> DOWN : downStops hai (ya nearer)
    UP --> UP : aur upStops bache hain
    UP --> DOWN : upStops khatam, downStops bache
    UP --> IDLE : dono khaali
    DOWN --> DOWN : aur downStops bache
    DOWN --> UP : downStops khatam, upStops bache
    DOWN --> IDLE : dono khaali
```

> **LOOK algorithm:** elevator ek direction me tab tak jaata hai jab tak us taraf stops
> hain, phir doosri taraf mudta hai — bina building ke end tak gaye (wahi "LOOK" vs "SCAN").
> `pickInitialDirection()` IDLE se shuru hote waqt nearest stop wali taraf chunta hai.

---

## 3. ⭐ Door state machine (safety interlock)

```mermaid
stateDiagram-v2
    [*] --> CLOSED
    CLOSED --> OPEN : open() — sirf jab elevator IDLE ho
    OPEN --> CLOSED : close() / forceClose()
    CLOSED --> CLOSED : chalte waqt open() BLOCK
```

> **Safety rule:** `door_.open(elevatorIsIdle)` — darwaza sirf tab khulta hai jab elevator
> **IDLE** ho (chal raha ho to `false`). Aur `step()` chalne se pehle `forceClose()` karta
> hai. Ye real elevator ka interlock hai — chalte elevator me darwaza nahi khul sakta.

---

## 4. ⭐ Scheduler scoring — best elevator kaise chunte hain

`ElevatorScheduler::scoreFor` har elevator ko ek score deta hai (kam = behtar):

```mermaid
flowchart TD
    A[scoreFor elevator, reqFloor, reqDir] --> B{elevator FULL?}
    B -- haan --> Skip[skip — consider hi nahi]
    B -- nahi --> C{elevator IDLE?}
    C -- haan --> D[score = distance<br/>-- sabse accha]
    C -- nahi --> E{same direction<br/>aur raaste me aa raha?}
    E -- haan --> F[score = distance + 1<br/>-- on-the-way, accha]
    E -- nahi --> G[score = distance + 10<br/>-- ulta jaana padega, mehnga]
```

| Elevator ki haalat | Score | Matlab |
|---|---|---|
| IDLE | `distance` | free hai, seedha aa jayega — best |
| Same dir, raaste me | `distance + 1` | waise bhi udhar jaa raha, pick up kar lega |
| Warna (ulta/opposite) | `distance + 10` | pehle apna kaam, phir mudega — penalty |

Sabse **kam score wala** elevator jeetta hai. Ye SCAN/LOOK disk-scheduling ki soch hai.

---

## 5. Sequence — external request se elevator movement tak

```mermaid
sequenceDiagram
    autonumber
    actor Person
    participant FP as FloorPanel
    participant Ctrl as ElevatorSystemController
    participant Sched as ElevatorScheduler
    participant Car as ElevatorCar

    Person->>FP: pressUp()
    FP->>Ctrl: handler(floor, UP) [callback]
    Ctrl->>Sched: selectBestElevator(elevators, floor, UP)
    loop har elevator
        Sched->>Sched: scoreFor(elevator, floor, UP)
    end
    Sched-->>Ctrl: best ElevatorCar*
    Ctrl->>Car: addStop(floor)  (upStops_ me daal do)

    Note over Person,Car: baad me har tick pe...
    Person->>Ctrl: runOneTick()
    Ctrl->>Car: step()
    Car->>Car: forceClose door, ek floor move, stop process
    Car-->>Ctrl: naya floor/direction
```

---

## 6. Design patterns summary

| Pattern | Kahan | Kyun |
|---|---|---|
| **State machine** | `ElevatorCar` (IDLE/UP/DOWN), `Door` (OPEN/CLOSED) | real elevator behavior + safety interlock |
| **Strategy (implicit)** | `ElevatorScheduler` | dispatch algorithm alag — FCFS/SCAN/ML se replace ho sakta |
| **Facade / Mediator** | `ElevatorSystemController` | panels, cars, scheduler ko jodta hai |
| **Callback / Observer-lite** | `FloorPanel::RequestHandler` (std::function) | panel controller ko loosely coupled call karta hai |
| **Command-lite** | `ExternalRequest` / `InternalRequest` structs | request ko object bana ke pass karna |

> ⚠ **Config:** `SystemLimits.h` me `MAX_FLOORS=15`, `MAX_ELEVATORS=3`, `MAX_PEOPLE=8`,
> `MAX_WEIGHT_KG=680` — capacity checks (`isFull`, `boardPassenger`) inhi pe based hain.
