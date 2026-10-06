# ESP32-S3 Warehouse LED Controller

Firmware **v0.1.0** untuk satu ESP32-S3, empat output WS2812B, maksimum 100 box / 300 LED. JARVIS mengirim ID box melalui HTTP; controller menyimpan mapping dan menjalankan timer secara lokal.

Default aman: satu box `R01-B01`, tiga LED, brightness maksimum 128, seluruh LED padam setelah reboot. Tambahkan box melalui generator/editor web sampai empat rak × 25 box. Jumlah LED diisi manual, tidak dideteksi otomatis.

## Mulai

1. Cocokkan board dan wiring dengan [pin map](01_Desain/Pin_Map.md). Jangan memasok LED dari regulator 3.3 V ESP32.
2. Install core `esp32:esp32@3.3.10`, lalu jalankan `./scripts/build.ps1 -InstallDependencies`.
3. Upload via USB setelah port, board, ukuran flash, dan pin dikonfirmasi. Target build generic ESP32-S3, flash 4 MB, dua slot OTA, tanpa PSRAM, USB CDC aktif. Panduan lengkap: [Build dan setup](04_Dokumen/Build_Setup.md).
4. Buka serial 115200, kirim `provision` dengan newline. Simpan token API, password AP, dan password OTA di penyimpanan pribadi. Tidak ada kredensial bawaan repository.
5. Hubungkan ke AP `Warehouse-<suffix>`, buka `http://192.168.4.1/`, masukkan token, dan simpan Wi-Fi gudang serta mapping.
6. Untuk JARVIS, gunakan [API](04_Dokumen/API.md) dan [client Python](scripts/warehouse_client.py). Database barang tidak berada di controller ini.

## Struktur dan Git

- `01_Desain`: wiring dan batas elektrik.
- `02_Firmware/WarehouseLed`: source aktif, sketch tipis, driver LED, domain box, codec, NVS, Wi-Fi, web, OTA.
- `04_Dokumen`: API, arsitektur, build, acceptance, dan persiapan publikasi.
- `scripts`, `tests`: client, build, dan pengujian otomatis.
- `_sandbox`: library, cache build, tool host, binary lokal; diabaikan Git.

Versi source tunggal: `firmware_version.h`. Dependency dipin: core 3.3.10, ArduinoJson 7.4.2, Adafruit NeoPixel 1.15.2. Partition table sketch menyediakan dua slot OTA dan NVS128 KiB. Tidak memakai sensor/backend solar atau filesystem web terpisah.

Repository target private `TaufikAS0/2026-esp32s3-warehouse-led`. Branch fitur `feature/warehouse-led-controller` berasal dari `develop`. `main` dicadangkan untuk rilis produksi pertama; tidak ada push langsung ke main. Lihat [Publishing](04_Dokumen/Publishing.md).

## Verifikasi

Jalankan `./scripts/test.ps1` dengan compiler C++ host, lalu `./scripts/build.ps1`. Lihat [laporan dan checklist penerimaan](04_Dokumen/Test_Report.md) untuk hasil aktual dan pekerjaan hardware yang belum dilakukan.

Keberhasilan API berarti firmware menerima perintah; WS2812B tidak memberikan konfirmasi optik atau deteksi putus. Batas brightness software tidak menggantikan PSU, fuse, level shifter, dan validasi kabel.
