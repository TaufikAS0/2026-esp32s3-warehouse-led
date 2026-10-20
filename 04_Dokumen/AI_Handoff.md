# Handoff v0.7.1 — perbaikan penyimpanan NVS

Source terbaru v0.7.1 memperbaiki save100box yang melampaui batas string NVS4000byte.
Konfigurasi aktif disimpan JSON/schema1 sebagai config_blob; string config lama tetap
dibaca bila blob belum ada. Blob rusak tidak fallback ke mapping legacy usang.
Jangan downgrade tanpa backup/restore eksplisit. Baca Release_v0.7.1.md dan bagian
terbaru Test_Report.md untuk hasil build/upload/uji nyata. OTA v0.7.1 di192.168.1.28
terverifikasi2026-10-20T10:27:29Z, build UTC2026-10-20T10:25:13Z.
Simpan ulang/apply profil100box pada ESP berhasil, lalu konfigurasi awal1box dipulihkan,
revision25, profil sementara dihapus, seluruh state padam. Reboot fisik setelah100box
save belum diuji. Checkpoint ini dipublikasikan atas instruksi pengguna pada branch
feature/warehouse-led-controller; identitas commit aktual dilihat melalui Git.
Tidak ada merge ke develop/main. Riwayat v0.7.0 berikut tetap historis.

## Baseline dan batas tugas

Repository TaufikAS0/2026-esp32s3-warehouse-led, branch feature/warehouse-led-controller.
Base Git347751a, firmware sebelumnya v0.6.1/source3eccf67. Source lokal kini v0.7.0;
belum commit/push/merge. OTA atas instruksi pengguna berhasil pada2026-10-20T10:07:55Z;
controller Warehouse-9FC114 di192.168.1.28 melaporkan v0.7.0/build UTC2026-10-20T09:17:45Z.
Mapping/warna35box, outputs75/75/75/75 dan Wi-Fi terjaga; driver ready, OTA tertutup,
seluruh state padam, dashboard cocok source. Bukti di Test_Report.md dan _sandbox/ota-v070.
PR#1 menuju develop tidak diubah oleh tugas ini.
Jangan memakai develop sebagai baseline fitur. Jangan mengubah JARVIS atau mengakses perangkat
tanpa instruksi lanjutan. IP/mapping diperiksa saat OTA tersebut; COM tidak diakses.

Mulai START_HERE.md, AGENTS.md, README.md, dokumen ini, Release_v0.7.0.md, Architecture,
API, Build_Setup, Web_Operator_Guide, Test_Report dan pin map.
THREAD_CONVERSATION.md berisi145pesan historis; instruksi terbaru pada release ini
menggantikan prioritas manual lama, urutan config dan generator Rxx-Bxx.

## Keputusan terbaru

- Satu S3, empat output WS2812B GPIO4–7;75LED/output,100box/300LED konfigurasi maksimum.
- Generator A-01..A-25/output1, B/output2, C/output3, D/output4; label "-", cyan#4cd7f6.
- Config lama/custom ID tidak diganti otomatis.
- Lima profil ESP + JSON cadangan, tanpa jaringan/secret/runtime. Pilih untuk preview,
  muat draft atau terapkan eksplisit. Apply memadamkan semua program/LED.
- Semua tes/idle eksklusif satu rak bergantian. Urutan fisik output/start_led,
  bukan nama atau urutan config. Perpindahan memadamkan rak lama terlebih dahulu.
- Rainbow per posisi pixel; breathe warna config. Manual berikutnya membatalkan tes;
  manual operasional biasa tetap bisa beberapa box sekaligus.
- HTTP/web LAN terbuka. OTA tetap password perangkat/manual10menit/STA-only.
- Popup box dan setting jumlah LED rak terpisah, warna config tetap terlihat saat padam.
- Constructor NeoPixel berparameter75/pin/GRB800kHz wajib dipertahankan.
- Config revision menolak write usang; draft tidak boleh hilang karena polling.
- Metadata UTC/nonce, gate binary/partisi, manifest SHA256 wajib sebelum build diterima.

## NVS dan build

Key config_blob schema1 + wifi + revision (legacy string config read-only), credentials terpisah. Profil profile1..profile5
format WLP1 CRC32 panjang eksplisit, maksimal8KiB/record. Tidak mengubah partisi:
dua slot1280KiB, NVS128KiB@0x290000. Jangan erase/ubah layout untuk upgrade OTA.
Build pinned core3.3.10/ArduinoJson7.4.2/NeoPixel1.15.2.
Default UART/CH340 CDCOnBoot=default; native USB CDC memakai switch eksplisit.
Sumber lab privat menghasilkan header ignored. Boot membandingkan HuaweiJIN/AP12345678,
menulis hanya perbedaan, menjaga mapping/warna/secret OTA. Config rusak tidak ditimpa.

## Aturan vault — baca berurutan

Vault terpisah: https://github.com/TaufikAS0/2026-vault-firmware-rules

1. 00_Start_Here/Firmware_AI_Vault_Home.md
2. 01_Rules/Rules_Nama_dan_Folder_Workspace.md
3. 01_Rules/Rules_Firmware_Standar.md
4. 01_Rules/Rules_GitHub_Gitflow.md
5. 01_Rules/Rules_Kredensial_WiFi.md
6. 04_Profiles/WiFi/WiFi_HuaweiJIN.md
7. 01_Rules/Rules_Versioning.md
8. 01_Rules/Rules_Change_Notifications.md

Jangan cetak/commit credential. Timestamp build bukan uptime/jam perangkat/upload.
Laporan perubahan wajib versi sebelum/sesudah, UTC, API/pin/NVS/jaringan, tes dan status upload.

## Batas bukti

Lihat Test_Report.md. Bukti historis hanya pengguna mengonfirmasi3LED GPIO4 pada v0.5.1,
dan tes software/API/OTA v0.6.1. Runtime v0.7.0 kini terverifikasi melalui OTA/identitas/API,
tanpa pengamatan optik LED.
75LED, kelistrikan, semua pola optik, router recovery penuh dan endurance memerlukan tes nyata.
Tes host v0.7.0 dilakukan lokal; OTA nyata terverifikasi terpisah, tidak memulai tes LED.
