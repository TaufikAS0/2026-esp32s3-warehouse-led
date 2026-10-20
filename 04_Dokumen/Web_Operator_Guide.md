# Operator dashboard v0.7.0

Buka IP controller, tanpa login/token. Versi dan build UTC tampil pada header.
Status layar adalah perintah firmware, bukan konfirmasi nyala optik.

## Generator dan mapping

Di Konfigurasi pilih jumlah rak1–4, box/rak1–25 dan LED/box (default3).
Buat draft mapping menghasilkan A-01..A-25/output1, B/output2, C/output3, D/output4;
LED tiap rak dimulai1,4,7,..73; label "-" dan warna #4cd7f6.
Generator tidak langsung menyimpan/menyalakan. Edit draft lalu Simpan konfigurasi.
Simpan memadamkan seluruh LED/program; ID baru perlu disamakan dengan pemanggil JARVIS.
ID lama/custom tetap diterima, tidak diubah otomatis.

Jumlah LED rak tetap terpisah dari popup box. Simpan setting rak memakai mapping tersimpan,
menjaga draft mapping/jaringan lain. Mengurangi kapasitas di bawah mapping ditolak.
Klik kartu membuka popup mapping/warna dan tes manual. Simpan box mempertahankan box lain/jaringan.
Nama/warna tersimpan terlihat saat padam; hasil generator tetap bisa diedit.

## Lima profil ESP dan cadangan file

1. Siapkan draft atau konfigurasi yang ingin disimpan.
2. Isi nama, tekan Simpan draft sebagai profil baru. Ini tidak menerapkan config.
3. Pilih profil untuk preview: nama, jumlah box, LED/rak, brightness dan contoh ID.
4. Muat sebagai draft untuk mengedit dahulu, atau Terapkan profil terpilih.
5. Terapkan memadamkan seluruh output/program dan menyimpan mapping, tanpa mengganti jaringan.
6. Perbarui profil dari draft mengganti profil terpilih. Hapus tidak menghapus config aktif.

Profil berisi mapping/warna/jumlah LED/batas brightness, tanpa password/Wi-Fi/OTA/state nyala.
Maksimal5 profil pada ESP,100 box/profil; nama unik1–48 byte UTF-8, case-sensitive.
Ekspor JSON mengekspor profil terpilih, atau draft jika tidak ada profil terpilih.
Impor JSON membuka draft, tidak langsung menyimpan ke ESP atau menerapkan.
Simpan file cadangan sebanyak yang diperlukan di HP/laptop. Slot rusak dapat dihapus atau diperbarui.

Draft yang ada harus diekspor/disimpan atau dibuang sebelum memuat/impor/menerapkan profil lain.
Jika config berubah dari klien lain, draft dipertahankan dan Simpan ditolak sampai muat ulang.
Buang draft / muat ulang adalah tindakan eksplisit membuang draft dan menutup popup.
Muat ulang koneksi mempertahankan draft yang belum disimpan.

## Tes otomatis dan hardware

Semua tes/idle eksklusif satu rak, maksimum75 LED terpasang.
Mulai tes memadamkan manual/tes sebelumnya. Perintah manual berikutnya menghentikan tes.
Running/kedip mengikuti LED awal meningkat pada output1..4, bukan nama/urutan baris config.
Running satu box per langkah; kedip setengah langkah nyala dan setengah padam.
Pelangi berspektrum menurut posisi pixel fisik selama6×waktu langkah/rak.
Napas memakai warna config dan naik-turun brightness selama2×waktu langkah/rak.
Rak tanpa mapping dilewati; LED tanpa mapping hitam.

Status program menunjukkan rak aktif, rentang LED, posisi box dan durasi satu putaran.
Durasi tes adalah batas total; bila lebih pendek dari satu putaran, tidak semua rak akan dites.
Idle mengulang sampai dihentikan; tidak aktif lagi setelah reboot.
Preview rainbow popup memakai warna per pixel, kartu memakai warna pixel pertama.
Tes hardware rak mencakup seluruh LED terpasang termasuk tanpa mapping, default3detik.
Perpindahan rak memadamkan rak lama sebelum menyalakan rak baru.

Kontrol manual operasional tetap dapat menyalakan beberapa box/rak bersamaan,
durasi default60detik,1–3600detik, blink1detik, brightness dibatasi global.
Simpan/apply/all-off/aktivasi OTA/reboot menghentikan seluruh program.
Timer lokal tetap berjalan tanpa router.

## OTA dan jaringan

OTA tetap manual10menit, STA, password OTA perangkat, berbeda dari AP/router.
Transfer sinkron dapat membuat web sementara tidak merespons. Periksa versi/UTC setelah reboot.
Build lab v0.7.0 menegakkan profil HuaweiJIN/AP12345678 pada boot; secret OTA dipertahankan.
Source/build lokal tidak membuktikan perangkat sudah diperbarui.
