# Handoff lintas laptop / AI — 2026-10-20

## Mulai dari sini

Repository: https://github.com/TaufikAS0/2026-esp32s3-warehouse-led

**Jangan memakai checkout default develop sebagai source fitur terbaru.**

```sh
git clone --branch feature/warehouse-led-controller https://github.com/TaufikAS0/2026-esp32s3-warehouse-led.git
cd 2026-esp32s3-warehouse-led
git status -sb
git log -3 --oneline
```

Baseline firmware sebelum handoff: commit `3eccf6798ad53ffc8bbf1d9abeb8f75294ab193e`, **v0.6.1**. Commit handoff sesudahnya hanya mengubah dokumentasi. PR #1 menuju develop: https://github.com/TaufikAS0/2026-esp32s3-warehouse-led/pull/1 . Jangan auto-merge atau push langsung ke main/develop.

Baca AGENTS.md, README.md, dokumen ini, Architecture.md, API.md, Build_Setup.md, Web_Operator_Guide.md, Test_Report.md dan Pin_Map.md sebelum perubahan. Paket privat pengguna berisi percakapan dan aturan vault yang tidak diterbitkan di repository ini.

## Tujuan dan keputusan pengguna

- Controller indikator lokasi gudang, bukan database stok/barang. Tidak mengubah source JARVIS aktif.
- Satu ESP32-S3, empat output WS2812B, satu output per rak; GPIO 4, 5, 6, 7. Maksimum 75 LED/output, 100 box/300 LED. Cocokkan board/wiring sebelum upload.
- ID stabil `R01-B01` sampai `R04-B25`; default tiga LED per box. Nomor web mulai 1, indeks internal mulai 0. Jumlah LED terpasang diatur manual, tidak dideteksi otomatis.
- Mulai satu box/tiga LED. Mapping menolak ID duplikat, overlap dan rentang di luar LED terpasang. Konfigurasi disimpan hanya melalui Simpan, bukan setiap perintah LED.
- Web dan HTTP API terbuka di LAN tanpa token/login, keputusan eksplisit pengguna. Token provisioning lama tidak lagi dipakai untuk HTTP. Jangan mengembalikan login tanpa permintaan.
- OTA pada v0.6.1 tetap berpassword perangkat, default tertutup, diaktifkan manual selama 10 menit saat STA tersambung. Reboot menutup OTA dan memadamkan seluruh LED/program. Kebijakan robot lab tanpa password OTA bukan kebijakan otomatis controller gudang ini.
- Password AP setup/pemulihan bersama `12345678`. Profil STA lab dan akses perangkat disertakan pada paket privat. Jangan mempublikasikan paket percakapan/credential ke GitHub.

## Fitur yang selesai

Dashboard embedded tanpa CDN/filesystem upload: empat rak dengan 25 slot/rak; semua slot dapat diklik. Popup menyediakan mapping, label, warna tersimpan dan kontrol manual. Warna tersimpan ditampilkan meskipun box padam. Setting jumlah LED rak terpisah dari popup box.

Kontrol per-box steady/blink, RGB, brightness dibatasi global, default timer 60 detik (1 detik–1 jam). Beberapa box bisa menyala bersamaan. Timer lokal tetap berjalan saat Wi-Fi putus. Respons HTTP bukan konfirmasi optik LED.

Tes hardware per rak mengikuti jumlah LED terpasang, termasuk area tanpa mapping. Program otomatis running, blink bergilir, rainbow dan breathe; mode tes bertimer atau idle kontinu di RAM. Perintah manual aktif mendapat prioritas; hardware test menimpa output terkait. Stop program mempertahankan perintah manual; all-off/config/OTA/reboot menghentikan program. Idle tidak otomatis hidup lagi setelah reboot.

API utama: GET status/boxes/config, PUT config, PUT boxes/{id}/state, POST all-off; semuanya di `/api/v1`. Program memakai PUT `/api/v1/program`. Baca API.md untuk kontrak lengkap. Client Python memakai timeout dan tidak mengulang perintah nyala otomatis setelah timeout.

v0.6.1 memperbaiki status web setelah timeout: GET 10 detik, mutation 5 detik tanpa retry; polling dilewati saat UI sibuk; alarm lama dibersihkan saat status pulih.

## Bug penting yang jangan diulang

NeoPixel 1.15.2 ESP32/IDF5: constructor kosong lalu setPin/updateType/updateLength/begin tidak menginisialisasi shared RMT mutex. API tampak sukses tetapi semua LED padam. Driver kini menggunakan constructor berparameter untuk empat strip, masing-masing 75 LED, GRB/800 kHz. Jangan mengganti kembali ke constructor kosong. Tes driver menautkan implementasi NeoPixel asli dengan shim batas hardware.

## Build dan batas bukti

