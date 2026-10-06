# Laporan verifikasi v0.1.0

Tanggal: **2026-10-06, Asia/Bangkok**. Status: implementasi software; **belum disahkan untuk operasi gudang**.

## Software yang diuji

| Pemeriksaan | Hasil | Batas bukti |
|---|---|---|
| ESP32-S3 compile, Arduino-ESP32 3.3.10 | Lulus | Target generic4MB, PSRAM off, USB CDC; bukan identifikasi board fisik |
| ArduinoJson 7.4.2 / NeoPixel 1.15.2 | Dipin dan berhasil compile | Empat objek output terkompilasi; RMT fisik belum diuji |
| C++ domain, -Wall -Wextra -Werror | Lulus | Mapping100 box, seluruh300 posisi, overlap/duplikat/rentang, brightness, kedip, expiry, replacement, rollover millis |
| Config transaction dengan storage failure injection | Lulus | Invalid candidate tidak menulis; failed write menjaga config/state lama; sukses reset state; bukan fault injection NVS fisik |
| Codec dengan library ArduinoJson asli | Lulus | Roundtrip100 box termasuk panjang field maksimum; tipe/rentang JSON dan bearer benar/salah |
| Python client HTTP loopback | 11 test lulus | Method/path/body, off/status/all-off,401,404, input invalid, timeout, disconnect, redirect; bukan HTTP ESP32 |
| HTML/JS asli dalam jsdom26.1.0 | Lulus | Auth, generator100 box, draft sebelum Simpan, mapping akhirR04-B25 LED73–75, save, controlwarna/brightness/timer, stable polling, rejected save, logout |
| Browser fixture lokal | Layout + login/control/brightness awal teramati | Fixture sintetis; dialog browser lama menghambat uji lanjutan. Generator final tanpa dialog native diverifikasi lewat DOM suite |
| Partition binary hasil build | Lulus | app0@0x10000, app1@0x150000 masing-masing1310720 byte; NVS@0x290000 ukuran131072 byte |

Ukuran build final setelah perubahan socket-start: **1020641 byte flash /1310720 (77%)**, **61260 byte RAM statis /327680 (18%)**, sisa266420 byte sebelum alokasi runtime. Web/mapping/JSON/RMT membutuhkan heap tambahan ketika runtime, sehingga sisa ini bukan hasil heap stress test.

Build pertama menampilkan warning initializer dari core/TinyUSB; source proyek terkompilasi tanpa warning proyek yang ditemukan. Rebuild cached final tidak menampilkan warning. Log lokal di `_sandbox/build-final.log` dan `_sandbox/test-final.log`, tidak masuk Git.

## Pengujian fisik yang belum dilakukan

| Tahap | Penerimaan | Status |
|---|---|---|
| Board/pin | Tipe S3, flash, USB, GPIO4/5/6/7 sesuai schematic | Belum diuji |
| USB/NVS provisioning | Secret unik tersimpan, tidak bocor lewat API/log boot, persist reboot | Belum diuji |
| 3 LED /1 box | ID memilih tepat tiga LED; RGB, steady/blink, timeout, boot padam | Belum diuji |
| 75 LED /1 rak | Semua25 box memilih posisi benar; brightness dan tail clear setelah remap | Belum diuji |
| 300 LED /4 output | Tidak reset/flicker, empat jalur berjalan, API/web responsif | Belum diuji |
| Mapping100 box/NVS | Save ulang full mapping termasuk label panjang, reboot, putus daya saat save | Belum diuji |
| HTTP ESP32 |401/400/404/405/413/415/503, token nyata, status multi-client, bounded timeout | Belum diuji |
| Recovery jaringan | Router hilang, AP berpassword muncul, reconnect router, timer terus berjalan | Belum diuji |
| OTA | Opt-in STA, password benar/salah, window10 menit, upload dan reboot | Belum diuji |
| Endurance | 8 jam dengan empat output + polling/perintah | Belum diuji |
| Listrik | Tegangan ujung, PSU/fuse/injeksi, suhu kabel/konektor pada beban maksimum | Belum diuji |
| JARVIS aktif | Integrasi source aplikasi dan pemetaan barang→box | Di luar paket awal; hanya contoh client disediakan |

## Keputusan penerimaan

Layak sebagai baseline software untuk **bring-up bertahap**, belum sebagai rilis produksi. Jangan menyatakan perangkat siap gudang hanya karena compile/host tests lulus. Jika fault terjadi, pisahkan diagnosis daya, data, jaringan, dan firmware; jangan otomatis menganggap pengurangan rak menyelesaikan masalah.

## Git/publication

Private remote sudah dibuat; kode belum dipush. develop berisi bootstrap minimal; feature/warehouse-led-controller berisi implementasi. main belum dibuat karena belum ada rilis produksi. Publikasi selanjutnya: bootstrap integration, feature push, PR ke develop, tanpa auto-merge. Jangan upload log secret provisioning.
