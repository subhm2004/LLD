# Notification LLD

This module provides a header-first C++ low-level design implementation of a notification engine.

## Folder Structure
- `core`: system orchestration (`NotificationSystem`)
- `enums`: common enums (`ChannelType`, `NotificationPriority`)
- `models`: domain models (`Recipient`, `NotificationMessage`)
- `services`: engine and service layer
- `rules`: configurable constants
- `main.cpp`: runnable sample
- `notification_lld.h`: umbrella include

## Build

```bash
g++ -std=c++17 main.cpp -o notification_app
./notification_app
```
