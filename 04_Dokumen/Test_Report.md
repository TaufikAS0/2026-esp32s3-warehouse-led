# Verifikasi v0.7.1 — 2026-10-20

## Perbaikan503 konfigurasi100box — v0.7.0 -> v0.7.1

- Akar masalah terkonfirmasi pada implementasi: SettingsStore memakai putString/nvs_set_str
  dengan batas4000byte termasuk NUL; JSON100box default8671byte sebelum Wi-Fi.
  Stub sebelumnya menerima string tanpa batas. Error NVS spesifik percobaan pengguna
  tidak tersedia melalui API, sehingga tidak ada klaim membaca log flash kegagalan itu.
- Config baru memakai blob config_blob, legacy string config tetap terbaca bila blob belum ada.
  Legacy/credentials/profil tidak dihapus, blob corrupt tidak fallback ke mapping legacy usang.
- Suite host15Python/7C++/DOM lulus. Storage memakai kode asli, stub dengan batas string
  dan tipe NVS:100box default+field maksimum, save ulang, reload/reboot simulasi,
  failure/OOM/revision dan corrupt isolation. API failure injection diperbarui ke key blob.
- Build UTC2026-10-20T10:25:13Z, pinned core3.3.10/Json7.4.2/NeoPixel1.15.2,
  UART/CH340 CDCOnBoot=default. Flash1089845/1310720, RAM61628/327680byte;
  app binary1090000byte, SHA2568eaa5624d761b20d5f245636a63c1303b7f6711d26137c2625dc3cc535ce0d51.
  Gate metadata/partisi dan manifest lulus.
- OTA app-only berpassword sukses pada2026-10-20T10:27:29Z di192.168.1.28,
  Warehouse-9FC114; identitas runtime cocok nonce59e2ae105ea3462a982b7306d68da0bc,
  dashboard cocok source, jaringan/config sebelum upgrade terjaga, OTA tertutup/driver ready.
- Uji API pada ESP nyata pada2026-10-20T10:27:41Z: PUT100box default, GET exact roundtrip,
  PUT ulang100box, create/read/update profil100box, apply profil100box: seluruhnya sukses.
  Konfigurasi awal1box/mapping/warna/output/brightness/Wi-Fi dikembalikan persis;
  revision maju21->25. Profil tes dihapus dan daftar profil kembali sama seperti sebelum tes.
  Tidak ada perintah LED on/program/hardware test. Semua state padam.
- Bukti lokal ignored _sandbox/ota-v071/{verification.json,storage-test.json,snapshot API}.
  manifest uploaded=false adalah snapshot waktu build; kejadian OTA tercatat terpisah.
- Batas:100box reboot fisik sesudah save belum diuji; hanya reload host dan GET langsung
  perangkat. Tidak ada uji optik, listrik, power-loss/endurance/fragmentasi NVS.
  Tidak ada commit/push/merge; branch feature/warehouse-led-controller, base347751a.

## Riwayat implementasi v0.7.0

Previous v0.6.1 -> source v0.7.0. Branch feature/warehouse-led-controller,
base347751a. Perubahan lokal belum commit/push/merge. OTA dilakukan kemudian atas instruksi pengguna.

## Verifikasi OTA nyata — 2026-10-20T10:07:55Z

- Controller Warehouse-9FC114 di192.168.1.28: v0.6.1 -> v0.7.0 melalui OTA app-only berpassword; uploader return0.
- Identitas setelah reboot cocok dengan manifest: build UTC2026-10-20T09:17:45Z,
  nonce05dc08cedb4d4bf38032b0b9b3629dda. SHA256 binary cocok sebelum transfer.
- Mapping/warna35box, outputs75/75/75/75 dan config Wi-Fi sama persis sebelum/sesudah.
- STA kembali, driver ready, storage_error kosong, OTA tertutup; seluruh box/program/tes berstatus padam.
- Dashboard yang disajikan cocok byte-per-byte dengan source setelah ekspansi macro versi/UTC.
  GET profiles merespons capacity5 dan daftar kosong; tidak membuat profil atau mengubah mapping perangkat.
