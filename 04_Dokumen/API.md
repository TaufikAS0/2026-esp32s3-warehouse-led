# HTTP API v1 — firmware v0.7.0

Base URL http://<IP-controller>/api/v1. HTTP LAN terbuka tanpa login/token.
Mutation JSON memakai Content-Type: application/json; body maksimal 32768 byte.
Tidak ada retry write otomatis. Timeout write berarti hasil belum diketahui: baca status.

| Method | Path | Fungsi |
|---|---|---|
| GET | /status | Version/build_utc/build_identity, config_revision, jaringan, driver/storage, manual/program/tes |
| GET | /boxes | Mapping, warna tersimpan, output, brightness |
| GET / PUT | /config | Config aktif termasuk revision; PUT mengganti penuh mapping, Wi-Fi opsional |
| PUT | /boxes/{id}/state | Manual steady/blink; menghentikan seluruh tes terlebih dahulu |
| PUT | /outputs/{1..4}/test | Tes eksklusif satu output, default 3 detik, maksimal 30 detik |
| PUT | /program | Running/blink/rainbow/breathe satu rak bergantian |
| POST | /all-off | Hentikan seluruh state/program/tes |
| POST | /ota | enabled boolean; jendela manual 10 menit, STA, upload tetap berpassword |
| GET / POST | /profiles | Daftar/create, kapasitas 5 slot |
| GET / PUT / DELETE | /profiles/{1..5} | Detail/update/delete |
| POST | /profiles/{1..5}/apply | Terapkan profil ke config aktif |

## Config dan revision

GET /config menambahkan revision integer uint32, awal 0 untuk record legacy.
PUT wajib mengirim revision yang sama dengan GET terakhir: hilang/salah tipe 428,
usang 409. Pemeriksaan dilakukan sebelum write; gagal tidak mengubah output/config.
Sukses menaikkan revision, menyimpan record, memadamkan semua output/program.
Revision disimpan bersama mapping dalam key config_blob (v0.7.1); schema mapping tetap1.
Legacy string config tetap dibaca bila blob belum ada; HTTP JSON tidak berubah.

Contoh (ambil revision aktual dahulu):

```json
{"schema":1,"revision":0,"brightness_limit":128,"outputs":[3,0,0,0],
 "boxes":[{"id":"A-01","name":"-","output":1,"start_led":1,"led_count":3,"color":[76,215,246]}]}
```

outputs tepat 4 integer 0..75; boxes 1..100; ID unik ASCII [A-Za-z0-9_-] 1..24 byte,
name maksimal 48 byte, output 1..4, start_led/led_count 1..75. Mapping tidak overlap
dan harus berada dalam jumlah terpasang. ID lama dan custom tetap diterima.
Warna opsional untuk schema lama, default hijau bila tidak ada.
Generator UI membatasi 25 box/rak; API mempertahankan validasi mapping legacy.

wifi opsional; bila dihilangkan jaringan dipertahankan. ssid string maksimal 32 byte.
Password baru wajib eksplisit saat mengganti SSID, kosong untuk router terbuka;
password nonkosong 8..63 byte. SSID kosong menghapus password dan memakai AP-only.
GET hanya memberikan ssid/password_configured. Build lab v0.7.0 menegakkan kembali
profil HuaweiJIN pada boot berikutnya; deployment lain memerlukan keputusan/build lain.

## Manual dan hardware test

```json
{"on":true,"color":[76,215,246],"brightness":128,"mode":"steady","duration_ms":60000}
```

on wajib boolean. Brightness/channel integer 0..255, mode steady/blink,
durasi 1000..3600000 ms. Default brightness128, steady, 60000 ms;
warna yang tidak dikirim memakai warna config box. Blink 500 ms on/500 ms off.
Brightness dibatasi global. off cukup {"on":false}; field tambahan tetap divalidasi.
Beberapa box manual boleh menyala bersamaan dalam operasi normal.

