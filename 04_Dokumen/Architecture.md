# Arsitektur

`WarehouseLed.ino` hanya memanggil `App.begin()` / `App.update()`. App memiliki controller, SettingsStore, LedDriver, WifiService, OtaService, WebService; tidak ada global state aplikasi lain.

`domain.h` bebas Arduino: mapping, validasi rentang/overlap/ID, state per box, timer elapsed uint32 yang tahan rollover, dan render RGB. Domain tidak bergantung pada jaringan; timer tetap berjalan tanpa router.

`codec.*` menerapkan tipe dan batas JSON sebelum nilai dikonversi ke field kecil. ID case-sensitive ASCII `[A-Za-z0-9_-]{1,24}`, label <=48 byte UTF-8. API/web memakai output dan LED mulai 1; domain mulai 0. Konfigurasi schema 1; schema lain ditolak, tidak dimigrasikan diam-diam.

`settings_store.*` menyimpan mapping + Wi-Fi sebagai satu string JSON pada satu key NVS. Hanya PUT konfigurasi yang menulis konfigurasi. Token API dan password OTA acak serta default AP bersama ditulis saat `provision` lewat USB. Perintah USB `ap-password` mengganti hanya field AP pada record credentials; state aktif berubah setelah tulis sukses, lalu jaringan restart. Tidak ada migrasi password otomatis saat upgrade. Invalid config / kegagalan tulis tidak mengganti state aktif. Config rusak saat boot memakai default aman, menampilkan storage_error, dan tidak menimpa record rusak sampai pengguna Simpan. NVS ditangani Preferences; atomicity terhadap mati listrik harus diuji pada board.

`led_driver.*` membungkus empat Adafruit_NeoPixel (GRB, 800 kHz) dengan buffer tetap 75 LED/output. Frame dibandingkan dengan frame sebelumnya; hanya perubahan dikirim. Output tidak bermapping tetap hitam. Transmisi show() membutuhkan waktu terbatas dan terjadi berurutan; loop tidak memakai delay(), tetapi ini bukan jaminan seluruh stack non-blocking. HTTP WebServer dan transfer OTA tetap dapat menunggu I/O. Uji responsiveness nyata diperlukan.

Partition table sketch mempertahankan dua slot OTA1280 KiB dan memindahkan NVS ke ruang128 KiB setelah app slots. Ini menyediakan ruang untuk record lama, record pengganti, kredensial, dan garbage collection pada kapasitas100 box. NVS bawaan yang kecil tidak dipakai. Partition table dipasang via USB saat commissioning pertama, tidak dimigrasikan oleh API.

`wifi_service.*` tidak memulai radio sebelum credential USB tersedia. STA memperoleh DHCP; setelah 15 detik gagal, AP berpassword dibuka. Coba STA lagi setiap 30 detik. AP ditutup ketika STA tersambung. Tanpa SSID, AP terus tersedia. Retry berlangsung tanpa menunda controller. Simpan Wi-Fi menjadwalkan restart jaringan setelah respons HTTP.

Controller memakai catu tetap; modem sleep dimatikan untuk mengurangi jeda HTTP/OTA. Ini pilihan latency dibanding idle power, bukan jaminan bebas gangguan router. ArduinoOTA menggunakan toleransi jeda10 detik dan log start/end/error. Transfer tetap sinkron pada maintenance window, sehingga web dapat berhenti merespons sementara.

`web_service.*` memeriksa bearer untuk semua endpoint API. Halaman `/` adalah shell publik tanpa data sensitif; token diisi manual dan disimpan di sessionStorage tab. Tidak ada CORS publik, CDN, atau password pada GET config. HTTP/token digunakan di LAN tepercaya; ini bukan transport terenkripsi. Client menonaktifkan proxy mesin dan redirect agar bearer tidak ikut ke host lain.

`ota_service.*` mati setelah boot, hanya STA, autentikasi password unik, jendela 10 menit. Putus STA menutup OTA. Mengaktifkan OTA memadamkan LED. Transfer OTA adalah pemeliharaan, bukan operasi gudang normal; reboot tetap memadamkan state box.

Provisioning mengaktifkan sumber entropy non-RF hanya saat pembuatan secret pertama sebelum Wi-Fi/ADC dijalankan, kemudian menonaktifkannya sebelum jaringan dimulai. Sesuai [panduan RNG ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/random.html).

## Batas dan perilaku konflik

- 4 output, 75 LED/output, 100 box, body JSON <=32768 byte setelah diterima WebServer.
- LED bukan perangkat beralamat tetap, tidak ada autodetection atau feedback LED.
- Beberapa pemanggil dapat mengontrol box; perintah terakhir per box berlaku dan memperbarui timer. API tidak menyediakan antrean pekerjaan gudang atau transaksi banyak box.
- Timeout client dapat terjadi setelah perintah diterapkan. Client tidak retry write; baca status lalu putuskan tindakan.
- Semua box padam saat config diterapkan / all-off / reboot / aktivasi OTA. Koneksi putus saja tidak memadamkan box sebelum timer selesai.
- Kapasitas di atas 300 LED membutuhkan perubahan firmware serta review daya dan timing; jangan klaim ekspansi tanpa batas.
