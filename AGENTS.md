# Project rules

- Read README and 04_Dokumen before editing. Follow the workspace JIN Firmware Standards.
- Laptop/AI continuation starts at 04_Dokumen/AI_Handoff.md on feature/warehouse-led-controller; develop is not the latest firmware implementation.
- Before future firmware changes/build/upload or publication, read the vault Rules_Versioning.md and Rules_Change_Notifications.md in full, as well as the eight-file order in AI_Handoff.md. The v0.6.1 legacy build predates generated UTC build metadata/gates/manifests; implement the new requirements before the next firmware build, do not fabricate timestamps.
- The public repository contains source and sanitized handoff only. User conversation exports and device access bundles stay outside Git.
- Arduino-ESP32 modular firmware; thin sketch, no runtime delay, pins centralized.
- Never commit Wi-Fi passwords, API tokens, OTA credentials, or machine-specific paths.
- Exception approved by user: the shared internal AP setup default `12345678` may be documented and stored in config.h. Real STA passwords, API tokens and OTA credentials remain private.
- Version source: 02_Firmware/WarehouseLed/firmware_version.h.
- Work on feature/* from develop; PR to develop. No direct commits/pushes to main.
- main is reserved for the first production release. Do not create a production label before hardware acceptance.
- Run domain tests, Python tests and ESP32-S3 compile before a feature is accepted.
- Report software verification separately from physical LED/electrical verification.
- Do not automatically flash hardware or change existing projects.

- User-approved v0.5.0 behavior: LAN web and HTTP API are open, without bearer/login. Do not reintroduce API authentication unless requested. OTA upload password remains required. Firmware version must be visible before API connection.