- Bukti lokal ignored: _sandbox/ota-v070/verification.json dan snapshot API sebelum/sesudah.
  Manifest build uploaded=false adalah snapshot saat build; kejadian upload dicatat terpisah di verification.json.
- Tidak menyalakan tes LED. Status API padam bukan pengamatan optik. Belum ada bukti optik v0.7.0,
  tes75LED, heap stress, NVS power-loss, endurance atau validasi seluruh pola pada perangkat nyata.

## Build final

- Build UTC aktual: **2026-10-20T09:17:45Z**, bukan waktu upload/uptime.
- FQBN: esp32:esp32:esp32s3:FlashSize=4M,PartitionScheme=default,PSRAM=disabled,USBMode=hwcdc,CDCOnBoot=default.
- Core3.3.10 / ArduinoJson7.4.2 / NeoPixel1.15.2, flash4MB/noPSRAM/UART-CH340.
- Flash menurut compiler: 1087373/1310720 byte; static RAM 61628/327680 byte.
- Binary aplikasi: 1087520 byte; SHA-256: 4c791ec842c5314547240fa0e57f6b3d0e386fc4cecb04d72f41f9d9b4ad8463.
- Header generated bersama dan gate versi/UTC/nonce binary lulus. Partisi dua slot1280KiB
  serta NVS128KiB@0x290000 lulus. Manifest lokal pada _sandbox/build/manifest.json.
- Build awal menampilkan warning missing-field-initializers dari dependency core/TinyUSB;
  tidak ditemukan error source proyek. Build final berhasil.
- Profile lab privat disiapkan lokal; header/binary ignored, nilai tidak dicatat di dokumen.

## Pengujian yang dilakukan sendiri

| Pengujian | Hasil / batas |
|---|---|
| Python | 15 tes:11 client HTTP loopback +4 metadata, termasuk stale identity/size/partition rejection |
| Domain | Kapasitas100box/300posisi, overlap/ID/range, manual timer/clamp, transaksi gagal |
| AnimationProgram | Config sengaja diacak, output/start_led ordering, skip rak kosong, satu rak/<=75LED, rainbow per pixel, breathe RGB, expiry/long uptime |
| Profil | 5x100box, maksimum field, roundtrip8065-byte limit, format/length/CRC/UTF-8, duplicate/full/revision, simulasi COW128KiB, gagal write/reboot/corrupt isolation |
| NeoPixel asli | Constructor berparameter, GRB bytes hijau/merah/cyan, frame dedup; setiap show diverifikasi satu rak;4->1 clear-before-light |
| Codec | ArduinoJson asli, schema1/warna legacy, tipe/range/100box |
| SettingsStore asli | Preferences/RNG/serial stub: legacy config, targeted HuaweiJIN/AP migration, no-op tanpa write, revision, gagal write dan String OOM tidak menulis JSON terpotong, secret/mapping/warna terjaga |
| Handler API asli | WebServer/WiFi/OTA/strip boundary stub: CRUD5profil/apply,428/409/400/404/405/422/503, credential exclusion, failed writes mempertahankan config/manual, tes eksklusif dan manual membatalkan tes, OTA STA guard |
| DOM web | Embedded JS di jsdom:100box A–D/cyan/label-, profil preview/save/draft/apply/update/delete/export/import, legacy/custom ID, dirty draft/popup conflict preservation, status recovery |
| Browser nyata ke fixture lokal | SIMULASI loopback: generator100box dan simpan profil sambil config aktif tetap1box; preview75LED/rak; screenshot _sandbox/v070-profiles-simulation.png |

Suite scripts/test.ps1 dan scripts/build.ps1 lulus. Tes host handler tidak memakai
socket WebServer ESP32; Preferences/strip stub bukan flash/RMT/LED fisik.
Fixture browser berlabel SIMULASI, hanya loopback; server/tab pengujian sudah ditutup.

## Batas dan penerimaan berikutnya

- OTA/IP dan identitas runtime telah diverifikasi pada bagian OTA di atas; COM tidak diakses.
- Belum membuktikan LED optik, sinyal/timing,75LED, listrik/suhu, recovery router/interupsi OTA,
  heap stress dan ketahanan NVS terhadap putus daya/fragmentasi.
