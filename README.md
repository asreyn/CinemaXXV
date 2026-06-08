# 🎬 CinemaXXV
### *Movie Ticket Reservation System*

**EST. 2026** — *Where Every Seat Tells a Story*

---

![C++](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Status](https://img.shields.io/badge/Status-Active-gold?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0.0-blue?style=for-the-badge)
![Team](https://img.shields.io/badge/Team-Kelompok%2020-blueviolet?style=for-the-badge)

> *"Nikmati pengalaman bioskop terbaik dari ujung jarimu — reservasi tiket, pilih kursi, pesan snack, semua dalam satu sistem."*

</div>

---

## 📌 Daftar Isi

- [Tentang Project](#-tentang-project)
- [Fitur Unggulan](#-fitur-unggulan)
- [Alur Sistem](#-alur-sistem)
- [Struktur Menu](#-struktur-menu)
- [Cara Menjalankan](#-cara-menjalankan)
- [Tim Pengembang](#-tim-pengembang)

---

## 🎥 Tentang Project

**CinemaXXV** adalah sistem reservasi tiket bioskop berbasis konsol yang dibangun menggunakan **C++**. Project ini dikembangkan sebagai bagian dari tugas akhir oleh **Kelompok 20**, dirancang untuk mensimulasikan pengalaman pemesanan tiket bioskop yang lengkap dan realistis.

Dari login, memilih film, memilih kursi, memesan makanan, hingga mencetak tiket — semua tersedia dalam satu sistem yang terintegrasi. 🍿

---

## ✨ Fitur Unggulan

| Fitur | Deskripsi |
|-------|-----------|
| 🔐 **Autentikasi Pengguna** | Sistem login & registrasi dengan validasi panjang karakter |
| 🎬 **Manajemen Film** | Admin dapat menambah, mengedit, dan menghapus data film |
| 🗓️ **Penjadwalan Tayang** | Atur studio, tanggal, dan jam tayang dengan fleksibel |
| 💺 **Pemilihan Kursi** | Layout kursi interaktif dengan validasi kursi terisi |
| 🍔 **Pemesanan Makanan** | Menu makanan, minuman, dan paket combo tersedia |
| 🏷️ **Kode Promo** | Diskon otomatis saat input kode promo yang valid |
| 💳 **Pembayaran Virtual Account** | Simulasi pembayaran cashless yang realistis |
| 🎟️ **Cetak Tiket** | Generate kode tiket unik dengan detail lengkap |
| 📊 **Dashboard Admin** | Statistik real-time: pendapatan, film terlaris, dan lainnya |
| 📜 **Riwayat Transaksi** | Lihat semua riwayat pemesanan beserta nomor Virtual Account |

---

## 🔄 Alur Sistem

```
[ MULAI ]
    │
    ▼
┌─────────────┐
│    LOGIN    │
└──────┬──────┘
       │
   ┌───┴───┐
   │       │
   ▼       ▼
[ADMIN] [CUSTOMER]
   │       │
   │       ├─► Lihat & Cari Film
   │       ├─► Reservasi Tiket
   │       ├─► Pilih Kursi
   │       ├─► Pesan Makanan
   │       ├─► Pembayaran
   │       └─► Cetak Tiket 🎟️
   │
   ├─► Manajemen Film
   ├─► Manajemen Jadwal
   ├─► Manajemen Promo
   └─► Laporan Penjualan 📊
```

---

## 🗂️ Struktur Menu

<details>
<summary>🔐 <b>LOGIN</b> — Klik untuk expand</summary>

```
LOGIN
│
├── SIGN UP
│   ├── Input Username  (dibatasi karakter)
│   └── Input Password  (dibatasi karakter)
│
├── SIGN IN
│   ├── Input Username
│   └── Input Password
│
└── EXIT
```
</details>

---

<details>
<summary>🛡️ <b>ADMIN</b> — Klik untuk expand</summary>

```
ADMIN
│
├── 📊 Dashboard Admin
│   ├── Total Film
│   ├── Total Customer
│   ├── Total Tiket Terjual
│   ├── Total Transaksi
│   ├── Total Pendapatan
│   └── Film Terlaris
│
├── 🎬 Manajemen Film
│   ├── Tambah Film (ID, Judul, Genre, Durasi, Rating, Harga, Stok)
│   ├── Edit Film
│   ├── Hapus Film
│   └── Lihat Daftar Film
│
├── 🗓️ Manajemen Jadwal
│   ├── Tambah Jadwal (Film, Studio, Tanggal, Jam)
│   ├── Edit Jadwal
│   ├── Hapus Jadwal
│   └── Lihat Jadwal
│
├── 🏷️ Manajemen Promo
│   ├── Tambah Promo (Kode, Diskon, Status)
│   ├── Edit Promo
│   ├── Hapus Promo
│   └── Lihat Promo
│
├── 📈 Laporan Penjualan
│   ├── Total Tiket Terjual
│   ├── Total Pendapatan
│   ├── Total Transaksi
│   ├── Data Transaksi
│   └── Film Terlaris
│
└── 🚪 Logout
```
</details>

---

<details>
<summary>👤 <b>CUSTOMER</b> — Klik untuk expand</summary>

```
CUSTOMER
│
├── 🏠 Beranda
│   ├── Lihat Daftar Film
│   ├── Cari Film (Judul / Genre)
│   ├── Urutkan Film (A-Z, Z-A, Termurah, Termahal)
│   └── Lihat Detail Film (Judul, Genre, Durasi, Rating, Harga, Sisa Tiket)
│
├── 🎟️ Reservasi Tiket
│   ├── Pilih Film
│   ├── Pilih Tanggal Jadwal
│   ├── Pilih Studio
│   ├── Pilih Jumlah Tiket
│   ├── Lihat Layout Kursi
│   ├── Pilih Kursi
│   └── Validasi Kursi Terisi
│
├── 🍿 Pemesanan Makanan
│   ├── Lihat Menu
│   ├── Pilih Makanan
│   ├── Pilih Minuman
│   ├── Pilih Paket Combo
│   └── Hitung Total Pesanan
│
├── 💳 Pembayaran
│   ├── Ringkasan Pesanan
│   ├── Input Kode Promo
│   ├── Hitung Diskon
│   ├── Generate Virtual Account
│   ├── Simulasi Pembayaran Cashless
│   └── Konfirmasi Pembayaran
│
├── 🖨️ Cetak Tiket
│   ├── Generate Kode Tiket
│   ├── Tampilkan Detail Tiket
│   └── Simpan Tiket
│
├── 📜 Riwayat Pemesanan
│   ├── Riwayat Tiket
│   ├── Riwayat Transaksi
│   └── Detail Pemesanan (+ Nomor Virtual Account)
│
└── 🚪 Logout
```
</details>

---

## 🚀 Cara Menjalankan

### Prasyarat
Pastikan kamu sudah menginstall compiler C++ di sistemmu:
- **GCC / G++** (Linux/Mac)
- **MinGW** (Windows)
- **Visual Studio** (Windows)

### Langkah Kompilasi & Eksekusi

```bash
# 1. Clone repository ini
git clone https://github.com/kelompok20/CinemaXXV.git

# 2. Masuk ke direktori project
cd CinemaXXV

# 3. Kompilasi program
g++ -o CinemaXXV main.cpp

# 4. Jalankan program
./CinemaXXV          # Linux / Mac
CinemaXXV.exe        # Windows
```

### Akun Default

| Role | Username | Password |
|------|----------|----------|
| 👑 Admin | `admin` | `admin123` |
| 👤 Customer | Daftar sendiri via Sign Up | — |

---

## 👥 Tim Pengembang

<div align="center">

### 🏆 Kelompok 20

*Dengan bangga mempersembahkan CinemaXXV — EST. 2026*

</div>

---

<div align="center">

### 🌟 Struktur Konsep C++ yang Digunakan

| Konsep | Implementasi |
|--------|-------------|
| `struct / class` | Data Film, Jadwal, Transaksi, User |
| `array / vector` | Menyimpan daftar film, kursi, menu |
| `string` | Input nama, kode promo, judul film |
| `function` | Modularisasi setiap fitur menu |
| `loop & kondisi` | Validasi input, navigasi menu |
| `file I/O` | Penyimpanan data transaksi & tiket |

</div>

---

<div align="center">

🎬 **CinemaXXV** — *Movie Ticket Reservation System*

**EST. 2026** | Built with ❤️ by **Kelompok 20**

*"Lights, Camera, Code!"* 🎥

---

⭐ Jangan lupa kasih **Star** kalau project ini bermanfaat!

</div>
