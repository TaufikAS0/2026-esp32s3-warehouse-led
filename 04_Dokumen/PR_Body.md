Adds an ESP32-S3 warehouse location indicator controlled by stable box IDs. One controller drives four WS2812B outputs, supporting 25 three-LED boxes per rack; initial configuration enables one box for staged bring-up.

Includes local mapping/control web UI, authenticated HTTP API, bounded per-box timers, NVS configuration, STA/AP recovery, opt-in OTA, and a Python JARVIS client without write retries. Version is v0.1.0.

Validation results and remaining physical acceptance are recorded in 04_Dokumen/Test_Report.md. No changes to the active JARVIS application. Hardware LED/RMT behavior, power distribution, network recovery, OTA upload, and endurance require physical validation before production release.