- Rencana fisik setelah instruksi terpisah:3LED, lalu satu rak <=75LED secara bergantian.
  Tidak menguji beberapa rak bersamaan dalam mode tes.
- HTTP API LAN tetap terbuka; OTA tetap password perangkat/manual10menit. Boot padam.
- Migrasi jaringan pada perangkat belum dijalankan. Build menargetkan HuaweiJIN/AP12345678
  dari sumber privat, hanya nilai berbeda yang ditulis; mapping/warna/OTA dipertahankan.
- File source/doc repository diperiksa terhadap credential lab privat; tidak ada kecocokan
  di luar header/artefak ignored. Ini bukan audit seluruh secret dalam riwayat Git.

---

# Laporan historis v0.6.1 dan sebelumnya

Bagian berikut adalah bukti dari sesi asal, bukan pengujian ulang perangkat pada tugas v0.7.0.

# Laporan verifikasi

## v0.6.1 — pemulihan status web, 2026-10-07

- Pesan timeout polling lama dibersihkan setelah pembacaan status sukses. GET web memakai timeout10 detik, mutation tetap5 detik tanpa retry. Polling dilewati selama perintah UI berjalan untuk mengurangi request status bersamaan. Tes DOM transport failure→recovery memastikan alarm lama hilang dan kontrol aktif kembali.
- Seluruh suite host lulus. Build target sama: flash1064217/1310720 byte (81%), RAM61500/327680 byte (18%).
- Authenticated OTA v0.6.1 sukses; versi/HTML sesuai source, STA kembali, OTA tertutup, program/box padam setelah reboot. Mapping25 box/75 LED serta warna B02 biru yang disimpan lewat popup bertahan melewati reboot OTA. Pola/render tidak berubah dari v0.6.0; pengamatan optik pola baru tetap memerlukan konfirmasi pengguna.

## v0.6.0 — popup, warna konfigurasi dan program otomatis, 2026-10-07

- Jumlah LED rak terpisah dari popup box, dengan Simpan setting rak yang memakai mapping tersimpan dan tidak menyimpan draft mapping/jaringan lain. Popup menyediakan mapping/warna konfigurasi dan kontrol manual, dapat ditutup tombol/Escape/backdrop, serta keyboard focus trap. Kartu menampilkan warna tersimpan walaupun padam.
- Field warna box optional pada schema1, default hijau untuk NVS lama. Running/kedip memakai warna tiap box; rainbow/breathe mengikuti mapping. Tes bertimer dan idle kontinu memakai RAM, brightness global, tanpa delay. Perintah manual aktif termasuk fase gelap blink mendapat prioritas; hardware test menimpa output. All-off/config/OTA/reboot menghentikan program.
- Tes host lulus:11 Python, domain/transactions, actual NeoPixel driver, codec termasuk kompatibilitas warna lama/invalid/100 box, animasi mapping/timer/rollover/idle/manual priority, DOM100 slot/popup/Escape/save warna/output terpisah/draft preservation/program controls. Ini simulasi software, bukan pengukuran RMT fisik.
- Build final core3.3.10 / NeoPixel1.15.2 / ArduinoJson7.4.2, S3 UART,4MB, PSRAM off: flash1063993/1310720 byte (81%), RAM61500/327680 byte (18%). Warning initializer hanya ditemukan pada dependency core pada build awal; build final cached lulus.
- Authenticated OTA v0.6.0 berhasil. Reboot kembali STA, OTA tertutup, seluruh box/program padam. Mapping aktif25 box /75 LED Rak1 dan jaringan lama tetap tersimpan; warna lama mendapat default hijau. Root HTML cocok persis dengan source.
- HTTP perangkat nyata: empat pola, finite expiry, idle melampaui durasi tes, global brightness clamp, invalid type/pattern/range400, manual blink priority, stop program mempertahankan manual, all-off dan konfigurasi tidak berubah selama tes lulus.
- Chrome perangkat nyata setelah OTA: v0.6.0 LIVE, popup B02, Simpan box warna biru, warna #0000FF bertahan setelah reload, mapping25 box tetap. Tes running60 detik dijalankan dari browser. Warna B02 sengaja menjadi biru; mapping/jaringan lain dipertahankan. Pengamatan optik pola baru dicatat terpisah dari status HTTP. Endurance, empat output simultan serta listrik tetap belum disertifikasi.

