# Meeting Scheduler — Low-Level Design

## Problem

Design a simplified **meeting scheduler** like Calendly / Google Calendar booking: users set availability, book meetings with conflict checks, and find mutual free slots.

## Functional scope (this LLD)

1. User registration (name, email, timezone label).
2. Set **availability windows** per user per calendar day.
3. **Schedule meeting** — organizer + attendees; reject on time overlap or outside availability.
4. **Cancel meeting** — organizer or attendee.
5. **List meetings** for a user on a date (or all upcoming).
6. **Find mutual free slots** — given user IDs, date, and duration, return bookable intervals (Strategy).

## Out of scope (HLD / extensions)

- Recurring rules (RRULE), Google/Outlook sync, video links.
- Distributed calendar shards, reminder cron, email ICS attachments.
- Full timezone conversion — demo uses single timezone label + local minutes.

## Assumptions

- Time stored as **minutes from midnight** (0–1439) on a `YYYY-MM-DD` date string.
- Meetings cannot span midnight in v1.
- In-memory storage; no thread-safety unless extended.
