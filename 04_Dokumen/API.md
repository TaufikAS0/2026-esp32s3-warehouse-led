# HTTP API v1

Base URL: `http://<IP-controller>/api/v1`. Semua request API memakai `Authorization: Bearer <token-dari-USB>`. JSON mutation memakai `Content-Type: application/json`. Body maksimal 32768 byte. Secret tidak berada di URL. Tidak ada endpoint untuk membaca token/password.

| Method | Path | Perilaku |
|---|---|---|
| GET | `/status` | Versi, koneksi, IP, jumlah LED/output, kondisi driver/storage, timer OTA, state tiap box |
| GET | `/boxes` | Schema, batas brightness, outputs, daftar ID/nama/mapping |
| PUT | `/boxes/{id}/state` | Perintah satu box, respons state efektif |
| POST | `/all-off` | Padamkan semua, respons `{"ok":true}` |
| GET | `/config` | Mapping + SSID dan password_configured, tanpa password |
| PUT | `/config` | Ganti penuh mapping/batas; opsi Wi-Fi; padamkan output hanya setelah write berhasil |
| POST | `/ota` | `{"enabled":true}` membuka 10 menit ketika STA, false menutup |

## State box

```json
{"on":true,"color":[0,255,0],"brightness":128,"mode":"steady","duration_ms":60000}
```

`on` wajib boolean. Default field lain: hijau, brightness128, steady, 60000 ms. Channel/brightness integer0..255, mode steady/blink, durasi1000..3600000 ms. Batas brightness global diterapkan; respons mengembalikan brightness efektif. Blink 500 ms nyala / 500 ms padam. Perintah baru mengganti seluruh state box dan memulai ulang timer. `{"on":false}` cukup untuk memadamkan. Field opsional yang dikirim tetap divalidasi walaupun on=false.

Respons:

```json
{"id":"R01-B01","on":true,"remaining_ms":60000,"brightness":128,"mode":"steady","color":[0,255,0]}
```

State `on` berarti timer aktif, termasuk fase blink padam. Tidak membuktikan LED fisik menyala. ID case-sensitive; JARVIS harus memilih ID dari `/boxes`, bukan membangun indeks pixel sendiri.

## Konfigurasi

```json
{
  "schema":1,
  "brightness_limit":128,
  "outputs":[3,0,0,0],
  "boxes":[{"id":"R01-B01","name":"Rak 1 Box 1","output":1,"start_led":1,"led_count":3}],
  "wifi":{"ssid":"NamaRouter","password":"<diisi-lokal>"}
}
```

`outputs` tepat empat integer0..75; 0 menonaktifkan mapping rak. boxes1..100; ID unik ASCII1..24 byte, name string<=48 byte, output1..4, start_led1..75, led_count1..75. Rentang harus terpasang dan tidak overlap. ID tidak wajib mengikuti Rxx-Bxx, tetapi generator memakai pola itu. Jumlah default tiga LED dapat diedit per box.

Wi-Fi opsional: jika tidak dikirim, simpan jaringan lama. Jika dikirim, ssid wajib string<=32 byte; password opsional hanya ketika SSID tetap sama. SSID baru membutuhkan password eksplisit, kosong untuk jaringan terbuka. SSID kosong memaksa AP-only dan menghapus password. Password non-kosong8..63 byte. GET mengembalikan password_configured, tidak mengembalikan password.

Satu operasi Simpan menulis keseluruhan mapping/jaringan. Gagal validasi/tulis tidak mengubah mapping aktif. Sukses memadamkan seluruh output. Mengubah Wi-Fi dapat memutus browser satu detik setelah respons.

## Error dan JARVIS

Error JSON `{"error":"pesan"}`: 400 input salah, 401 token salah/belum provision, 404 ID/path tidak ditemukan, 405 method salah, 409 OTA tanpa STA, 413 body terlalu besar, 415 content type salah, 503 alokasi/driver/storage gagal.

Client Python stdlib, tidak membutuhkan pip. Isi `WAREHOUSE_LED_URL` dan `WAREHOUSE_LED_TOKEN` secara lokal, lalu:

```powershell
python scripts/warehouse_client.py boxes
python scripts/warehouse_client.py on R01-B01 --seconds 60 --color 0 255 0 --blink
python scripts/warehouse_client.py off R01-B01
python scripts/warehouse_client.py all-off
python scripts/warehouse_client.py status
```

Di kode JARVIS, import WarehouseClient dan panggil set_box. Token dibaca dari environment/private credential store. Jangan menyimpan token dalam prompt, log, repo, atau URL. JARVIS mengelola relasi barang→ID box; jika barang ambigu, minta lokasi yang benar, jangan menyalakan tebakan.

Client memiliki timeout5 detik dan tidak mengikuti redirect. Kegagalan transport setelah write menghasilkan AmbiguousResult: hasil belum diketahui. Jangan retry otomatis; panggil status dan periksa box. Ini tidak memberikan exactly-once delivery.