## v0.5.1 — perbaikan driver tanpa keluaran LED, 2026-10-06

- Pengguna melaporkan LED tidak menyala pada firmware gudang, tetapi program pembanding `esp32s3-ws2812b-web` berhasil pada rangkaian yang sama. Kedua program memakai GPIO4 dan GRB/800 kHz.
- Akar penyebab: empat objek NeoPixel dibuat dengan constructor kosong. Pada NeoPixel1.15.2 dan ESP32 IDF5, constructor ini tidak memanggil `espInit()`; setPin/updateType/updateLength/begin tidak menggantikannya. Mutex bersama tetap kosong, sehingga espShow tidak mengirim sinyal meskipun buffer/API berhasil. Program pembanding memakai constructor berparameter yang memanggil espInit. Pemeriksaan source lokal1.15.5 juga menunjukkan perbedaan constructor yang sama.
- Driver kini mengonstruksi keempat strip langsung dengan panjang75, pin terpusat dan GRB/800 kHz. Return begin dan alokasi diperiksa; tidak ada salinan objek yang memiliki buffer.
- Tes regresi mengompilasi implementasi NeoPixel asli yang dipin bersama LedDriver. Shim hanya menggantikan GPIO/clock dan batas espInit/espShow: pola lama direproduksi tanpa inisialisasi/transmisi; pola baru menginisialisasi empat strip, mengirim frame GRB tiga LED hijau GPIO4 dan75 LED merah GPIO7, menahan frame identik serta memadamkan semua buffer. Ini bukan pengukuran timing RMT fisik.
- Suite domain, driver, codec,11 Python tests dan UI DOM lulus. Compile S3 UART/CH340, core3.3.10,4MB, PSRAM off: flash1053517/1310720 byte (80%), RAM61180/327680 byte (18%).
- Authenticated OTA v0.5.1 sukses; reboot kembali STA, mapping satu box/tiga LED dan Wi-Fi tetap, OTA tertutup, seluruh state padam. HTML perangkat cocok dengan source dan versi terlihat di Chrome.
- Tes fisik awal: dari Chrome menyalakan R01-B01/GPIO4/LED1–3 hijau, brightness50, timer60 detik. Pengguna mengonfirmasi **ketiga LED fisik menyala**. Bukti ini mengesahkan nyala tiga LED pada rangkaian awal; RGB lengkap,75/300 LED, empat output RMT simultan, listrik dan8 jam masih belum diuji.
- Konfigurasi B02 melalui Chrome: mapping LED4–6 ditolak ketika jumlah LED Rak1 masih3. Setelah jumlah output diubah menjadi6, R01-B02 berhasil disimpan pada LED4–6, B01 tetap LED1–3, dan mapping bertahan setelah halaman dimuat ulang. Perintah B02 biru/brightness50/timer60 detik diterima; dashboard menunjukkan B02 aktif dan B01 padam. Pengguna menerima hasil dan mengizinkan publikasi. Tidak ada perubahan source atau versi firmware untuk penyesuaian konfigurasi perangkat ini; warna/jumlah LED fisik B02 tidak dikonfirmasi secara terpisah.

## v0.5.0 — akses terbuka dan tes hardware rak, 2026-10-06

