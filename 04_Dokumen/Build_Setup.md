# Build, USB setup, dan OTA

## Dependency

| Komponen | Versi |
|---|---|
| Arduino-ESP32 | 3.3.10 |
| ArduinoJson | 7.4.2 |
| Adafruit NeoPixel | 1.15.2 |

Script mencari arduino-cli di PATH atau instalasi Arduino IDE pengguna. Alternatif: set `ARDUINO_CLI` atau pass `-ArduinoCli <path>`. Tidak menyimpan path laptop dalam source. Library project dipasang pada `_sandbox/sketchbook/libraries`; library global proyek lain tidak ditimpa.

```powershell
arduino-cli core install esp32:esp32@3.3.10
./scripts/build.ps1 -InstallDependencies
./scripts/build.ps1
```

Jika CLI tidak di PATH, gunakan path CLI yang ditemukan atau env ARDUINO_CLI untuk perintah instalasi core. Build menggunakan profile **generic ESP32-S3, flash4MB, default dua slot OTA, PSRAM disabled, USB hwcdc / CDCOnBoot aktif**. Profile ini baseline compile, bukan hasil identifikasi board fisik.

Sketch menyertakan `partitions.csv`: dua slot OTA masing-masing1280 KiB, NVS128 KiB pada0x290000, application offset0x10000. Arduino-ESP32 menggunakan tabel sketch ini menggantikan tabel default. NVS yang diperbesar diperlukan agar record mapping100 box bisa diganti tanpa kekurangan ruang. Tidak ada filesystem web.

## Upload pertama

1. Identifikasi board, flash, pin, USB port, dan catu sebelum menghubungkan strip.
2. Pilih ESP32S3 Dev Module di Arduino IDE, samakan opsi build di atas dan gunakan port USB yang benar. Jika board memakai USB-UART saja, sesuaikan CDC/USBMode dan rebuild; jangan menebak port.
3. Upload sketch `02_Firmware/WarehouseLed/WarehouseLed.ino` lewat USB termasuk partition table. Jika board sebelumnya memakai firmware lain, lakukan erase flash manual setelah mengamankan data lama; NVS pindah lokasi. Jangan mengubah partition table perangkat lama hanya melalui OTA. ESP tidak mengendalikan kontaktor/rangkaian lain dalam proyek ini.
4. Buka monitor115200 baud dengan newline. Boot log menampilkan versi tanpa secret; kirim `info` untuk IP/status.
5. Kirim `provision` sekali. ESP menghasilkan token64 hex dan password OTA32 hex, memakai default AP `12345678`, dan menyimpannya di NVS. Tidak ada jaringan sampai provisioning berhasil. Baca/simpan secret langsung pada perangkat, jangan commit output.
6. Untuk membaca kembali secret via akses fisik USB, kirim `credentials`. Provision ulang saat sudah provisioned ditolak. Penghapusan NVS untuk reset lengkap adalah tindakan servis manual yang menghapus mapping dan secret.
7. Sambungkan AP Warehouse, buka192.168.4.1 tanpa token. Isi Wi-Fi router dan mapping; Simpan. Cari DHCP IP melalui router atau USB `info`.

Untuk perangkat yang sudah provisioned, kirim `ap-password 12345678` melalui USB115200 dengan newline. Firmware menyimpan hanya perubahan password AP, mempertahankan token/OTA/mapping, lalu memulai ulang jaringan. Password dapat berupa8..63 karakter ASCII printable; input salah atau gagal tulis tidak mengganti nilai aktif. Default bersama mudah ditebak dan digunakan atas keputusan pengguna untuk setup internal. OTA tetap berpassword unik.

## OTA

OTA default mati. Tekan Aktifkan OTA10 menit melalui web, ketika STA tersambung; semua LED padam. Upload melalui Arduino IDE network port menggunakan password OTA dari USB. Boot baru menutup OTA dan memadamkan LED lagi. Jaga USB sebagai recovery. Jangan menjalankan acceptance gudang saat transfer OTA berlangsung.

Mulai v0.3.2, toleransi jeda data OTA10 detik dan log USB menampilkan start/end/error tanpa secret. Wi-Fi power save dimatikan agar HTTP/OTA lebih responsif pada controller bercatu tetap; idle power dapat meningkat. Transfer ArduinoOTA bersifat sinkron dan web dapat sementara tidak merespons. Jendela10 menit membatasi aktivasi pemeliharaan, bukan bukti transfer berhasil.

Pada komputer dengan beberapa adapter satu subnet, gunakan listener uploader `0.0.0.0` agar koneksi balik TCP tidak menuju adapter yang berbeda dari IP sumber undangan UDP. Contoh portable setelah mengaktifkan OTA, dengan password dari penyimpanan pribadi:

```powershell
python <path-espota.py-dari-core> -i <IP-controller> -I 0.0.0.0 -P 3233 -a $env:WAREHOUSE_OTA_PASSWORD -f _sandbox/build-uart/WarehouseLed.ino.bin
```

Jangan menyimpan password nyata atau IP/port lokal sebagai default source. Cocokkan profile build dan partition table perangkat; OTA memperbarui aplikasi dan tidak mengganti partition table. Panduan penggunaan web: [Web Operator Guide](Web_Operator_Guide.md).

## Pengujian host

Butuh Python3.11+, Node18+, compiler C++17, dan library ArduinoJson pinned dari instalasi build. Gunakan g++ pada PATH, `WAREHOUSE_CXX`, atau `./scripts/test.ps1 -Cxx <compiler>`.

Alternatif compiler lokal terisolasi yang dipakai dalam verifikasi awal:

```powershell
python -m pip install --target _sandbox/python-tools ziglang==0.14.1
npm install --prefix _sandbox/ui-tests jsdom@26.1.0 --no-audit --no-fund
./scripts/test.ps1
```

Adapter tests/host_string.h hanya menyediakan operasi String yang dipakai codec; parser JSON menggunakan library ArduinoJson asli. Host tests tidak mensimulasikan hardware RMT, flash NVS, Wi-Fi, OTA, maupun transfer HTTP WebServer ESP32.

UI DOM tests menjalankan HTML/JavaScript firmware asli di jsdom dengan HTTP fixture sintetis. Server browser fixture opsional: `python tests/ui_fixture.py` pada loopback8769; ini bukan firmware ESP32.

## Akses v0.5.0

Web dan HTTP API tidak memerlukan token. USB provision tetap menyiapkan AP dan password OTA pada perangkat baru. Token yang tercetak pada firmware lama sudah tidak digunakan untuk HTTP. Pada perangkat yang sudah dikonfigurasi, buka IP langsung dan klik box atau tes hardware rak.
