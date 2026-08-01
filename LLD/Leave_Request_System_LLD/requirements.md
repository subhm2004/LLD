# Requirements: Leave Request System

## Functional Requirements

1. System should **register** employees with id, name, and team.
2. System should let an employee **submit** a leave request (employee, leave type, start/end date, working days).
3. System should route a pending request through an **approval chain** by duration:
   - **Team Lead** — up to **1** day
   - **Manager** — up to **3** days
   - **HR** — up to **7** days
   - **Director** — any duration (final authority)
4. The **first handler** with sufficient authority should approve and **stop** the chain.
5. If no handler can approve, the request should be marked **REJECTED** with a reason.
6. System should let a user **query** a request by id and **list** requests by employee.
7. System should let an employee **cancel** a request while it is still `PENDING`.

## Non-Functional Requirements

1. In-memory only (interview scope).
2. C++17, header-based modules, built via `./compile.sh`.
3. Use `#ifndef / #define / #endif` guards.
4. Console demo in `main.cpp` covering short, medium, long, and rejected flows.

## Design Patterns

| Pattern | Where |
| ------- | ----- |
| **Chain of Responsibility** | `handlers/` — `TeamLead → Manager → HR → Director` |
| **Facade / Service layer** | `core/LeaveRequestSystem.h`, `services/` |

## Assumptions

1. Working days are provided at submit time (no weekend/holiday calculator).
2. One approval chain per organization (not per team).
3. Approved requests are immutable; cancel only while `PENDING`.

## Out of Scope

- Payroll integration, calendar sync, email notifications.
- Persistent storage / multi-tenant HRIS.
- Partial-day / hourly leave (whole days only).
- Dynamic org-chart lookup (handlers are configured in code).
