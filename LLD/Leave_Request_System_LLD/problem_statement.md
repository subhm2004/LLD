# Problem Statement: Leave Request System

Design a low-level **employee leave request** system where approval flows through a **Chain of Responsibility** based on leave duration.

## Functional requirements

1. **Register** employees (id, name, team).
2. **Submit** a leave request: employee, leave type, start/end date, computed working days.
3. **Process approval** on a pending request through an **approval chain**:
   - Team Lead — up to **1** day
   - Manager — up to **3** days
   - HR — up to **7** days
   - Director — any duration (final authority)
4. The **first handler with sufficient authority** approves and stops the chain.
5. If no handler can approve, mark the request **REJECTED** with a reason.
6. **Query** leave requests by id or list by employee.
7. **Cancel** a pending request before `processApproval` runs.

## Non-functional

- In-memory only (interview scope).
- C++17, header-based modules, `compile.sh`.
- Console demo in `main.cpp` covering short, medium, long, and rejected flows.

## Out of scope

- Payroll integration, calendar sync, email notifications
- Persistent storage / multi-tenant HRIS
- Partial-day / hourly leave (whole days only)
- Dynamic org-chart lookup (handlers are configured in code)

## Assumptions

- Working days are provided at submit time (no weekend/holiday calculator).
- One approval chain per organization (not per team).
- Approved requests are immutable; cancel only while `PENDING`.
