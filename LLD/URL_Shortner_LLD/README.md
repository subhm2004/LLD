# URL Shortner LLD (C++)

This folder contains a Low-Level Design implementation of a URL shortener service.

## Components

- `UrlEntry`: mapping entity for short code and long URL
- `Base62Encoder`: generates compact base62 short codes
- `UrlShortnerService`:
  - shorten long URLs
  - resolve short URLs
  - track click count and metadata

## Features

- Long URL to short URL conversion
- Deterministic deduping for repeated long URLs
- Short URL resolution
- Click count tracking
- Basic input validation

## Structure

- `main.cpp`
- `services/UrlShortnerService.h`
- `models/UrlEntry.h`
- `utils/Base62Encoder.h`

## Run

```bash
g++ -std=c++17 main.cpp -o url_shortner_app
./url_shortner_app
```
