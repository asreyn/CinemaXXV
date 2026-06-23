#include <iostream>
#include <iomanip>
#include <conio.h>
using namespace std;

const string RESET         = "\033[0m";
const string KUNING        = "\033[33m";
const string KUNING_TERANG = "\033[93m";
const string PUTIH_TERANG  = "\033[97m";
const string MERAH         = "\033[31m";
const string HIJAU_TERANG  = "\033[92m";
const string BIRU          = "\033[94m";
const string COKLAT        = "\033[33m";
const string UNGU          = "\033[35m";
const string MERAH_JAMBU   = "\033[95m";
const string TOSCA         = "\033[36m";
const string HITAM         = "\033[30m";

const int MAX_USER    = 30;
const int MAX_FILM    = 20;
const int MAX_JADWAL  = 30;
const int MAX_PROMO   = 10;
const int MAX_TRX     = 50;
const int TOTAL_KURSI = 40;
const int MAX_STUDIO  = 3;

struct User {
    string username;
    string password;
};

struct Film {
    int    id, durasi, stok, terjual;
    string judul, genre, rating;
    long long harga;
};

struct Jadwal {
    int    id;
    string tanggal, jam, jamSelesai;
    int    film, studio, mulai, selesai;
    bool   kursi[TOTAL_KURSI];
};

struct Promo {
    string kode;
    int    diskon;
    bool   aktif;
};

struct Transaksi {
    string id, kodeTiket;
    int    user, jadwal, jumlahTiket;
    int    kursi[TOTAL_KURSI];
    long long total, diskon;
};

User      user[MAX_USER];
Film      film[MAX_FILM];
Jadwal    jadwal[MAX_JADWAL];
Promo     promo[MAX_PROMO];
Transaksi transaksi[MAX_TRX];

int jumlahUser = 0, jumlahFilm = 0, jumlahJadwal = 0, jumlahPromo = 0, jumlahTransaksi = 0;
int indeksUserAktif = -1;

string adminUsn = "admin";
string adminPsw = "admin123";

int indeksFilmSementara  = -1;
int indeksJadwalSementara = -1; // menandakan data tidak ditemukan
int jumlahTiketSementara = 0;
int kursiSementara[TOTAL_KURSI];

// Fungsi sederhana untuk mengembalikan semua data reservasi sementara
// ke kondisi awal (kosong). Dipakai saat pembayaran dibatalkan, transaksi
// penuh, pembayaran selesai, maupun saat customer logout, agar tidak ada
// kode reset yang ditulis berulang-ulang di banyak tempat.
void resetReservasiSementara() {
    indeksFilmSementara   = -1;
    indeksJadwalSementara = -1;
    jumlahTiketSementara  = 0;
    for (int i = 0; i < TOTAL_KURSI; i++)
        kursiSementara[i] = 0;
}

// ===================== ID OTOMATIS (AUTO INCREMENT) =====================
// Counter sederhana untuk ID Film dan ID Jadwal.
// ID Film dimulai dari 1001, ID Jadwal dimulai dari 2001.
int idFilmBerikutnya   = 1001;
int idJadwalBerikutnya = 2001;

// ===================== UTILITAS =====================

void garis() {
    for (int i = 0; i < 60; i++) cout << KUNING_TERANG << "=" << RESET;
    cout << endl;
}

void header(string judul) {
    system("cls");
    garis();
    string baris[6] = {
        R"(*  _____ _                           __   ____   ___   _   *)",
        R"(* /  __ (_)                          \ \ / /\ \ / / | | |  *)",
        R"(* | /  \/_ _ __   ___ _ __ ___   __ _ \ V /  \ V /| | | |  *)",
        R"(* | |   | | '_ \ / _ \ '_ ` _ \ / _` |/   \  /   \| | | |  *)",
        R"(* \__/\ | | | |  __/ | | | | | (_| / /^\ \/ /^\ \ \_  / /  *)",
        R"(*  \____/_|_| |_|\___|_| |_| |_|\__,_\/   \/\/   \/\___/   *)"};
    cout << KUNING        << baris[0] << RESET << endl;
    cout << KUNING        << baris[1] << RESET << endl;
    cout << KUNING_TERANG << baris[2] << RESET << endl;
    cout << KUNING_TERANG << baris[3] << RESET << endl;
    cout << KUNING  << baris[4] << RESET << endl;
    cout << KUNING  << baris[5] << RESET << endl;
    cout << "\t\t  " << KUNING_TERANG << judul << RESET << endl;
    garis();
}

void pause() {
    cout << "\nTekan sembarang tombol...";
    getch();
}

string toLower(string s) {
    for (int i = 0; i < (int)s.length(); i++)
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
    return s;
}

// misal nk=0 (A1)
string namaKursi(int nk) {
    return string(1, char('A' + nk / 8)) + to_string(nk % 8 + 1);
} // mengubah nama kursi (A1, DLL)


// ===================== TAMPILAN QR CODE (QRIS) =====================
// Fungsi ini HANYA bertugas menampilkan QR Code dummy (visual aja)
void tampilQRCode() {
    cout << "##########  ####    ##########" << endl;
    cout << "##      ##    ##    ##      ##" << endl;
    cout << "##  ##  ##  ##      ##  ##  ##" << endl;
    cout << "##      ##    ##    ##      ##" << endl;
    cout << "##########  ##      ##########" << endl;
    cout << "              ##              " << endl;
    cout << "  ####    ####  ######  ##  ##" << endl;
    cout << "####  ##  ######  ####    ####" << endl;
    cout << "                ##    ####    " << endl;
    cout << "            ####  ######  ##  " << endl;
    cout << "##########  ######  ####    ##" << endl;
    cout << "##      ##  ##    ##    ####  " << endl;
    cout << "##  ##  ##    ####  ######  ##" << endl;
    cout << "##      ##    ######  ####    " << endl;
    cout << "##########  ####    ##    ####" << endl;
}

int inputAngka(string prompt, int batasBawah, int batasAtas, string pesanError = "") {
    string input;
    int nilai;
    bool valid;
    do { // program akan tetap meminta ulang input, selama belum valid
        valid = true;
        cout << prompt;
        getline(cin >> ws, input);

        if (toLower(input) == "exit") return -1;

        if (input.find(' ') != string::npos) {
            cout << MERAH << "Input tidak valid, jangan menggunakan spasi. Masukkan satu angka saja.\n" << RESET;
            valid = false; continue;
        }

        bool angkaValid = !input.empty(); //kosong --> false, ada isi --> true
        for (int i = 0; i < (int)input.length(); i++)
            if (!isdigit(input[i])) { angkaValid = false; break; } //memastikan input berupa angka

        if (!angkaValid) {
            cout << MERAH << "Input tidak valid, masukkan angka.\n" << RESET;
            valid = false; continue;
        }
        if (input.length() > 9) {
            cout << MERAH << "Angka terlalu besar.\n" << RESET;
            valid = false; continue;
        }
        nilai = stoi(input);
        if (nilai < batasBawah || nilai > batasAtas) {
            if (pesanError != "")
                cout << MERAH << pesanError << "\n" << RESET;
            else
                cout << MERAH << "Input harus antara " << batasBawah << " - " << batasAtas << ".\n" << RESET;
            valid = false;
        }
    } while (!valid);
    return nilai;
}

