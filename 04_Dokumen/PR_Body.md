Adds an ESP32-S3 warehouse location indicator controlled by stable box IDs. One controller drives four WS2812B outputs, supporting 25 three-LED boxes per rack; initial configuration enables one box for staged bring-up.

Includes an embedded slate/cyan rack dashboard with100 visible positions, selected-box control,3-second RGB/blink tests, per-LED setup presets, validated mapping drafts, authenticated HTTP API, bounded timers, NVS configuration, STA/AP recovery, opt-in OTA, and a Python client without write retries. Version is v0.3.2. User-approved internal AP default is12345678; actual router/API/OTA credentials stay private.

Validation: ESP32-S3 compile, C++ domain/codec,11 Python tests and real HTML/JS DOM suite pass. USB boot, stored router reconnect, actual HTTP status/config/RGB/expiry/error responses and browser manual control were verified on the device. Authenticated OTA upload succeeded, with wrong-password rejection, reboot, retained mapping/Wi-Fi and closed OTA after boot. Initial transfer failures and final mitigation are documented in04_Dokumen/Test_Report.md.

No active JARVIS source changes. Optical LED output,75/300 LED/RMT behavior, power distribution, full network recovery and8-hour endurance remain open; this PR is a software baseline, not a production certification.
