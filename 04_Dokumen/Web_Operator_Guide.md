# Dashboard dan tes manual

Dashboard memakai warna slate/cyan dan empat panel rak dengan25 posisi per rak. Klik box membuka popup konfigurasi/kontrol; tutup memakai tombol Tutup, Escape atau area gelap. Warna konfigurasi tersimpan ditampilkan sebagai titik warna dan kode hex pada kartu, termasuk saat padam. Layout5×5 adalah urutan tampilan, bukan konfirmasi susunan fisik rak. Pada ponsel, gunakan filter untuk berpindah rak atau melihat semuanya.

## Masuk

1. Hubungkan controller ke router melalui Konfigurasi. SSID/password tersimpan di NVS dan dipakai otomatis setelah boot. Jangan memasukkan kredensial router ke source.
2. Buka alamat IP controller. IP dapat dibaca melalui USB115200 dengan perintah `info`.
3. Mapping dimuat otomatis tanpa login/token. Versi firmware terlihat pada header bahkan sebelum koneksi API selesai. Muat ulang koneksi mengulangi pembacaan controller.
4. Koneksi putus menandai status sebagai data terakhir dan menonaktifkan perintah nyala sampai status berhasil dibaca lagi.

## Tes hardware per rak

Di bawah grid setiap rak, isi brightness tes (default32), pilih warna custom atau tekan Merah/Hijau/Biru/Putih/Kedip. Seluruh LED1 sampai jumlah LED terpasang pada rak tersebut menyala selama3 detik, termasuk LED yang belum dipetakan ke box. Rak dengan jumlah LED0 menonaktifkan tes; isi jumlah terpasang di Konfigurasi lalu Simpan dahulu. Jumlah ini adalah pengaturan manual, bukan hasil autodetection.

Tes menghentikan box aktif pada rak yang diuji; rak lain tetap berjalan. Padam pada panel rak, Padamkan semua, simpan konfigurasi dan aktivasi OTA menghentikan tes. Nyala layar/status API menunjukkan perintah firmware, bukan konfirmasi optik. Tes tidak menulis flash.

## Tes awal tiga LED

Atur jumlah LED rak terlebih dahulu di **Konfigurasi → Output hardware → Simpan setting rak**. Ini memakai mapping tersimpan dan tidak mengubah mapping box. Mengurangi jumlah LED di bawah rentang box yang sudah tersimpan ditolak. Draft mapping/jaringan lain tidak ikut disimpan.

Semua posisi box dapat diklik, termasuk slot abu-abu. Popup menyediakan ID, nama, output/rak, LED awal, jumlah LED box dan warna konfigurasi. Jumlah LED rak tampil sebagai informasi; tersedia tombol menuju setting rak. **Simpan box** hanya mengubah box terpilih, memadamkan seluruh LED/program dan mempertahankan box lain serta Wi-Fi. Mapping baru harus disimpan sebelum tes. Jika ada draft di halaman Konfigurasi, simpan atau buang draft tersebut dahulu. Contoh B02 memakai LED4–6 sehingga jumlah LED Rak1 minimal6.

Untuk box yang sudah tersimpan, klik box lalu gunakan Tes warna cepat atau Nyalakan LED box. Perintah tes memakai mapping tersimpan; perubahan pada field editor baru diterapkan setelah Simpan box. Slot dengan ID Rxx-Bxx mengikuti nomor B pada rak tersebut; ID custom menempati posisi kosong menurut urutan mapping.

1. Buka Konfigurasi → Setup1 box /3 LED → Simpan konfigurasi. Generator hanya membuat draft; Simpan memadamkan seluruh box dan menerapkan mapping.
2. Buka Peta rak & box → pilih B01 pada Rak1. Lokasi harus menunjukkan GPIO4, LED1–3.
3. Tekan Merah, Hijau, atau Biru pada bagian Tes warna cepat. Setiap tes berlangsung3 detik, memakai brightness yang dipilih serta batas global.
4. Tes kedip3 detik memakai periode1 detik. Untuk nyala lebih lama, pilih warna, brightness, mode dan durasi, lalu Nyalakan LED box. Padamkan box atau Padamkan semua menghentikan perintah.
5. Untuk menguji alamat secara terpisah, gunakan draft Tes tiap LED:3 box /1 LED, lalu Simpan. B01/B02/B03 menunjuk LED1/2/3. Setelah selesai, kembalikan Setup1 box /3 LED dan Simpan.