// Meminta input angka khusus untuk ID (tanpa batas atas tertentu).
// Mengembalikan nilai ID yang dimasukkan, atau -1 jika ketik 'exit'.
int inputID(string prompt) {
    string input;
    int nilai;
    while (true) {
        cout << prompt;
        getline(cin >> ws, input);
        if (toLower(input) == "exit") return -1;

        if (input.find(' ') != string::npos) {
            cout << MERAH << "ID tidak boleh mengandung spasi, masukkan satu angka saja.\n" << RESET;
            continue;
        }

        bool angkaValid = !input.empty();
        for (char c : input) if (!isdigit(c)) { angkaValid = false; break; }
        if (!angkaValid) { cout << MERAH << "ID harus berupa angka bulat.\n" << RESET; continue; }
        if (input.length() > 9) { cout << MERAH << "Angka terlalu besar.\n" << RESET; continue; }
        nilai = stoi(input);
        break;
    }
    return nilai;
}

// ===================== MENU INTERAKTIF =====================

int pilihMenu(string judul, string opsi[], int jumlahOpsi) {
    const int LEBAR_KONSOL = 60;
    int panjangMaks = 0;
    for (int i = 0; i < jumlahOpsi; i++)
        if ((int)opsi[i].length() > panjangMaks)
            panjangMaks = opsi[i].length();  //mengecek panjang huruf agar dapat sejajar

    int lebarBlok = 4 + panjangMaks; // menghitung posisi tengah
    int padKiri   = (LEBAR_KONSOL - lebarBlok) / 2;
    if (padKiri < 0) padKiri = 0;
    string spasi = string(padKiri, ' '); //menghasilkan spasi

    int pilihan = 0, tombolKeyboard;
    do {
        header(judul);
        cout << "Gunakan panah Atas/Bawah, Enter untuk pilih\n";
        garis();
        for (int i = 0; i < jumlahOpsi; i++) {
            if (i == pilihan)
                cout << spasi << UNGU << " >> " << opsi[i] << RESET << endl;
            else
                cout << spasi << "    " << opsi[i] << endl;
        }
        garis();

        tombolKeyboard = getch();
        if (tombolKeyboard == 224) {
            tombolKeyboard = getch();
            if (tombolKeyboard == 72)      pilihan = (pilihan - 1 + jumlahOpsi) % jumlahOpsi; //panah atas
            else if (tombolKeyboard == 80) pilihan = (pilihan + 1) % jumlahOpsi; //panah bawah
        }
    } while (tombolKeyboard != 13);
    return pilihan;
}

// ===================== SEARCHING =====================

int cariUser(string usn) {
    for (int i = 0; i < jumlahUser; i++)
        if (toLower(user[i].username) == toLower(usn)) return i;
    return -1; //mengecek usn yang sudah ada yagesya
}

int cariFilm(int id) {
    for (int i = 0; i < jumlahFilm; i++)
        if (film[i].id == id) return i;
    return -1;
}

int cariJadwal(int id) {
    for (int i = 0; i < jumlahJadwal; i++)
        if (jadwal[i].id == id) return i;
    return -1;
}

int cariPromo(string kode) {
    for (int i = 0; i < jumlahPromo; i++)
        if (toLower(promo[i].kode) == toLower(kode)) return i;
    return -1;
}

// ===================== CEK BENTROK JADWAL =====================

bool cekBentrok(int studio, string tanggal, int waktuMulaiBaru, int waktuSelesaiBaru, int kecuali) {
    for (int i = 0; i < jumlahJadwal; i++) {
        if (i == kecuali) continue;
        if (jadwal[i].studio == studio && jadwal[i].tanggal == tanggal)
            if (waktuMulaiBaru < jadwal[i].selesai && jadwal[i].mulai < waktuSelesaiBaru)
                return true; // mulai baru di cek dengan selesai lama
    }
    return false;
}

// ===================== POINTER =====================

void hitungTotal(long long *totalPendapatan, int *totalTiket) {
    *totalPendapatan = 0; *totalTiket = 0;
    for (int i = 0; i < jumlahTransaksi; i++) {
        *totalPendapatan += transaksi[i].total;
        *totalTiket      += transaksi[i].jumlahTiket;
    }
} // karena fungsi ini menghasilkan 2 nilai makanya memakai pointer daripada return

// ===================== UTILITAS JAM =====================

int jamKeMenit(int jamH, int jamM) {
    return jamH * 60 + jamM;
}

string menitKeJam(int menit) {
    int jamH = (menit / 60) % 24;
    int jamM = menit % 60;
    return (jamH < 10 ? "0" : "") + to_string(jamH) + "." +
           (jamM < 10 ? "0" : "") + to_string(jamM);
}