- Instruksi pengguna mengubah web/HTTP API menjadi akses LAN tanpa token. Mapping/status dimuat otomatis, dan versi firmware dirangkai pada HTML langsung dari satu macro firmware_version.h. Token browser lama dibuang.
- Empat panel rak mempunyai tes merah/hijau/biru/putih/custom/kedip, brightness per tes, durasi3 detik, dan Padam. Mengikuti jumlah LED terpasang; output0 LED tidak dapat dites. API mengizinkan1–30 detik, default3 detik. Seluruh tes hanya di RAM.
- Tes hardware menghentikan box aktif pada rak terkait; tes tidak menyalakan kembali box setelah expiry. Perintah box membatalkan tes output itu. All-off/config/OTA/boot membersihkan tes.
- Host tests domain/codec,11 Python tests dan HTML/JS DOM lulus. Kasus hardware: range3/75 LED, tail padam, batas brightness, blink, expiry, rollover uint32, pembatalan output, clear; UI otomatis membaca tanpa Authorization dan tombol rak0 LED dinonaktifkan.
- Compile S3 UART/CH340 core3.3.10 lulus: flash1053445/1310720 byte (80%), RAM61180/327680 byte (18%). Peringatan missing-field-initializers berasal dari header dependency core/USB/hal.
- Authenticated OTA v0.5.0 lulus; reboot kembali STA, mapping/Wi-Fi tetap tersimpan, seluruh LED padam, OTA tertutup. Root HTML cocok dengan source termasuk versi yang dirangkai saat compile.
- HTTP tanpa Authorization lulus: status/config, tes3 LED sesuai jumlah terpasang, brightness200 dibatasi128, warna/kedip, expiry3 detik, perintah box membatalkan tes, all-off, penolakan rak0 LED/durasi>30 detik/output5. Konfigurasi tidak berubah.
- Chrome perangkat nyata: auto-load tanpa token, label Firmware v0.5.0, klik B02 membuka editor, klik B01 dan Simpan box lulus, tombol tes hardware hijau/biru rak1 mengirim LED1–3 pada brightness32 selama3 detik. Konfirmasi optik LED dan pengujian75/300 LED tetap terpisah.

## v0.4.0 — editor box langsung, 2026-10-06

- Semua 100 slot awal dapat dipilih, termasuk box belum dikonfigurasi. Editor per box menyediakan ID, nama, rak/output, LED awal, jumlah LED box dan jumlah LED terpasang. Tes hanya tersedia untuk mapping tersimpan.
- Suite domain/codec, 11 tes Python, serta DOM HTML/JS lulus. Kasus tambahan: tambah box lewat slot kosong, ganti ID/nama, tes box hasil rename, rentang melebihi output, overlap, ID duplikat, penolakan penyimpanan 503, dan proteksi draft konfigurasi global. Penolakan menjaga mapping/state lama dan draft editor.
- Compile core 3.3.10 / generic S3 / flash4 MB / PSRAM disabled / hwcdc / CDCOnBoot disabled lulus: flash1049101/1310720 byte (80%), RAM61116/327680 byte (18%).
- Upload USB COM10 lulus. Perangkat sempat diganti ke program Running LED oleh pengguna saat verifikasi; firmware dashboard diunggah ulang, lalu root HTML dibandingkan persis dengan source. Karena ada pergantian firmware selama sesi, kegagalan OTA awal tidak dapat disimpulkan sebagai cacat autentikasi atau transfer firmware gudang.
- Setelah firmware stabil, authenticated OTA v0.4.0 berhasil. Reboot kembali STA, mapping/Wi-Fi tetap tersimpan, semua LED padam dan OTA tertutup. Kredensial tetap privat.
- HTTP perangkat nyata lulus untuk simpan perubahan nama satu box tanpa mengubah Wi-Fi, penolakan box baru di luar jumlah LED terpasang, RGB/blink brightness32, serta expiry3 detik. Mapping asli satu box/tiga LED dikembalikan dan seluruh LED dipadamkan setelah tes.
- Verifikasi ulang visual browser pada versi ini sempat terhambat koneksi CDP browser Codex; hasil DOM otomatis dan HTTP perangkat dicatat terpisah dari klik browser aktual. Nyala optik LED, 75/300 LED, listrik dan endurance belum disertifikasi.

## Update v0.3.2 — 2026-10-06

