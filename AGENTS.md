# Project rules

- Read README and 04_Dokumen before editing. Follow the workspace JIN Firmware Standards.
- Arduino-ESP32 modular firmware; thin sketch, no runtime delay, pins centralized.
- Never commit Wi-Fi passwords, API tokens, OTA credentials, or machine-specific paths.
- Exception approved by user: the shared internal AP setup default `12345678` may be documented and stored in config.h. Real STA passwords, API tokens and OTA credentials remain private.
- Version source: 02_Firmware/WarehouseLed/firmware_version.h.
- Work on feature/* from develop; PR to develop. No direct commits/pushes to main.
- main is reserved for the first production release. Do not create a production label before hardware acceptance.
- Run domain tests, Python tests and ESP32-S3 compile before a feature is accepted.
- Report software verification separately from physical LED/electrical verification.
- Do not automatically flash hardware or change existing projects.