int maxHariBulan(int bln) {
    int hari[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return hari[bln];
}

// Meminta input tanggal (bulan + tanggal) dan jam mulai, lalu mengisinya ke parameter output.
// Jika user mengetik 'exit', bln akan bernilai -1 sebagai tanda pembatalan.
void inputTanggalDanJam(int &bln, int &tgl, int &jamH, int &jamM) {
    cout << "Tahun             : 2026\n";

    bln = inputAngka("Bulan (6-12)      : ", 6, 12, "Bulan yang tersedia hanya Juni-Desember.");
    if (bln == -1) return; // ketika user mengetik exit

    int maxHari = maxHariBulan(bln);
    int minHari = (bln == 6) ? 23 : 1; // jika bulan 6 = 23, selain itu 1
    string promptTgl = (bln == 6)
        ? "Tanggal (" + to_string(minHari) + "-" + to_string(maxHari) + ")  : "
        : "Tanggal (1-" + to_string(maxHari) + ")     : "; // Tanggal (23 - 30)
    string errorTgl = (bln == 6)
        ? "Jadwal hanya dapat dibuat mulai 23 Juni 2026."
        : "Tanggal tidak sesuai dengan jumlah hari pada bulan yang dipilih.";

    tgl = inputAngka(promptTgl, minHari, maxHari, errorTgl);
    if (tgl == -1) { bln = -1; return; }

    cout << "\n-- Masukkan Jam Mulai --\n";
    jamH = inputAngka("Jam (0-23)   : ", 0, 23);
    if (jamH == -1) { bln = -1; return; }

    jamM = inputAngka("Menit (0-59) : ", 0, 59);
    if (jamM == -1) { bln = -1; return; }
}

// ===================== SIGN UP & SIGN IN =====================

void signUp() {
    header("    S I G N  U P");
    if (jumlahUser >= MAX_USER) {
        cout << MERAH << "\nData user penuh, sign up tidak dapat dilakukan.\n" << RESET;
        pause(); return;
    }

    string username, password;
    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    do {
    cout << "\nUsername : ";
    getline(cin >> ws, username);

    if (toLower(username) == "exit") {
        cout << "\nSign up dibatalkan.\n";
        pause();
        return;
    }

    if (username.find(' ') != string::npos)
        cout << MERAH << "Username tidak boleh mengandung spasi.\n" << RESET;
    else if (username.empty())
        cout << MERAH << "Username tidak boleh kosong.\n" << RESET;
    else if (toLower(username) == toLower(adminUsn))
        cout << MERAH << "Username tidak boleh sama dengan akun admin.\n" << RESET;
    else if (cariUser(username) != -1)
        cout << MERAH << "Username sudah digunakan.\n" << RESET;

    } while (username.empty() ||
         username.find(' ') != string::npos ||
         toLower(username) == toLower(adminUsn) ||
         cariUser(username) != -1);

    do {
    cout << "Password : ";
    getline(cin >> ws, password);

    if (toLower(password) == "exit") {
        cout << "\nSign up dibatalkan.\n";
        pause();
        return;
    }

    if (password.find(' ') != string::npos)
        cout << MERAH << "Password tidak boleh mengandung spasi.\n" << RESET;
    else if (password.empty())
        cout << MERAH << "Password tidak boleh kosong.\n" << RESET;

    } while (password.empty() ||
         password.find(' ') != string::npos);

    user[jumlahUser].username = username;
    user[jumlahUser].password = password;
    jumlahUser++;

    cout << HIJAU_TERANG << "\nAkun berhasil dibuat.\n" << RESET;
    pause();
}

int signIn() {
    header("    S I G N  I N");
    string username, password;
    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    while (true){
        cout << "\nUsername : ";
        getline(cin >> ws, username);

        if (username.find(' ') != string::npos) {
        cout << MERAH << "Username tidak boleh mengandung spasi.\n" << RESET;
        continue; }

        if (toLower(username) == "exit") { cout << "\nSign in dibatalkan.\n"; pause(); return 0; }

        cout << "Password : ";
        getline(cin >> ws, password);
        if (toLower(password) == "exit") { cout << "\nSign in dibatalkan.\n"; pause(); return 0; }

        if (password.find(' ') != string::npos) {
        cout << MERAH << "Password tidak boleh mengandung spasi.\n" << RESET;
        continue;}

        if (username == adminUsn && password == adminPsw) {
            cout << HIJAU_TERANG << "\nLogin admin berhasil.\n" << RESET;
            pause(); return 1;
        }

        int indeks = cariUser(username);
        if (indeks != -1 && toLower(user[indeks].password) == toLower(password)) {
            indeksUserAktif = indeks;
            cout << HIJAU_TERANG << "\nSelamat datang, " << user[indeks].username << "!\n" << RESET;
            pause(); return 2;
        }

        cout << MERAH << "\nUsername atau password salah, silakan coba lagi.\n" << RESET;
    }
    return 0;
}

// ===================== MANAJEMEN FILM =====================

void tampilFilm() {
    if (jumlahFilm == 0) { cout << MERAH << "\nBelum ada film.\n" << RESET; return; }
    garis();
    cout << left << setw(8)  << "ID"
                 << MERAH_JAMBU << setw(20) << "Judul" << RESET
                 << COKLAT   << setw(10) << "Genre"  << RESET
                 << UNGU     << setw(8)  << "Durasi" << RESET
                 << TOSCA    << setw(8)  << "Usia"   << RESET
                 << BIRU     << "Harga"              << RESET << endl;
    garis();
    for (int i = 0; i < jumlahFilm; i++)
        cout << left << setw(8)  << film[i].id
                     << setw(20) << film[i].judul
                     << setw(10) << film[i].genre
                     << setw(8)  << film[i].durasi
                     << setw(8)  << film[i].rating
                     << film[i].harga << endl;
    garis();
}

string pilihGenre(string judul) {
    string genreList[5] = {"Action", "Horror", "Comedy", "Drama", "Animasi"};
    return genreList[pilihMenu(judul, genreList, 5)];
}

string pilihRating(string judul) {
    string ratingList[4] = {"SU", "13+", "17+", "21+"};
    return ratingList[pilihMenu(judul, ratingList, 4)];
}

void tambahFilm() {
    header("    T A M B A H  F I L M");
    if (jumlahFilm >= MAX_FILM) {
        cout << MERAH << "\nData film penuh, tidak bisa menambah film baru.\n" << RESET;
        pause(); return;
    }

    Film f;
    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    // ID Film dibuat otomatis (auto increment), tidak perlu input manual.
    f.id = idFilmBerikutnya;
    cout << "\nID Film    : " << f.id << " (otomatis)\n";

    // Input Judul
    do {
        cout << "Judul Film : "; getline(cin >> ws, f.judul);
        if (toLower(f.judul) == "exit") { cout << "\nDibatalkan.\n"; pause(); return; }
        else if (f.judul.empty()) cout << MERAH << "Judul tidak boleh kosong.\n" << RESET;
    } while (f.judul.empty());

    f.genre  = pilihGenre("PILIH GENRE");
    f.rating = pilihRating("PILIH RATING USIA");

    header("    T A M B A H  F I L M");
    cout << "\nID Film    : " << f.id    << endl;
    cout << "Judul Film : " << f.judul  << endl;
    cout << "Genre      : " << f.genre  << endl;
    cout << "Rating     : " << f.rating << endl;

    f.durasi = inputAngka("\nDurasi (40-360 menit)      : ", 40, 360);
    if (f.durasi == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

    int hargaInt = inputAngka("Harga Tiket (25000-300000) : ", 25000, 300000);
    if (hargaInt == -1) { cout << "\nDibatalkan.\n"; pause(); return; }
    f.harga = hargaInt;

    f.stok = TOTAL_KURSI; f.terjual = 0;
    film[jumlahFilm++] = f;
    idFilmBerikutnya++;
    cout << HIJAU_TERANG << "\nFilm berhasil ditambahkan.\n" << RESET;
    pause();
}

void editFilm() {
    header("    E D I T  F I L M");
    tampilFilm();
    if (jumlahFilm == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;
    int id, indeks;
    do {
        id = inputID("\nMasukkan ID Film : ");
        if (id == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        indeks = cariFilm(id);
        if (indeks == -1)
            cout << MERAH << "Film dengan ID tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    string judulBaru;
    do {
        cout << "Judul Baru : "; getline(cin >> ws, judulBaru);
        if (toLower(judulBaru) == "exit") { cout << "\nDibatalkan, data film tidak diubah.\n"; pause(); return; }
        if (judulBaru.empty()) cout << MERAH << "Judul tidak boleh kosong.\n" << RESET;
    } while (judulBaru.empty());

    string genreBaru  = pilihGenre("PILIH GENRE BARU");
    string ratingBaru = pilihRating("PILIH RATING USIA BARU");

    header("    E D I T  F I L M");

    int durasiBaru = inputAngka("\nDurasi Baru (40-360 menit)      : ", 40, 360);
    if (durasiBaru == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

    int hargaBaruInt = inputAngka("Harga Baru (25000-300000) : ", 25000, 300000);
    if (hargaBaruInt == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

    film[indeks].judul  = judulBaru;  film[indeks].genre  = genreBaru;
    film[indeks].rating = ratingBaru; film[indeks].durasi = durasiBaru;
    film[indeks].harga  = hargaBaruInt;
    // Stok dihitung ulang dari tiket yang sudah terjual, jangan langsung
    // di-reset ke TOTAL_KURSI, agar tiket yang sudah terjual tidak hilang.
    film[indeks].stok = TOTAL_KURSI - film[indeks].terjual;

    cout << HIJAU_TERANG << "\nFilm berhasil diubah.\n" << RESET;
    pause();
}

void hapusFilm() {
    header("    H A P U S  F I L M");
    tampilFilm();
    if (jumlahFilm == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;
    int id, indeks;
    do {
        id = inputID("\nMasukkan ID Film : ");
        if (id == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        indeks = cariFilm(id);
        if (indeks == -1)
            cout << MERAH << "Film dengan ID tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    bool dipakai = false;
    for (int i = 0; i < jumlahJadwal; i++)
        if (jadwal[i].film == indeks) { dipakai = true; break; }
    if (dipakai) {
        cout << MERAH << "\nFilm masih digunakan pada suatu jadwal, hapus jadwalnya terlebih dahulu.\n" << RESET;
        pause(); return;
    }

    for (int i = indeks; i < jumlahFilm - 1; i++) film[i] = film[i + 1]; //ngehapus film dengan cara menggeser seluruh data di belakangnya ke kiri
    jumlahFilm--;
    for (int i = 0; i < jumlahJadwal; i++)
        if (jadwal[i].film > indeks) jadwal[i].film--; //perbaiki referensi film pada seluruh jadwal.

    cout << HIJAU_TERANG << "\nFilm berhasil dihapus.\n" << RESET;
    pause();
}

void tampilFilmTerjual() {
    if (jumlahFilm == 0) { cout << MERAH << "\nBelum ada film.\n" << RESET; return; }
    int urut[MAX_FILM];
    for (int i = 0; i < jumlahFilm; i++) urut[i] = i;
    for (int i = 0; i < jumlahFilm - 1; i++)
        for (int j = i + 1; j < jumlahFilm; j++)
            if (film[urut[j]].terjual > film[urut[i]].terjual) {
                int tmp = urut[i]; urut[i] = urut[j]; urut[j] = tmp;
            } // mengurutkan indeks film terjual
    garis();
    cout << left << setw(8)  << "ID"
                 << MERAH_JAMBU << setw(20) << "Judul" << RESET
                 << COKLAT   << setw(10) << "Genre"  << RESET
                 << UNGU     << setw(8)  << "Durasi" << RESET
                 << TOSCA    << setw(8)  << "Usia"   << RESET
                 << BIRU     << setw(10) << "Harga"  << RESET
                 << HIJAU_TERANG << "Terjual" << RESET << endl;
    garis();
    for (int i = 0; i < jumlahFilm; i++) {
        int k = urut[i];
        cout << left << setw(8)  << film[k].id
                     << setw(20) << film[k].judul
                     << setw(10) << film[k].genre
                     << setw(8)  << film[k].durasi
                     << setw(8)  << film[k].rating
                     << setw(10) << film[k].harga
                     << film[k].terjual << endl;
    }
    garis();
}

void menuFilm() {
    string opsi[5] = {"Tambah Film", "Edit Film", "Hapus Film", "Lihat Film", "Kembali"};
    int pilihan;
    do {
        pilihan = pilihMenu(" M A N A J E M E N  F I L M", opsi, 5);
        switch (pilihan) {
            case 0: tambahFilm(); break;
            case 1: editFilm();   break;
            case 2: hapusFilm();  break;
            case 3: header("    D A F T A R  F I L M"); tampilFilmTerjual(); pause(); break;
        }
    } while (pilihan != 4);
}

// ===================== MANAJEMEN JADWAL =====================

void tampilJadwal() {
    if (jumlahJadwal == 0) { cout << MERAH << "\nBelum ada jadwal.\n" << RESET; return; }
    garis();
    cout << left << setw(8)  << "ID" << setw(20) << "Film"
                 << setw(8)  << "Studio" << setw(15) << "Tanggal" << "Jam" << endl;
    garis();
    for (int i = 0; i < jumlahJadwal; i++) // menampilkan seluruh jadwal
        cout << left << setw(8)  << jadwal[i].id
                     << setw(20) << film[jadwal[i].film].judul
                     << setw(8)  << jadwal[i].studio
                     << setw(15) << jadwal[i].tanggal
                     << jadwal[i].jam << " - " << jadwal[i].jamSelesai << endl;
    garis();
}

void tambahJadwal() {
    header("    T A M B A H  J A D W A L");
    if (jumlahFilm == 0) {
        cout << MERAH << "\nBelum ada film, tambahkan film terlebih dahulu.\n" << RESET;
        pause(); return;
    }
    if (jumlahJadwal >= MAX_JADWAL) {
        cout << MERAH << "\nData jadwal penuh, tidak bisa menambah jadwal baru.\n" << RESET;
        pause(); return;
    }

    tampilFilm();
    Jadwal j;
    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    // ID Jadwal dibuat otomatis (increment)
    j.id = idJadwalBerikutnya;
    cout << "\nID Jadwal : " << j.id << " (otomatis)\n";

    // Input ID Film
    int idFilm, indeksFilm;
    do {
        idFilm = inputID("ID Film   : ");
        if (idFilm == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        indeksFilm = cariFilm(idFilm);
        if (indeksFilm == -1)
            cout << MERAH << "Film dengan ID tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeksFilm == -1);
    j.film = indeksFilm;

    // Input Studio
    j.studio = inputAngka("Studio (1-" + to_string(MAX_STUDIO) + ") : ", 1, MAX_STUDIO);
    if (j.studio == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

    // Input Tanggal dan Jam (diulang jika bentrok dengan jadwal lain)
    bool bentrok;
    do {
        cout << "\n-- Masukkan Tanggal Tayang --\n";
        int bln = 0, tgl = 0, jamH = 0, jamM = 0;
        inputTanggalDanJam(bln, tgl, jamH, jamM);
        if (bln == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        j.tanggal = (tgl < 10 ? "0" : "") + to_string(tgl) + "/" +
                    (bln < 10 ? "0" : "") + to_string(bln) + "/2026";
        j.jam      = (jamH < 10 ? "0" : "") + to_string(jamH) + "." + (jamM < 10 ? "0" : "") + to_string(jamM);
        j.mulai    = jamKeMenit(jamH, jamM);
        j.selesai  = j.mulai + film[indeksFilm].durasi + 15;
        j.jamSelesai = menitKeJam(j.selesai);

        bentrok = cekBentrok(j.studio, j.tanggal, j.mulai, j.selesai, -1);
        if (bentrok)
            cout << MERAH << "\nJadwal bentrok dengan jadwal lain di studio yang sama, silakan masukkan tanggal/jam lain.\n" << RESET;
    } while (bentrok);

    for (int i = 0; i < TOTAL_KURSI; i++) j.kursi[i] = false;
    jadwal[jumlahJadwal++] = j; // tanggal akan disimpan disini
    idJadwalBerikutnya++;
    cout << HIJAU_TERANG << "\nJadwal berhasil ditambahkan (Jam " << j.jam << " - " << j.jamSelesai << ").\n" << RESET;
    pause();
}

void editJadwal() {
    header("    E D I T  J A D W A L");
    tampilJadwal();
    if (jumlahJadwal == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;
    int id, indeks;
    do {
        id = inputID("\nMasukkan ID Jadwal : ");
        if (id == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        indeks = cariJadwal(id);
        if (indeks == -1)
            cout << MERAH << "Jadwal dengan ID tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    // Input Studio Baru
    int studioBaru = inputAngka("Studio Baru (1-" + to_string(MAX_STUDIO) + ") : ", 1, MAX_STUDIO);
    if (studioBaru == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

    // Input Tanggal dan Jam Baru (diulang jika bentrok dengan jadwal lain)
    string tanggalBaru, jamBaru;
    int waktuMulaiBaru, waktuSelesaiBaru;
    bool bentrok;
    do {
        cout << "\n-- Masukkan Tanggal Baru --\n";
        int bln = 0, tgl = 0, jamH = 0, jamM = 0;
        inputTanggalDanJam(bln, tgl, jamH, jamM);
        if (bln == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        tanggalBaru = (tgl < 10 ? "0" : "") + to_string(tgl) + "/" +
                      (bln < 10 ? "0" : "") + to_string(bln) + "/2026";
        jamBaru     = (jamH < 10 ? "0" : "") + to_string(jamH) + "." + (jamM < 10 ? "0" : "") + to_string(jamM);

        waktuMulaiBaru   = jamKeMenit(jamH, jamM);
        waktuSelesaiBaru = waktuMulaiBaru + film[jadwal[indeks].film].durasi + 15;

        bentrok = cekBentrok(studioBaru, tanggalBaru, waktuMulaiBaru, waktuSelesaiBaru, indeks);
        if (bentrok)
            cout << MERAH << "\nJadwal bentrok dengan jadwal lain, silakan masukkan tanggal/jam lain.\n" << RESET;
    } while (bentrok);

    jadwal[indeks].studio     = studioBaru;
    jadwal[indeks].tanggal    = tanggalBaru;
    jadwal[indeks].jam        = jamBaru;
    jadwal[indeks].mulai      = waktuMulaiBaru;
    jadwal[indeks].selesai    = waktuSelesaiBaru;
    jadwal[indeks].jamSelesai = menitKeJam(waktuSelesaiBaru);

    cout << HIJAU_TERANG << "\nJadwal berhasil diubah.\n" << RESET;
    pause();
}

void hapusJadwal() {
    header("    H A P U S  J A D W A L");
    tampilJadwal();
    if (jumlahJadwal == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;
    int id, indeks;
    do {
        id = inputID("\nMasukkan ID Jadwal : ");
        if (id == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

        indeks = cariJadwal(id);
        if (indeks == -1)
            cout << MERAH << "Jadwal dengan ID tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    bool sudahTerjual = false;
    for (int i = 0; i < TOTAL_KURSI; i++)
        if (jadwal[indeks].kursi[i]) { sudahTerjual = true; break; } // cek jadwal dengan tiket terjual
    if (sudahTerjual) {
        cout << MERAH << "\nJadwal sudah memiliki tiket terjual, tidak bisa dihapus.\n" << RESET;
        pause(); return;
    }

    for (int i = indeks; i < jumlahJadwal - 1; i++) jadwal[i] = jadwal[i + 1]; // elemen jadwal yg dihapus di geser ke kiri
    jumlahJadwal--;
    for (int i = 0; i < jumlahTransaksi; i++)
        if (transaksi[i].jadwal > indeks) transaksi[i].jadwal--; // agar transaksi menunjuk jadwal yg benar

    cout << HIJAU_TERANG << "\nJadwal berhasil dihapus.\n" << RESET;
    pause();
}

void menuJadwal() {
    string opsi[5] = {"Tambah Jadwal", "Edit Jadwal", "Hapus Jadwal", "Lihat Jadwal", "Kembali"};
    int pilihan;
    do {
        pilihan = pilihMenu(" M A N A J E M E N  J A D W A L", opsi, 5);
        switch (pilihan) {
            case 0: tambahJadwal(); break;
            case 1: editJadwal();   break;
            case 2: hapusJadwal();  break;
            case 3: header("    D A F T A R  J A D W A L"); tampilJadwal(); pause(); break;
        }
    } while (pilihan != 4);
}

// ===================== MANAJEMEN PROMO =====================

void tampilPromo() {
    if (jumlahPromo == 0) { cout << MERAH << "\nBelum ada promo.\n" << RESET; return; }
    garis();
    cout << left << setw(15) << "Kode" << setw(10) << "Diskon" << "Status" << endl;
    garis();
    for (int i = 0; i < jumlahPromo; i++)
        cout << left << setw(15) << promo[i].kode
                     << setw(10) << promo[i].diskon
                     << (promo[i].aktif ? "Aktif" : "Nonaktif") << endl;
    garis();
}

// Meminta input kode promo dari pengguna. Mengembalikan "exit" jika user membatalkan.
string inputKodePromo() {
    string kode;
    bool valid;
    do {
        valid = true;
        cout << "  (tanpa spasi, contoh: DISKON20)\nKode Promo : ";
        getline(cin >> ws, kode);
        if (toLower(kode) == "exit") return "exit";
        if (kode.empty()) { cout << MERAH << "Kode tidak boleh kosong.\n" << RESET; valid = false; continue; }
        for (int i = 0; i < (int)kode.length(); i++)
            if (kode[i] == ' ') { cout << MERAH << "Kode promo tidak boleh mengandung spasi.\n" << RESET; valid = false; break; }
    } while (!valid);
    return kode;
}

void tambahPromo() {
    header("    T A M B A H  P R O M O");
    if (jumlahPromo >= MAX_PROMO) {
        cout << MERAH << "\nData promo penuh, tidak bisa menambah promo baru.\n" << RESET;
        pause(); return;
    }

    Promo p;
    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    do {
        p.kode = inputKodePromo();
        if (p.kode == "exit") { cout << "\nDibatalkan.\n"; pause(); return; }

        if (cariPromo(p.kode) != -1)
            cout << MERAH << "Kode promo sudah digunakan, gunakan kode lain.\n" << RESET;
    } while (cariPromo(p.kode) != -1);

    int diskon = inputAngka("Diskon (%) : ", 1, 100);
    if (diskon == -1) { cout << "\nDibatalkan.\n"; pause(); return; }
    p.diskon = diskon;

    p.aktif = true;
    promo[jumlahPromo++] = p;
    cout << HIJAU_TERANG << "\nPromo berhasil ditambahkan.\n" << RESET;
    pause();
}

void editPromo() {
    header("    E D I T  P R O M O");
    tampilPromo();
    if (jumlahPromo == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    string kode; int indeks;
    do {
        kode = inputKodePromo();
        if (kode == "exit") { cout << "\nDibatalkan.\n"; pause(); return; }

        indeks = cariPromo(kode);
        if (indeks == -1)
            cout << MERAH << "Promo dengan kode tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    int diskonBaru = inputAngka("Diskon Baru : ", 1, 100);
    if (diskonBaru == -1) { cout << "\nDibatalkan.\n"; pause(); return; }

    // Input status (0=nonaktif, 1=aktif)
    string inputStatus;
    bool validStatus;
    do {
        validStatus = true;
        cout << "Status (1=aktif, 0=nonaktif, 'exit'=batal) : ";
        getline(cin >> ws, inputStatus);
        if (toLower(inputStatus) == "exit") { cout << "\nDibatalkan, promo tidak diubah.\n"; pause(); return; }
        if (inputStatus != "0" && inputStatus != "1") {
            cout << MERAH << "Input tidak valid, masukkan 0 atau 1.\n" << RESET; validStatus = false;
        }
    } while (!validStatus);

    promo[indeks].diskon = diskonBaru;
    promo[indeks].aktif  = (inputStatus == "1");
    cout << HIJAU_TERANG << "\nPromo berhasil diubah.\n" << RESET;
    pause();
}

void hapusPromo() {
    header("    H A P U S  P R O M O");
    tampilPromo();
    if (jumlahPromo == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;

    string kode; int indeks;
    do {
        kode = inputKodePromo();
        if (kode == "exit") { cout << "\nDibatalkan.\n"; pause(); return; }

        indeks = cariPromo(kode);
        if (indeks == -1)
            cout << MERAH << "Promo dengan kode tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    for (int i = indeks; i < jumlahPromo - 1; i++) promo[i] = promo[i + 1];
    jumlahPromo--;
    cout << HIJAU_TERANG << "\nPromo berhasil dihapus.\n" << RESET;
    pause();
}

void menuPromo() {
    string opsi[5] = {"Tambah Promo", "Edit Promo", "Hapus Promo", "Lihat Promo", "Kembali"};
    int pilihan;
    do {
        pilihan = pilihMenu("    M A N A J E M E N  P R O M O", opsi, 5);
        switch (pilihan) {
            case 0: tambahPromo(); break;
            case 1: editPromo();   break;
            case 2: hapusPromo();  break;
            case 3: header("    D A F T A R  P R O M O"); tampilPromo(); pause(); break;
        }
    } while (pilihan != 4);
}

// ===================== LAPORAN =====================

void laporanPenjualan() {
    header("L A P O R A N  P E N J U A L A N");
    long long totalPendapatan; int totalTiket;
    hitungTotal(&totalPendapatan, &totalTiket);
    cout << "\nTotal Tiket Terjual : " << totalTiket      << endl;
    cout << "Total Transaksi     : " << jumlahTransaksi  << endl;
    cout << "Total Pendapatan    : Rp" << totalPendapatan << endl;
    garis();
    if (jumlahTransaksi == 0) {
        cout << MERAH << "Belum ada transaksi.\n" << RESET;
    } else {
        for (int i = 0; i < jumlahTransaksi; i++)
            cout << transaksi[i].id << "  Rp" << transaksi[i].total << endl;
    }
    pause();
}

// ===================== MENU ADMIN =====================

void menuAdmin() {
    string opsi[5] = {"Manajemen Film", "Manajemen Jadwal", "Manajemen Promo", "Laporan Penjualan", "Logout"};
    int pilihan;
    do {
        pilihan = pilihMenu("    M E N U  A D M I N", opsi, 5);
        switch (pilihan) {
            case 0: menuFilm();         break;
            case 1: menuJadwal();       break;
            case 2: menuPromo();        break;
            case 3: laporanPenjualan(); break;
        }
    } while (pilihan != 4);
}

// ===================== FITUR CUSTOMER =====================

void berandaCustomer() {
    header(" B E R A N D A  C U S T O M E R");
    cout << "\nSelamat datang, " << user[indeksUserAktif].username << "!\n\nDaftar Film\n";
    tampilFilm();
    pause();
}

void tampilKursi(int indeks) {
    cout << "\n          [ LAYAR BIOSKOP ]\n\n     ";
    for (int k = 1; k <= 8; k++) cout << " " << k << " "; // nomor kolom
    cout << endl;
    garis();
    for (int baris = 0; baris < 5; baris++) { // baris kursi
        cout << "  " << (char)('A' + baris) << "  ";
        for (int kolom = 0; kolom < 8; kolom++) {
            int nomorKursi = baris * 8 + kolom;
            if (jadwal[indeks].kursi[nomorKursi]) cout << MERAH << "[X]" << RESET;
            else cout << HIJAU_TERANG << "[O]" << RESET;
        }
        cout << endl;
    }
    garis();
    cout << "  Keterangan: " << HIJAU_TERANG << "[O]" << RESET << " Tersedia   "
         << MERAH << "[X]" << RESET << " Terisi\n";
    garis();
}

void pembayaran();

void reservasiTiket() {
    header("    R E S E R V A S I  T I K E T");
    tampilJadwal();
    if (jumlahJadwal == 0) { pause(); return; }

    cout << MERAH << "(ketik 'exit' untuk batal)\n" << RESET;
    int id, indeks;
    do {
        id = inputID("\nID Jadwal : ");
        if (id == -1) { cout << "\nReservasi dibatalkan.\n"; pause(); return; }

        indeks = cariJadwal(id);
        if (indeks == -1)
            cout << MERAH << "Jadwal dengan ID tersebut tidak ditemukan, silakan coba lagi.\n" << RESET;
    } while (indeks == -1);

    int kursiTersedia = 0;
    for (int i = 0; i < TOTAL_KURSI; i++)
        if (!jadwal[indeks].kursi[i]) kursiTersedia++;
    if (kursiTersedia == 0) {
        cout << MERAH << "\nMaaf, kursi pada jadwal ini sudah penuh.\n" << RESET;
        pause(); return;
    }

    int jumlahTiket = inputAngka("Jumlah Tiket (1-" + to_string(kursiTersedia) + ") : ", 1, kursiTersedia);
    if (jumlahTiket == -1) { cout << "\nReservasi dibatalkan.\n"; pause(); return; }

    tampilKursi(indeks); // nilai indeks pada tampilKursi(indeks) diperoleh dari hasil fungsi cariJadwal(id), yaitu posisi (index array) dari jadwal yang dipilih pengguna.
    cout << "\nPilih kursi (contoh: A1, B3, E8), ketik 'exit' untuk batal\n";

    int  kursiPilihan[TOTAL_KURSI];
    bool batal = false;

    for (int i = 0; i < jumlahTiket && !batal; i++) {
        string inputKursi;
        int    nomorKursi = -1; // cek nomor kursi benar atau salah
        bool   valid; // variabel sementara
        do {
            valid = true;
            cout << "Kursi " << i + 1 << " : "; getline(cin >> ws, inputKursi);
            if (toLower(inputKursi) == "exit") { batal = true; break; }

            if (inputKursi.find(' ') != string::npos) { cout << MERAH << "Format tidak valid, jangan menggunakan spasi. Contoh: A1\n" << RESET;
            valid = false; continue; }

            if (inputKursi.length() != 2) { cout << MERAH << "Format tidak valid, contoh: A1\n" << RESET; valid = false; continue; }
            char baris = inputKursi[0];

            if (baris >= 'a' && baris <= 'z') baris -= 32;

            if (baris < 'A' || baris > 'E') { cout << MERAH << "Baris tidak valid, gunakan A-E.\n" << RESET; valid = false; continue; }

            if (!isdigit(inputKursi[1])) { cout << MERAH << "Kolom tidak valid, gunakan 1-8.\n" << RESET; valid = false; continue; }

            int kolom = inputKursi[1] - '0';

            if (kolom < 1 || kolom > 8) { cout << MERAH << "Kolom tidak valid, gunakan 1-8.\n" << RESET; valid = false; continue; }
            nomorKursi = (baris - 'A') * 8 + (kolom - 1);

            if (jadwal[indeks].kursi[nomorKursi]) { cout << MERAH << "Kursi " << baris << kolom << " sudah terisi, pilih kursi lain.\n" << RESET; valid = false; continue; }

            for (int k = 0; k < i && valid; k++)
                if (kursiPilihan[k] - 1 == nomorKursi) { cout << MERAH << "Kursi " << baris << kolom << " sudah dipilih sebelumnya.\n" << RESET; valid = false; }
        } while (!valid);
        if (!batal) kursiPilihan[i] = nomorKursi + 1;
    }

    if (batal) { cout << "\nReservasi dibatalkan.\n"; pause(); return; }

    indeksJadwalSementara = indeks;
    indeksFilmSementara   = jadwal[indeks].film;
    jumlahTiketSementara  = jumlahTiket;
    for (int i = 0; i < jumlahTiket; i++) kursiSementara[i] = kursiPilihan[i];

    cout << "\nKursi yang dipilih:\n";
    for (int i = 0; i < jumlahTiketSementara; i++)
        cout << i + 1 << ". " << KUNING_TERANG << namaKursi(kursiSementara[i] - 1) << RESET << endl;
    cout << HIJAU_TERANG << "\nReservasi berhasil.\n" << RESET;
    pause();

    pembayaran();
}

string buatIDTrx()     { return "TRX" + to_string(jumlahTransaksi + 1001); }
string buatKodeTiket() { return "TKT" + to_string(jumlahTransaksi + 5001); }

void pembayaran() {
    header("    P E M B A Y A R A N");

    if (indeksFilmSementara < 0 || indeksFilmSementara >= jumlahFilm ||
        indeksJadwalSementara < 0 || indeksJadwalSementara >= jumlahJadwal) {
        cout << MERAH << "\nData reservasi tidak valid. Silakan lakukan reservasi ulang.\n" << RESET;
        resetReservasiSementara(); pause(); return;
    }

    long long totalTiket = jumlahTiketSementara * film[indeksFilmSementara].harga;
    long long total      = totalTiket;

    cout << "\nFilm        : " << film[indeksFilmSementara].judul << endl;
    cout << "Total Bayar : Rp" << total << endl;

    // ---------- Validasi Kode Promo (dengan kesempatan input ulang) ----------
    int diskon = 0;
    bool  promoSelesai = false;

    do {
        string kode;
        cout << "\nKode Promo (ketik '-' jika tidak ada, 'exit' untuk batal) : ";
        getline(cin >> ws, kode);

        if (toLower(kode) == "exit") {
            cout << "\nPembayaran dibatalkan.\n";
            resetReservasiSementara();
            pause(); return;
        }

        if (kode == "-") {
            cout << KUNING_TERANG << "\nTidak menggunakan kode promo.\n" << RESET;
            promoSelesai = true;
        } else {
            int indeks = cariPromo(kode);
            if (indeks == -1) {
                cout << MERAH << "Kode promo tidak ditemukan, silakan coba lagi.\n" << RESET;
            } else if (!promo[indeks].aktif) {
                cout << MERAH << "Kode promo tidak aktif, silakan coba lagi.\n" << RESET;
            } else {
                diskon = total * promo[indeks].diskon / 100;
                total -= diskon;
                cout << HIJAU_TERANG << "Promo berhasil diterapkan (" << promo[indeks].diskon << "%).\n" << RESET;
                promoSelesai = true;
            }
        }
    } while (!promoSelesai);

    cout << "\nDiskon      : Rp" << diskon << endl;
    cout << "Total Akhir : Rp" << total   << endl;

    if (jumlahTransaksi >= MAX_TRX) {
        cout << MERAH << "\nData transaksi penuh, pembayaran tidak dapat diproses.\n" << RESET;
        resetReservasiSementara();
        pause(); return;
    }

    cout << "\nTekan sembarang tombol untuk lanjut ke pembayaran QRIS...";
    getch();

    // ---------- Tampilkan QRIS untuk pembayaran ----------
    header("    P E M B A Y A R A N  Q R I S");
    cout << "\nMerchant : CinemaXXV\n";
    cout << "Metode   : QRIS\n";
    cout << "Total    : Rp" << total << "\n\n";
    tampilQRCode();
    cout << "\nSilakan scan QR di atas untuk melakukan pembayaran.\n";
    cout << "Tekan Enter setelah pembayaran QRIS selesai dilakukan...";
    cin.ignore(1000, '\n');
    cin.get();

    cout << HIJAU_TERANG << "\nPembayaran berhasil.\n" << RESET;
    pause();

    Transaksi &t  = transaksi[jumlahTransaksi];
    t.id          = buatIDTrx();   t.kodeTiket   = buatKodeTiket();
    t.user        = indeksUserAktif;
    t.jadwal      = indeksJadwalSementara; t.jumlahTiket = jumlahTiketSementara;
    t.total       = total;         t.diskon      = diskon;

    for (int i = 0; i < jumlahTiketSementara; i++) {
        t.kursi[i] = kursiSementara[i];
        jadwal[indeksJadwalSementara].kursi[kursiSementara[i] - 1] = true;
    }
    film[indeksFilmSementara].terjual += jumlahTiketSementara;
    jumlahTransaksi++;
    resetReservasiSementara();
}

void cetakTiket() {
    header("    C E T A K  T I K E T");
    if (jumlahTransaksi == 0) {
        cout << MERAH << "\nBelum ada transaksi, tidak ada tiket yang bisa dicetak.\n" << RESET;
        pause(); return;
    }

    int indeksTransaksi = -1;
    for (int i = jumlahTransaksi - 1; i >= 0; i--)
        if (transaksi[i].user == indeksUserAktif) { indeksTransaksi = i; break; }
    if (indeksTransaksi == -1) { cout << MERAH << "\nAnda belum memiliki transaksi.\n" << RESET; pause(); return; }

    Transaksi &t = transaksi[indeksTransaksi];
    if (t.jadwal < 0 || t.jadwal >= jumlahJadwal) {
        cout << MERAH << "\nData jadwal untuk transaksi ini sudah tidak tersedia.\n" << RESET; pause(); return;
    }
    Jadwal &j = jadwal[t.jadwal];
    if (j.film < 0 || j.film >= jumlahFilm) {
        cout << MERAH << "\nData film untuk transaksi ini sudah tidak tersedia.\n" << RESET; pause(); return;
    }

    cout << "\nKode Tiket   : " << KUNING_TERANG << t.kodeTiket << RESET << endl;
    cout << "Film         : " << film[j.film].judul << endl;
    cout << "Tanggal      : " << j.tanggal << endl;
    cout << "Jam          : " << j.jam << " - " << j.jamSelesai << endl;
    cout << "Studio       : " << j.studio << endl;
    cout << "Jumlah Tiket : " << t.jumlahTiket << endl;
    cout << "Kursi        : ";
    for (int i = 0; i < t.jumlahTiket; i++) {
        cout << KUNING_TERANG << namaKursi(t.kursi[i] - 1) << RESET;
        if (i < t.jumlahTiket - 1) cout << ", ";
    }
    cout << endl;
    pause();
}

void riwayatPemesanan() {
    header("  R I W A Y A T  P E M E S A N A N");
    bool ada = false;
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (transaksi[i].user != indeksUserAktif) continue;
        ada = true;
        cout << "\nID Transaksi : " << BIRU << transaksi[i].id << RESET << endl;
        int indeksJadwal = transaksi[i].jadwal;
        if (indeksJadwal >= 0 && indeksJadwal < jumlahJadwal &&
            jadwal[indeksJadwal].film >= 0 && jadwal[indeksJadwal].film < jumlahFilm)
            cout << "Film         : " << film[jadwal[indeksJadwal].film].judul << endl;
        else
            cout << "Film         : " << MERAH << "(data tidak tersedia)" << RESET << endl;
        cout << "Jumlah Tiket : " << transaksi[i].jumlahTiket << endl;
        cout << "Total Bayar  : Rp" << transaksi[i].total << endl;
        cout << "Metode Bayar : " << KUNING_TERANG << "QRIS" << RESET << endl;
        garis();
    }
    if (!ada) cout << MERAH << "\nBelum ada riwayat pemesanan.\n" << RESET;
    pause();
}

// ===================== MENU CUSTOMER =====================

void menuCustomer() {
    string opsi[5] = {"Beranda", "Reservasi Tiket", "Cetak Tiket", "Riwayat Pemesanan", "Logout"};
    int pilihan;
    do {
        pilihan = pilihMenu("  M E N U  C U S T O M E R", opsi, 5);
        switch (pilihan) {
            case 0: berandaCustomer(); break;
            case 1: reservasiTiket();  break;
            case 2: cetakTiket();      break;
            case 3: riwayatPemesanan(); break;
        }
    } while (pilihan != 4);
    indeksUserAktif = -1;
    resetReservasiSementara();
}

// ===================== MENU UTAMA =====================

void menuUtama() {
    string opsi[3] = {"Sign Up", "Sign In", "Exit"};
    int pilihan;
    do {
        pilihan = pilihMenu("   C I N E M A X X V", opsi, 3);
        if (pilihan == 0) { signUp(); }
        else if (pilihan == 1) {
            int login = signIn();
            if (login == 1)      menuAdmin();
            else if (login == 2) menuCustomer();
        }
    } while (pilihan != 2);
}

// ===================== DATA DUMMY =====================
// Fungsi ini mengisi data awal (film, jadwal, promo) agar program
// bisa langsung diuji coba tanpa perlu input manual dari admin.
// ID Film dan ID Jadwal tetap menggunakan sistem auto increment
// yang sama seperti pada fitur Tambah Film / Tambah Jadwal.
void isiDataDummy() {
    // ---------- Film Dummy (2 film, genre bervariasi) ----------
    film[jumlahFilm].id = idFilmBerikutnya++;
    film[jumlahFilm].judul = "The Last Stand";
    film[jumlahFilm].genre = "Action";
    film[jumlahFilm].rating = "17+";
    film[jumlahFilm].durasi = 120;
    film[jumlahFilm].harga = 45000;
    film[jumlahFilm].stok = TOTAL_KURSI;
    film[jumlahFilm].terjual = 0;
    jumlahFilm++;

    film[jumlahFilm].id = idFilmBerikutnya++;
    film[jumlahFilm].judul = "Malam Tanpa Bulan";
    film[jumlahFilm].genre = "Horror";
    film[jumlahFilm].rating = "21+";
    film[jumlahFilm].durasi = 100;
    film[jumlahFilm].harga = 40000;
    film[jumlahFilm].stok = TOTAL_KURSI;
    film[jumlahFilm].terjual = 0;
    jumlahFilm++;

    // ---------- Jadwal Dummy (2 jadwal, studio berbeda, tidak bentrok) ----------
    // Jadwal 1: Film index 0, Studio 1, 25/06/2026, jam 13.00
    jadwal[jumlahJadwal].id = idJadwalBerikutnya++;
    jadwal[jumlahJadwal].film = 0;
    jadwal[jumlahJadwal].studio = 1;
    jadwal[jumlahJadwal].tanggal = "25/06/2026";
    jadwal[jumlahJadwal].jam = "13.00";
    jadwal[jumlahJadwal].mulai = jamKeMenit(13, 0);
    jadwal[jumlahJadwal].selesai = jadwal[jumlahJadwal].mulai + film[jadwal[jumlahJadwal].film].durasi + 15;
    jadwal[jumlahJadwal].jamSelesai = menitKeJam(jadwal[jumlahJadwal].selesai);
    for (int i = 0; i < TOTAL_KURSI; i++) jadwal[jumlahJadwal].kursi[i] = false;
    jumlahJadwal++;

    // Jadwal 2: Film index 1, Studio 2, 25/06/2026, jam 15.00
    jadwal[jumlahJadwal].id = idJadwalBerikutnya++;
    jadwal[jumlahJadwal].film = 1;
    jadwal[jumlahJadwal].studio = 2;
    jadwal[jumlahJadwal].tanggal = "25/06/2026";
    jadwal[jumlahJadwal].jam = "15.00";
    jadwal[jumlahJadwal].mulai = jamKeMenit(15, 0);
    jadwal[jumlahJadwal].selesai = jadwal[jumlahJadwal].mulai + film[jadwal[jumlahJadwal].film].durasi + 15;
    jadwal[jumlahJadwal].jamSelesai = menitKeJam(jadwal[jumlahJadwal].selesai);
    for (int i = 0; i < TOTAL_KURSI; i++) jadwal[jumlahJadwal].kursi[i] = false;
    jumlahJadwal++;

    // ---------- Promo Dummy (2 promo) ----------
    promo[jumlahPromo].kode = "DISKON10";
    promo[jumlahPromo].diskon = 10;
    promo[jumlahPromo].aktif = true;
    jumlahPromo++;

    promo[jumlahPromo].kode = "DISKON20";
    promo[jumlahPromo].diskon = 20;
    promo[jumlahPromo].aktif = true;
    jumlahPromo++;
}

int main() {
    isiDataDummy();
    menuUtama();
    return 0;
}
