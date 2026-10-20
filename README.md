# ESP32-S3 Warehouse LED Controller

Firmware **v0.7.1** untuk satu ESP32-S3, empat output WS2812B, maksimum 100 box / 300 LED. JARVIS mengirim ID box melalui HTTP; controller menyimpan mapping dan menjalankan timer secara lokal.

Default aman: satu box `R01-B01`, tiga LED, brightness maksimum 128, seluruh LED padam setelah reboot. Tambahkan box melalui generator/editor web sampai empat rak × 25 box. Jumlah LED diisi manual, tidak dideteksi otomatis.

## Baru pada v0.7.0

Generator membuat A-01–A-25, B-01–B-25, C dan D, label "-", warna #4cd7f6.
Simpan hingga lima profil pada ESP; preview, muat draft, terapkan, ekspor/impor JSON.
Profil tidak membawa Wi-Fi atau secret. Running/kedip mengikuti urutan LED fisik,
pelangi mengikuti posisi pixel, napas mempertahankan warna config. Rak dites bergantian.
Config memakai revision untuk mencegah penyimpanan dari data klien yang usang.

Build wajib memakai scripts/build.ps1, sumber lab privat dan metadata UTC generated.
Lihat [release notes v0.7.0](04_Dokumen/Release_v0.7.0.md) dan [test report](04_Dokumen/Test_Report.md).
v0.7.1 memperbaiki batas string NVS saat menyimpan100box melalui blob dengan pembacaan
config lama tetap didukung. Lihat [release notes v0.7.1](04_Dokumen/Release_v0.7.1.md)
dan Test_Report untuk status upload/bukti perangkat aktual; tes optik tetap terpisah.

## Mulai

1. Cocokkan board dan wiring dengan [pin map](01_Desain/Pin_Map.md). Jangan memasok LED dari regulator 3.3 V ESP32.
2. Install core `esp32:esp32@3.3.10`, lalu jalankan `./scripts/build.ps1 -InstallDependencies`.
3. Upload via USB setelah port, board, ukuran flash, dan pin dikonfirmasi. Target build generic ESP32-S3, flash 4 MB, dua slot OTA, tanpa PSRAM, UART/CH340 (CDCOnBoot disabled). Panduan lengkap: [Build dan setup](04_Dokumen/Build_Setup.md).
4. Buka serial 115200, kirim `provision` dengan newline. Simpan password AP dan password OTA di penyimpanan pribadi. Default password AP: 12345678; password OTA tetap unik dan privat.
5. Hubungkan ke AP `Warehouse-<suffix>`, buka `http://192.168.4.1/`, langsung simpan Wi-Fi gudang serta mapping.
6. Untuk JARVIS, gunakan [API](04_Dokumen/API.md) dan [client Python](scripts/warehouse_client.py). Database barang tidak berada di controller ini.

Dashboard mengikuti referensi visual LumenGrid: empat panel rak,25 posisi per rak, status aktif/timer/warna, pencarian/filter dan panel kontrol box. Semua slot dapat diklik untuk editor per box langsung pada peta; konfigurasi global tetap tersedia. Web/API terbuka di LAN tanpa token sesuai permintaan pengguna. Tes hardware per rak tersedia untuk seluruh LED terpasang, termasuk yang belum bermapping; RGB/kedip berlangsung3 detik; preset setup satu box/tiga LED dan tiga box/satu LED membantu bring-up. Panduan langkah demi langkah: [Web Operator Guide](04_Dokumen/Web_Operator_Guide.md). Tidak memakai CDN atau aset eksternal.

## Struktur dan Git

Klik box membuka popup mapping, warna konfigurasi tersimpan dan kontrol manual. Jumlah LED rak diatur terpisah melalui Simpan setting rak. Panel program otomatis menyediakan running antar-box, kedip bergilir, pelangi dan napas warna; dapat dites dengan timer atau dijalankan sebagai idle. Tes/idle eksklusif satu rak bergantian, maksimum 75 LED. Perintah manual menghentikan tes sebelum diterapkan. Reboot tetap padam; animasi tidak menulis flash.

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
