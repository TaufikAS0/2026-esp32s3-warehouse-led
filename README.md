# ESP32-S3 Warehouse LED Controller

Firmware **v0.6.1** untuk satu ESP32-S3, empat output WS2812B, maksimum 100 box / 300 LED. JARVIS mengirim ID box melalui HTTP; controller menyimpan mapping dan menjalankan timer secara lokal.

Default aman: satu box `R01-B01`, tiga LED, brightness maksimum 128, seluruh LED padam setelah reboot. Tambahkan box melalui generator/editor web sampai empat rak × 25 box. Jumlah LED diisi manual, tidak dideteksi otomatis.

## Mulai

1. Cocokkan board dan wiring dengan [pin map](01_Desain/Pin_Map.md). Jangan memasok LED dari regulator 3.3 V ESP32.
2. Install core `esp32:esp32@3.3.10`, lalu jalankan `./scripts/build.ps1 -InstallDependencies`.
3. Upload via USB setelah port, board, ukuran flash, dan pin dikonfirmasi. Target build generic ESP32-S3, flash 4 MB, dua slot OTA, tanpa PSRAM, USB CDC aktif. Panduan lengkap: [Build dan setup](04_Dokumen/Build_Setup.md).
4. Buka serial 115200, kirim `provision` dengan newline. Simpan password AP dan password OTA di penyimpanan pribadi. Default password AP: 12345678; password OTA tetap unik dan privat.
5. Hubungkan ke AP `Warehouse-<suffix>`, buka `http://192.168.4.1/`, langsung simpan Wi-Fi gudang serta mapping.
6. Untuk JARVIS, gunakan [API](04_Dokumen/API.md) dan [client Python](scripts/warehouse_client.py). Database barang tidak berada di controller ini.

Dashboard mengikuti referensi visual LumenGrid: empat panel rak,25 posisi per rak, status aktif/timer/warna, pencarian/filter dan panel kontrol box. Semua slot dapat diklik untuk editor per box langsung pada peta; konfigurasi global tetap tersedia. Web/API terbuka di LAN tanpa token sesuai permintaan pengguna. Tes hardware per rak tersedia untuk seluruh LED terpasang, termasuk yang belum bermapping; RGB/kedip berlangsung3 detik; preset setup satu box/tiga LED dan tiga box/satu LED membantu bring-up. Panduan langkah demi langkah: [Web Operator Guide](04_Dokumen/Web_Operator_Guide.md). Tidak memakai CDN atau aset eksternal.

## Struktur dan Git

Klik box membuka popup mapping, warna konfigurasi tersimpan dan kontrol manual. Jumlah LED rak diatur terpisah melalui Simpan setting rak. Panel program otomatis menyediakan running antar-box, kedip bergilir, pelangi dan napas warna; dapat dites dengan timer atau dijalankan sebagai idle. Perintah box aktif mendapat prioritas. Reboot tetap padam; animasi tidak menulis flash.

- `01_Desain`: wiring dan batas elektrik.
- `02_Firmware/WarehouseLed`: source aktif, sketch tipis, driver LED, domain box, codec, NVS, Wi-Fi, web, OTA.
- `04_Dokumen`: API, arsitektur, build, acceptance, dan persiapan publikasi.
- `scripts`, `tests`: client, build, dan pengujian otomatis.
- `_sandbox`: library, cache build, tool host, binary lokal; diabaikan Git.

Versi source tunggal: `firmware_version.h`. Dependency dipin: core 3.3.10, ArduinoJson 7.4.2, Adafruit NeoPixel 1.15.2. Partition table sketch menyediakan dua slot OTA dan NVS128 KiB. Tidak memakai sensor/backend solar atau filesystem web terpisah.

Repository public `TaufikAS0/2026-esp32s3-warehouse-led` atas permintaan pengguna untuk melanjutkan lintas laptop/AI. **Clone branch `feature/warehouse-led-controller` untuk firmware terbaru**; default `develop` masih baseline integration. Branch fitur berasal dari `develop`. `main` dicadangkan untuk rilis produksi pertama; tidak ada push langsung ke main. Mulai dari [START_HERE.md](START_HERE.md), [handoff AI/laptop baru](04_Dokumen/AI_Handoff.md), [riwayat percakapan yang disamarkan](04_Dokumen/THREAD_CONVERSATION.md), dan [Publishing](04_Dokumen/Publishing.md). Credential perangkat dan ekspor tanpa penyamaran tetap dalam paket lokal privat.

## Verifikasi

Jalankan `./scripts/test.ps1` dengan compiler C++ host, lalu `./scripts/build.ps1`. Lihat [laporan dan checklist penerimaan](04_Dokumen/Test_Report.md) untuk hasil aktual dan pekerjaan hardware yang belum dilakukan.

v0.5.1 memperbaiki inisialisasi NeoPixel ESP32 yang sebelumnya menyebabkan LED tidak menyala meskipun API sukses. Authenticated OTA, reboot/reconnect, mapping tetap tersimpan, OTA tertutup dan HTML sesuai source telah diuji. Pengguna mengonfirmasi tiga LED GPIO4 menyala hijau dari kontrol Chrome. Pengujian300 LED, listrik dan endurance8 jam masih terbuka. Detail jaringan dan kredensial perangkat tetap pada penyimpanan lokal privat.

Keberhasilan API berarti firmware menerima perintah; WS2812B tidak memberikan konfirmasi optik atau deteksi putus. Batas brightness software tidak menggantikan PSU, fuse, level shifter, dan validasi kabel.
