# Firmware v0.7.0

## Previous

v0.6.1, baseline source3eccf67 dan handoff347751a pada feature/warehouse-led-controller.
Perubahan saat ini lokal, belum commit/push/merge/upload.

## Changes

- Generator A–D,25box/rak, label "-", cyan#4cd7f6; legacy/custom ID tetap.
- Lima profil NVS berversi/checksum, preview/draft/apply, JSON export/import tanpa secret.
- Running/blink menurut LED fisik; rainbow per pixel; breathe warna config.
- Semua tes/idle eksklusif satu rak/75LED; driver clear-before-light termasuk4->1.
- Expiry dilatch mati; idle menggunakan akumulasi waktu64bit.
- Config revision persisted dan konflik antarklien dengan draft preservation.
- Build UTC/nonce generated, gate binary/partisi dan manifest SHA256.
- Default build UART/CH340; native USB CDC eksplisit.
- Migrasi lab dari sumber privat, hanya saat berbeda, menjaga mapping/warna/secret OTA.

## Unchanged

Satu S3, GPIO4–7, GRB800kHz, constructor NeoPixel berparameter; schema1 legacy,
key config/credentials dan layout partisi; web/API LAN terbuka; OTA password/manual10menit;
boot padam; source JARVIS tidak diubah. Manual normal tetap dapat beberapa box sekaligus.

## Tests

Lihat Test_Report.md untuk hasil aktual build UTC, ukuran dan suite.
Host storage/driver/DOM adalah simulasi batas hardware, bukan bukti LED fisik.

## Limitations

Lima profil;100box/300LED kapasitas config; tes hanya satu rak/75LED.
Durasi tes global dapat berhenti sebelum selesai satu putaran.
CRC32 mendeteksi kerusakan, bukan autentikasi. Atomicity/power-loss/fragmentasi NVS
dan hasil optik semua pola pada perangkat nyata belum diverifikasi dalam tugas ini.
OTA app-only kemudian dilakukan atas instruksi pengguna pada2026-10-20T10:07:55Z;
v0.7.0/build UTC2026-10-20T09:17:45Z terverifikasi di192.168.1.28.
Mapping/warna35box, outputs dan Wi-Fi terjaga, OTA tertutup dan state padam.
Dashboard cocok source. Ini bukti transfer/runtime/API, bukan bukti LED optik.
Belum commit/push/merge; rincian bukti di Test_Report.md.

## Upgrade

Build melalui wrapper dengan sumber lab privat. Sebelum upload yang diminta terpisah,
backup config, cocokkan profil UART/partisi dan baca kondisi perangkat.
NVS config lama tetap terbaca; ID tidak dimigrasikan menjadi A–D otomatis.
Jaringan berbeda dimigrasikan saat boot ke HuaweiJIN/AP default, bukan secret OTA.
PUT config baru memerlukan revision dari GET terakhir (428 bila hilang,409 bila usang).
Manual membatalkan tes, menggantikan prioritas manual-dan-animasi versi lama.

## Rules read

Delapan file: Firmware_AI_Vault_Home, Rules_Nama_dan_Folder_Workspace,
Rules_Firmware_Standar, Rules_GitHub_Gitflow, Rules_Kredensial_WiFi,
WiFi_HuaweiJIN, Rules_Versioning, Rules_Change_Notifications.
Profil STA HuaweiJIN; AP default bersama12345678. Credential privat tidak dicatat di sini.