Box aktif ditandai warna, label status dan sisa timer. Box terpilih memiliki outline cyan. Slot belum dikonfigurasi tidak dapat dinyalakan. Jumlah box terkonfigurasi berbeda dari kapasitas100 slot. API memberi status perintah firmware; LED fisik tetap harus diamati langsung.

## Program otomatis dan idle

Panel di atas peta menyediakan Running box, Kedip bergilir, Pelangi mengalir dan Napas warna. Running/kedip mengikuti urutan mapping tersimpan; semua LED dalam box menyala bersama memakai warna konfigurasi box. Pelangi/napas bekerja pada semua box yang terpetakan. LED yang belum bermapping tetap padam; gunakan tes hardware rak untuk LED tersebut.

Brightness default32 dibatasi batas global. Waktu langkah100–5000 ms mengatur kecepatan running/kedip dan setengah siklus napas. Pelangi bergeser tiap30 ms. Tes default30 detik dapat diatur1–3600 detik. **Jalankan sebagai idle** berjalan terus sampai dihentikan; tidak otomatis hidup setelah reboot. Status menunjukkan TES/IDLE dan pola aktif; kartu yang diterangi program diberi label PROGRAM.

Perintah manual/JARVIS yang masih aktif mendapat prioritas pada box yang sama, termasuk fase gelap blink; setelah timer habis, box mengikuti program lagi. Tes hardware rak menimpa animasi pada output itu. Hentikan program hanya mematikan animasi; perintah manual tetap berjalan. Padamkan semua, simpan konfigurasi, reboot dan aktivasi OTA menghentikan seluruh program/LED. Animasi berjalan lokal tanpa delay dan tidak menulis flash.

## Menambah rak

Isi jumlah rak, box per rak, dan LED per box pada generator. Contoh4 rak ×25 box ×3 LED menghasilkan100 box dan75 LED per output. Gunakan hanya sesuai jumlah LED yang benar-benar terpasang. Editor mendukung ID/nama/output/LED awal/jumlah; ID duplikat, overlap, dan rentang di luar output ditolak sebelum request, lalu divalidasi kembali oleh firmware.

Perubahan draft tidak menyalakan LED atau menulis flash. Muat ulang membuang draft. Request nyala tidak menulis NVS. Timer tetap berjalan ketika Wi-Fi putus. Setelah reboot seluruh box padam.

## OTA berikutnya

Controller harus terhubung ke router yang sama dengan komputer. Di Konfigurasi → OTA, aktifkan jendela10 menit. Aktivasi memadamkan LED. Upload melalui port jaringan Arduino IDE atau espota bawaan core3.3.10 dengan password OTA perangkat, bukan password AP/router/API.

Saat transfer berlangsung, web bisa sementara tidak merespons karena ArduinoOTA menangani upload secara sinkron. Setelah sukses, perangkat reboot, menyambung lagi ke router, mempertahankan mapping dan menutup OTA. Aktifkan lagi untuk upload berikutnya. Jika transfer gagal, periksa versi/status sebelum mengulang; jangan menganggap timeout berarti flash berhasil.

Pada komputer dengan beberapa adapter satu subnet, uploader ESP32 menggunakan IP sumber undangan UDP untuk koneksi balik TCP. Listener host `0.0.0.0` menghindari pengikatan ke adapter yang berbeda. Tentukan port host dan izinkan koneksi balik hanya jika dibutuhkan oleh firewall jaringan; jangan mematikan firewall secara umum. Catat hasil transfer nyata, bukan hanya aktivasi OTA.