Hardware test memakai payload yang sama; default durasi3000, maksimal30000.
Mulai tes memadamkan semua manual/program/tes sebelumnya, hanya satu rak dites.
Perintah manual berikutnya menghentikan tes/program sebelum diterapkan.
Tes off menghentikan tes tanpa menghapus state manual operasional yang ada.

## Program otomatis eksklusif

```json
{"on":true,"pattern":"running","idle":false,"brightness":32,"step_ms":500,"duration_ms":30000}
```

Pola running/blink/rainbow/breathe. idle defaultfalse, brightness default32,
step_ms 100..5000 default500, duration_ms 1000..3600000 default30000.
Idle mengulang terus; duration_ms tetap divalidasi bila dikirim.
{"on":false} menghentikan animasi. Durasi tes membatasi total program,
bukan waktu per rak; tes dapat berakhir sebelum seluruh rak selesai.

Mulai program memadamkan manual/tes. Urutan output1..4 lalu start_led meningkat,
tanpa mengubah urutan record config. Rak tanpa mapping dilewati.
Running: satu box per langkah. Blink: setengah langkah on, setengah off.
Rainbow: spektrum per posisi LED fisik; satu rak selama 6*step_ms.
Breathe: warna config box, naik/turun intensitas selama 2*step_ms per rak.
Semua pola maksimum satu rak/75 LED; LED tak terpetakan hitam.
Manual berikutnya membatalkan program. Simpan/apply/all-off/OTA/reboot juga menghentikannya.

Status program: on,idle,pattern,brightness,step_ms,remaining_ms,
output (1..4; 0 bila padam), position (1-based, 0 untuk pola seluruh rak),
total (box pada rak), box_id saat running/blink, start_led,end_led,cycle_ms.
Status box mempertahankan state manual on/color/remaining_ms dan menambah
program_on/program_color (warna pixel pertama, sudah diskalakan).
Saat rainbow aktif, program_colors berisi RGB setiap LED box untuk preview per pixel.
Status bukan konfirmasi optik dan tidak mendeteksi strip putus.

## Profil

Envelope JSON portabel:

```json
{"format":1,"name":"Gudang utama","config":{"schema":1,"brightness_limit":128,
 "outputs":[3,0,0,0],"boxes":[{"id":"A-01","name":"-","output":1,"start_led":1,"led_count":3,"color":[76,215,246]}]}}
```

Nama 1..48 byte UTF-8 valid, tanpa kontrol/spasi tepi; nama unik case-sensitive.
config hanya menerima schema/brightness_limit/outputs/boxes, tanpa Wi-Fi/credential/runtime.
POST create memilih slot kosong terkecil, respons201. Penuh/nama duplikat409.
GET list: capacity dan profiles berisi id/name/box_count/valid/profile_revision.
Record rusak tetap muncul valid=false,error dan revision untuk recovery/delete.
GET detail menambah id/profile_revision pada envelope. GET detail rusak422.
PUT update memakai envelope + profile_revision dari GET, tanpa menerapkan profil.
DELETE memakai {"profile_revision":<aktual>}; key lain dan config aktif tetap.
Apply memakai {"revision":<config aktual>,"profile_revision":<profil aktual>}.
Revision tidak ada428; berubah409. Sukses apply mengembalikan GET config terbaru,
memadamkan output, mempertahankan jaringan/credential dan tidak mengubah isi profil.
Tulis gagal503; record lama dipertahankan. Penyimpanan hanya saat aksi eksplisit.

## Error dan client

400 input invalid; 404 ID/path; 405 method; 409 konflik/penuh/OTA tanpa STA;
413 body; 415 content-type; 422 profil rusak; 428 revision wajib; 503 alokasi/driver/storage.
Client Python stdlib tetap kompatibel untuk status/boxes/manual/all-off, timeout5 detik,
tanpa Authorization, proxy mesin atau redirect, tanpa retry write.
JARVIS memetakan barang ke ID box; menerapkan profil dengan ID baru perlu menyamakan ID pemanggil.
