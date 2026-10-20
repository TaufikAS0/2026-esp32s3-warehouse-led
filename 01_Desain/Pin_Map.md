# Pin map dan instalasi awal

Target software: generic ESP32-S3. GPIO berikut adalah default firmware, **belum disahkan untuk board fisik pengguna**.

| Output / rak | GPIO data | Kapasitas | Level shifter |
|---|---:|---:|---|
| 1 | 4 | 75 LED | 74AHCT125 kanal 1 |
| 2 | 5 | 75 LED | 74AHCT125 kanal 2 |
| 3 | 6 | 75 LED | 74AHCT125 kanal 3 |
| 4 | 7 | 75 LED | 74AHCT125 kanal 4 |

Semua pin berada di `02_Firmware/WarehouseLed/config.h`. Pastikan board tidak memakai pin tersebut untuk peripheral onboard, flash, atau fungsi lain. USB native ESP32-S3 umumnya menggunakan GPIO19/20; cek schematic board sebenarnya.

## Wiring per rak

ESP32 GPIO → input 74AHCT125 (VCC 5 V) → resistor seri 300–500 ohm dekat DIN LED pertama → DIN rantai LED rak. Ground ESP32, level shifter, PSU, dan LED bersama. Aktifkan OE masing-masing kanal dengan level LOW; input yang tidak dipakai jangan dibiarkan mengambang.

PSU LED 5 V terpisah, kapasitor bulk 500–1000 uF minimal rating 6.3 V di inlet LED, dan bypass 100 nF dekat level shifter. Jangan memasukkan sinyal 5 V ke GPIO ESP32. Perhatikan arah DIN/DOUT dan polaritas. Jangan paralelkan DOUT empat rak menjadi satu.

Referensi: [Adafruit Basic Connections](https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections), [Best Practices](https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices), [Powering NeoPixels](https://learn.adafruit.com/adafruit-neopixel-uberguide/powering-neopixels).

## Daya dan pengujian

Identifikasi varian/lot LED dan gunakan datasheet serta hasil pengukuran untuk menentukan arus. Sebagai perkiraan konservatif LED RGB lama 60 mA/pixel pada putih penuh: 75 LED sekitar 4.5 A/rak, 300 LED sekitar 18 A total, belum termasuk controller dan margin. Ini **bukan ukuran PSU final**; varian LED berbeda dapat memiliki arus berbeda.

Tentukan gauge kabel, fuse tiap cabang, konektor, dan titik injeksi daya berdasarkan arus, panjang, drop tegangan, dan suhu. Jangan menyalurkan arus seluruh rak lewat jalur USB/board ESP32. Pastikan tidak terjadi backfeed antara USB dan input 5 V board saat pengujian.

Firmware mengirim buffer 75 posisi pada setiap output yang perlu diperbarui; hanya LED terpasang yang dapat menyala. Posisi yang tidak dipetakan selalu hitam. Tambahkan LED dengan power dimatikan, lalu ubah jumlah dan mapping melalui web.

Acceptance: uji 3 → 75 → 300 LED. Ukur tegangan pada ujung jalur, kestabilan data, suhu kabel/konektor, serta Wi-Fi ketika semua output aktif. Jika gagal, cari penyebab daya/sinyal; mengurangi rak tidak otomatis memperbaiki sambungan yang buruk.
