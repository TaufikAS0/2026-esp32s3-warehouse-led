# v0.7.1 — konfigurasi 100 box dapat disimpan di NVS

## Previous

v0.7.0 menyimpan konfigurasi aktif sebagai NVS string. JSON generator100box sekitar8,7KB
melampaui batas nvs_set_str4000byte termasuk terminator, sehingga Simpan/Terapkan profil
menghasilkan503 configuration write failed. Stub host sebelumnya tidak mereplikasi batas itu.

## Changes

Konfigurasi aktif schema1/JSON kini disimpan sebagai blob pada key config_blob,
tetap satu record untuk mapping/warna/output/brightness/Wi-Fi/revision.
Jika blob belum ada, boot membaca string config lama. Migrasi terjadi saat save berhasil;
record lama tidak dihapus. Blob rusak tidak membuat firmware kembali diam-diam ke mapping lama.
Stub host kini memeriksa batas4000byte dan membedakan tipe string/blob.

## Unchanged

API/schema JSON, GPIO4–7, profil1–5, credentials/password OTA, jaringan,
layout partisi, manual/program dan boot padam tetap. JARVIS tidak diubah.

## Tests

SettingsStore asli: legacy load, first-write failure menjaga legacy,100box default
dan field maksimum, save ulang, simulasi reboot, failed update menjaga record/revision,
OOM, blob corrupt tanpa fallback usang. Suite host/API/DOM dan build pinned dijalankan;
hasil aktual serta bukti perangkat dicatat di Test_Report.md.
OTA v0.7.1/build UTC2026-10-20T10:25:13Z berhasil. Uji ESP nyata: simpan ulang100box,
GET exact roundtrip, create/update/apply profil100box lulus; konfigurasi awal dipulihkan
dan profil tes dihapus. Tidak memulai LED on atau program tes.

## Limitations

Read compatibility hanya maju: v0.7.0 tidak membaca config_blob. Downgrade dapat
membaca mapping legacy usang; backup/restore eksplisit diperlukan sebelum downgrade.
NVS power-loss/fragmentasi/endurance dan hasil optik tidak dibuktikan tes host.

## Upgrade

OTA app-only, tanpa erase NVS/ubah partisi. Backup config/profil dahulu.
Setelah upgrade, halaman lama memerlukan refresh untuk menampilkan versi baru;
ekspor draft sebelum refresh. Revisi konfigurasi tetap mencegah overwrite data usang.
Identitas UTC/SHA256 mengikuti manifest build generated, bukan waktu upload.

## Publication

Checkpoint source v0.7.1 dipublikasikan atas instruksi pengguna ke branch
feature/warehouse-led-controller, tanpa merge develop/main atau tag produksi.
Mencakup fitur v0.7.0 dan perbaikan NVS v0.7.1. SHA commit aktual tersedia pada Git.
Credential, binary perangkat, generated header runtime dan backup API tetap di luar Git;
header fixture dalam tests hanya berisi nilai simulasi.