- Versi tunggal `02_Firmware/WarehouseLed/firmware_version.h` = v0.6.1.
- Pinned: Arduino-ESP32 3.3.10, ArduinoJson 7.4.2, Adafruit NeoPixel 1.15.2.
- Perangkat terakhir: ESP32-S3 rev0.2, USB-UART CH340; board dilaporkan flash16MB/PSRAM8MB, tetapi profil yang teruji menggunakan flash4MB, PSRAM disabled, USB hwcdc, **CDCOnBoot=default (disabled)**. Port COM lama bukan jaminan port laptop baru.
- Build terakhir yang di-upload memakai FQBN `esp32:esp32:esp32s3:FlashSize=4M,PartitionScheme=default,PSRAM=disabled,USBMode=hwcdc,CDCOnBoot=default`. Script build.ps1 lama memakai CDCOnBoot=cdc; jangan menyamakan dua profil itu. Cocokkan profil sebelum build berikutnya.
- Partition sketch: dua slot aplikasi 1280 KiB, NVS 128 KiB pada 0x290000. OTA hanya mengganti aplikasi; jangan erase flash atau mengubah partition table untuk migrasi biasa.
- Hasil historis v0.6.1: flash1064217/1310720, static RAM61500/327680. Build ini mendahului aturan timestamp build 2026-10-12; **tidak memiliki identitas build UTC/gate/manifest yang diwajibkan aturan baru**. Jangan mengarang timestamp dari waktu file/upload.
- Host: 11 tes Python, domain, codec, driver NeoPixel, animasi dan DOM UI lulus pada sesi sebelumnya. Hardware HTTP empat program, timer, prioritas, brightness clamp, invalid input lulus. OTA berpassword berhasil, kembali STA, mapping/warna terjaga, OTA tertutup, LED padam saat reboot.
- Pengguna mengonfirmasi tiga LED GPIO4 menyala pada v0.5.1. Mapping kemudian menjadi 25 box/75 LED rak1; B02 biru tersimpan melewati reboot. Pengguna bisa mengubah konfigurasi sewaktu-waktu: baca kondisi aktual sebelum menulis.
- Belum terbukti: seluruh pola secara optik, 75/300 LED dan empat output serentak, distribusi daya, tegangan ujung, suhu kabel, router recovery penuh dan endurance delapan jam. Ini baseline lab, bukan sertifikasi gudang.

Tidak ada build/upload baru dalam tugas handoff 2026-10-20. Versi sebelum/sesudah tetap v0.6.1; API/pin/NVS/perilaku tidak diubah. Status firmware perangkat hari ini tidak ditentukan hanya dari source Git.

## Aturan baru yang wajib diterapkan sebelum perubahan firmware berikutnya

Vault rules terpisah: https://github.com/TaufikAS0/2026-vault-firmware-rules (aksesnya terpisah dari repository public ini; snapshot aturan ada dalam paket privat).

Baca penuh, berurutan:
1. `00_Start_Here/Firmware_AI_Vault_Home.md`
2. `01_Rules/Rules_Nama_dan_Folder_Workspace.md`
3. `01_Rules/Rules_Firmware_Standar.md`
4. `01_Rules/Rules_GitHub_Gitflow.md`
5. `01_Rules/Rules_Kredensial_WiFi.md`
6. `04_Profiles/WiFi/WiFi_HuaweiJIN.md`
7. `01_Rules/Rules_Versioning.md`
8. `01_Rules/Rules_Change_Notifications.md`

Aturan versi/notifikasi baru membutuhkan metadata UTC yang dibuat ulang saat build, satu header generated bersama, gate metadata versi/timestamp dalam binary, manifest SHA-256, tampilan serial/API/dashboard, dan laporan perubahan. Implementasikan sebelum build/upload firmware baru; jangan menganggap baseline lama sudah patuh.

Profil lab wajib STA HuaweiJIN dan AP default12345678. Pada v0.6.1 jaringan tersimpan dalam record JSON key `config` namespace `warehouse`; credentials AP/token/OTA berada pada record `credentials`. Mengganti default source tidak otomatis mengganti NVS. Saat migrasi berikutnya, ubah hanya nilai jaringan yang diperlukan saat berbeda dan pertahankan mapping/warna/secret OTA. Jangan menerapkan penghapusan key robot lain ke schema ini.

## Instruksi singkat untuk AI di laptop baru

> Lanjutkan controller gudang dari feature/warehouse-led-controller, bukan develop. Baca 04_Dokumen/AI_Handoff.md dan seluruh aturan vault dalam paket privat terlebih dahulu. Gunakan percakapan sebagai konteks historis; keputusan terbaru menggantikan rencana awal yang menyebut API token. Audit Git dan perangkat sebelum perubahan. Pertahankan web/API LAN terbuka, popup box, setting rak terpisah, warna tersimpan dan program idle. Baca credential perangkat dari paket lokal privat jika perlu OTA; jangan cetak atau commit credential. Terapkan aturan metadata build baru sebelum build firmware berikutnya. Jangan mengklaim tes fisik dari respons HTTP. Jangan auto-merge PR atau mengubah source JARVIS.

Paket privat adalah ekspor konteks, bukan impor sesi otomatis ke produk AI lain. Berikan file START_HERE.md dan THREAD_CONVERSATION.md kepada AI tujuan; arsip JSONL disediakan sebagai bukti lengkap yang tersedia pada mesin asal. Autentikasi GitHub untuk push dilakukan sendiri di laptop baru, bukan menyalin token akun dari komputer asal.