| Pemeriksaan | Hasil dan batas bukti |
|---|---|
| Dashboard | Empat rak ×25 slot, pemilihan box, warna/timer, kontrol manual RGB/kedip3 detik, filter, generator LED/box, preset setup awal dan per-alamat, draft/save serta validasi lokal. Semua HTML/CSS/JS embedded, tanpa CDN |
| Host tests | Domain/codec lulus,11 test Python lulus, UI DOM lulus termasuk100 box, RGB/blink, selection stabil saat polling, status stale, batas generator, preset tiap LED dan UTF-8 |
| Compile | Arduino-ESP32 3.3.10, ArduinoJson 7.4.2, NeoPixel1.15.2; S3 UART/CH340,4MB, PSRAM off, CDC disabled. Flash1045069/1310720 byte (79%), RAM statis61116/327680 byte (18%) |
| USB upload | COM10,460800 baud, hash terverifikasi. Mapping/token/AP/OTA tetap tersimpan |
| Wi-Fi fisik | Konfigurasi router disimpan melalui authenticated API, tersambung STA dan menutup AP; kembali ke router setelah USB reboot dan OTA reboot. Kredensial/IP tidak dimasukkan source |
| HTTP ESP32 asli | Bearer benar/salah,401/400/404, konfigurasi/status, perintah RGB satu box dan expiry1 detik. Respons firmware terverifikasi; nyala optik LED belum dikonfirmasi |
| Web perangkat asli | Dibuka di browser pada LAN; v0.3.2,1 box/3 LED, konfigurasi router tersimpan, pemilihan R01-B01, tes hijau brightness32 dan padam sukses. Halaman yang disajikan cocok byte-per-byte dengan source |
| Responsive | Desktop1440px dan mobile390px teramati; mobile tidak overflow horizontal, satu rak awal dan kontrol terpilih dapat diakses. Peta100 box diuji pada fixture, bukan konfigurasi100 box fisik |
| OTA autentikasi | Password salah ditolak. Transfer aplikasi dengan password benar berhasil setelah penyesuaian jaringan/timeout; log USB start/end dan reboot teramati |
| OTA reboot | v0.3.2 kembali ke STA, OTA tertutup, box padam, mapping dan Wi-Fi tetap. Jendela10 menit, recovery saat putus router, interruption/power-loss dan endurance belum divalidasi penuh |

Percobaan OTA awal pada v0.3.0 gagal di tengah transfer melalui adapter pertama; firmware lama tetap berjalan. Percobaan dengan bind adapter lain mendapat no-response akibat koneksi balik menuju IP sumber undangan UDP. Baseline v0.3.2 menambah toleransi jeda data10 detik, log start/end/error dan mematikan Wi-Fi power save. Transfer ulang memakai listener0.0.0.0 berhasil. Ini bukti satu transfer LAN yang sukses, bukan jaminan terhadap seluruh kondisi router.

Firmware/dashboard sesuai cakupan planning software. Acceptance75/300 LED, hasil optik/RMT empat output, pengukuran daya/suhu dan8 jam masih **belum dilakukan**. Tidak ada perubahan source JARVIS aktif. Bukti/log/binary dan screenshot lokal tersimpan di `_sandbox` dan diabaikan Git.

## Update v0.2.0 — 2026-10-06

- Default AP internal `12345678` atas instruksi pengguna; perintah USB `ap-password` untuk perangkat yang sudah provisioned. Token API dan OTA tetap terpisah.
- Compile target ESP32-S3 UART/CH340, core3.3.10, flash4MB, PSRAM off, CDCOnBoot disabled: lulus. Flash1026237/1310720 byte, RAM statis61116/327680 byte.
- Domain, codec, 11 test Python, dan UI DOM: seluruhnya lulus ulang.
- Upload USB COM10 pada460800 baud: sukses, hash terverifikasi. Firmware merespons `v0.2.0`, AP aktif di192.168.4.1 setelah reboot.
- Uji USB pada perangkat: password7 karakter dan input64 karakter ditolak tanpa perubahan credentials; password yang diminta tersimpan dan bertahan setelah reboot. Token API dan password OTA identik sebelum/sesudah perubahan. Record lokal privat diperbarui, tetap diabaikan Git.
- Identifikasi perangkat sebelumnya: ESP32-S3 rev0.2, flash16MB, embedded PSRAM8MB, USB-UART CH340. Profile4MB/no-PSRAM menggunakan subset hardware ini; GPIO masih perlu dicocokkan dengan schematic board.
- LED fisik, autentikasi Wi-Fi dari client, API ESP32, OTA dan endurance belum diuji. Ringkasan di bawah adalah hasil historis v0.1.0, bukan status terbaru provisioning USB.

## Baseline historis v0.1.0

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
