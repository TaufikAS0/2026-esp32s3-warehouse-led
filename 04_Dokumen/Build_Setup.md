# Build dan verifikasi v0.7.0

Pinned: Arduino-ESP32 3.3.10, ArduinoJson 7.4.2, Adafruit NeoPixel 1.15.2.
Build default generic ESP32-S3,4MB,no-PSRAM,hwcdc,CDCOnBoot=default (UART/CH340).
Native USB CDC hanya dengan switch -NativeUsbCdc. Tidak menebak COM/board untuk upload.

## Sumber lab privat

Buat _sandbox/lab-secrets.json secara lokal, atau set WAREHOUSE_LAB_SECRETS_FILE
ke file privat dengan bentuk {"ssid":"HuaweiJIN","password":"<isi secara lokal>"}.
Jangan commit/menyalin nilai sebenarnya ke chat. generated_lab.h dan generated_build.h
diabaikan Git; binary/ELF juga memuat profil lab sehingga tetap privat.
Build menolak file privat hilang/invalid. Host tests memakai profil sintetis.

## Build yang didukung

```powershell
arduino-cli core install esp32:esp32@3.3.10
./scripts/build.ps1 -InstallDependencies
./scripts/build.ps1
# Hanya untuk native USB CDC:
./scripts/build.ps1 -NativeUsbCdc
```

CLI di PATH/instalasi IDE, atau ARDUINO_CLI/-ArduinoCli. Library diisolasi pada
_sandbox/sketchbook; global library proyek lain tidak diubah. Compile jobs2 untuk membatasi beban host.
Gunakan wrapper build.ps1; direct compile Arduino IDE tidak menjalankan gate/manifest.

Setiap invocation menghasilkan UTC ISO8601 dan nonce baru. Semua translation unit
memakai header generated yang sama. Tidak memakai waktu upload/reboot atau timestamp manual.
Gate menolak marker versi/UTC/nonce hilang/usang, binary melebihi1280KiB, partisi tidak sesuai.
_sandbox/build/manifest.json berisi versi, UTC, identity, SHA256, bytes, FQBN.
Manifest lama dihapus sebelum build; hanya sukses gate menghasilkan manifest baru.
Partisi tetap dua slot1280KiB dan NVS128KiB@0x290000, tanpa perubahan layout.

## Host tests

Python3.11+, Node18+, C++17/g++ atau WAREHOUSE_CXX/-Cxx.
Alternatif compiler terisolasi:

```powershell
python -m pip install --target _sandbox/python-tools ziglang==0.14.1
npm install --prefix _sandbox/ui-tests jsdom@26.1.0 --no-audit --no-fund
./scripts/test.ps1
```

Tes memakai domain, codec/ArduinoJson asli, NeoPixel asli dengan shim batas GPIO/RMT,
profile codec/repository dengan backend fault injection, SettingsStore asli dengan
Preferences/serial/RNG sintetis, Python client HTTP loopback, metadata dan DOM embedded UI.
Fixture browser opsional: python tests/ui_fixture.py pada127.0.0.1:8769, berlabel SIMULASI.
Tidak membuktikan timing RMT, flash/NVS nyata, listrik, optical output, OTA atau endurance.

## Upgrade perangkat — hanya setelah diminta

Jangan upload otomatis. Cocokkan board, ukuran flash, pin, profil UART/native USB,
dan kondisi aktual mapping/jaringan sebelum menyentuh perangkat.
OTA tetap aplikasi-only, password unik, aktivasi manual10menit saat STA.
Jangan erase/NVS/partisi saat upgrade biasa. Backup config/mapping sebelum upgrade.
Boot memigrasikan hanya jaringan yang berbeda ke profil lab dan default AP;
mapping/warna/token/OTA dipertahankan, config rusak tidak ditimpa otomatis.
Setelah upload yang diizinkan, periksa versi+UTC, jaringan, mapping, boot padam,
OTA tertutup, lalu uji3LED dan satu rak maksimal75LED secara bergantian.
