# Arsitektur v0.7.0

Sketch tipis -> App -> Controller/SettingsStore/LedDriver/WifiService/OtaService/WebService.
Domain tidak memakai jaringan/flash; timer lokal terus berjalan tanpa router.
Codec menerapkan batas JSON sebelum konversi field kecil. API/web 1-based, internal 0-based.

## Output dan animasi

Empat NeoPixel berconstructor panjang75/pin/type GRB800kHz; GPIO4–7.
Constructor kosong lama tidak boleh digunakan. Buffer tetap75/output.
AnimationProgram menyusun indeks output/start_led, tanpa mengurutkan record config.
Waktu idle diakumulasi uint64 dari delta millis uint32; finite expiry dilatch mati.
HardwareTests juga menghapus flag on saat expiry; set menggantikan tes sebelumnya.

API memadamkan state manual saat mulai tes/program; manual baru membatalkan tes/program.
Driver membatasi render tes ke satu rak dan memprioritaskan hardware test bila dua state
internal tidak sengaja aktif. Semua frame hitam yang berubah dikirim sebelum frame nyala;
pergantian output4->1 tidak boleh menyalakan rak baru sebelum rak lama padam.
Operasi manual biasa tetap mendukung beberapa box/rak bersamaan.
show() tetap sinkron; tidak ada delay di loop, tetapi bukan seluruh stack non-blocking.

## Config, profil dan transaksi

Namespace NVS warehouse; key config_blob adalah blob JSON schema1 + jaringan + revision
sejak v0.7.1 (menghindari batas string4000byte). Legacy string config dibaca hanya bila
blob belum ada dan tidak dihapus saat migrasi; blob rusak tidak fallback ke config usang.
Legacy tanpa revision memakai0. Commit sukses menambah revision dan memadamkan state.
UI memeriksa revision; update dari klien lain memuat config bila bersih, atau menandai
konflik tanpa menimpa draft global/popup. Record credentials terpisah tetap dipertahankan.

Lima key profile1..profile5, setiap key satu blob maksimal8192 byte.
Format WLP1: magic4, panjang total uint16 little-endian, CRC32 IEEE payload uint32
little-endian, lalu nama profil length-prefixed, brightness, empat jumlah LED,
boxCount uint8, dan tiap box: ID/name length-prefixed, output/start/count, RGB.
Maksimal8065 byte pada100 box/field terpanjang. Tidak menyalin padding/raw struct.
Profile revision adalah CRC32 seluruh record; config revision adalah counter persisted.
Repository profil memakai adapter Preferences dan dapat diuji dengan backend fault injection.
Tidak ada index multi-key; overwrite/delete bekerja pada satu key.
Alokasi record/buffer berada di heap, bukan temporary Config8KB pada stack loop.

Config/profil divalidasi sebelum write. Runtime hanya berubah setelah write sukses.
Config rusak memakai default aman dan storage_error, record rusak tidak ditimpa otomatis.
Profil rusak tidak menghalangi boot config aktif. Menyimpan/memilih profil tidak menerapkan.
Apply menyimpan config menggunakan jaringan aktif, lalu memadamkan output/program.
Power-loss atomicity/fragmentasi NVS nyata tetap perlu diuji pada board.

Partisi tidak berubah: app0@0x10000/app1@0x150000 masing-masing1280KiB;
NVS128KiB@0x290000. OTA hanya aplikasi, tanpa migrasi partisi atau erase.

## Jaringan, build dan akses

Build menghasilkan generated_lab.h dari file privat diabaikan Git, bukan credential di source tracked.
Saat boot, migrasi membandingkan jaringan terhadap HuaweiJIN dan AP12345678, hanya menulis
jika berbeda; mapping/warna/token/secret OTA dipertahankan. Config rusak tidak dimigrasikan.
Wi-Fi tetap menunggu provisioning credentials, fallback AP setelah15detik,
STA retry30detik, AP ditutup setelah STA berhasil. Modem sleep dimatikan.

HTTP LAN terbuka sesuai keputusan pengguna, embedded UI tanpa CDN/filesystem.
OTA tetap password perangkat, manual10menit, STA-only, reboot default tertutup/padam.
Transfer OTA/WebServer dapat menahan loop saat I/O; acceptance responsiveness belum digantikan tes host.

build.ps1 menghasilkan header UTC/nonce bersama setiap pemanggilan, mengompilasi pinned,
memverifikasi marker versi/UTC/nonce pada binary dan layout partisi, lalu manifest SHA256.
Versi/UTC terlihat di serial/API/HTML. Manifest tidak mengklaim uploaded/hardware_verified.
