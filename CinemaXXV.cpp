#include <iostream>
#include <iomanip>
#include <limits>
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
    float  harga;
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
    float  total, diskon;
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
int indeksJadwalSementara = -1;
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

string namaKursi(int nk) {
    return string(1, char('A' + nk / 8)) + to_string(nk % 8 + 1);
}

// ===================== TAMPILAN QR CODE (QRIS) =====================
// Fungsi ini HANYA bertugas menampilkan QR Code dummy (visual saja,
// tidak bisa discan). Ukuran 15x15 karakter, dibuat manual memakai
// karakter '#' dan spasi, tanpa library atau algoritma QR apapun.
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

// ===================== FUNGSI INPUT PEMBANTU =====================

// Meminta input angka dari pengguna. Mengembalikan -1 jika user ketik 'exit'.
// Terus meminta ulang sampai input valid dan berada dalam rentang [batasBawah, batasAtas].
int inputAngka(string prompt, int batasBawah, int batasAtas, string pesanError = "") {
    string input;
    int nilai;
    bool valid;
    do {
        valid = true;
        cout << prompt;
        cin  >> input;

        if (toLower(input) == "exit") return -1;

        bool angkaValid = !input.empty();
        for (int i = 0; i < (int)input.length(); i++)
            if (!isdigit(input[i])) { angkaValid = false; break; }

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
        cin  >> input;
        if (toLower(input) == "exit") return -1;

        bool angkaValid = true;
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
            panjangMaks = opsi[i].length();

    int lebarBlok = 4 + panjangMaks;
    int padKiri   = (LEBAR_KONSOL - lebarBlok) / 2;
    if (padKiri < 0) padKiri = 0;
    string spasi = string(padKiri, ' ');

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
            if (tombolKeyboard == 72)      pilihan = (pilihan - 1 + jumlahOpsi) % jumlahOpsi;
            else if (tombolKeyboard == 80) pilihan = (pilihan + 1) % jumlahOpsi;
        }
    } while (tombolKeyboard != 13);
    return pilihan;
}

// ===================== SEARCHING =====================

int cariUser(string usn) {
    for (int i = 0; i < jumlahUser; i++)
        if (toLower(user[i].username) == toLower(usn)) return i;
    return -1;
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
                return true;
    }
    return false;
}

// ===================== POINTER =====================

void hitungTotal(float *totalPendapatan, int *totalTiket) {
    *totalPendapatan = 0; *totalTiket = 0;
    for (int i = 0; i < jumlahTransaksi; i++) {
        *totalPendapatan += transaksi[i].total;
        *totalTiket      += transaksi[i].jumlahTiket;
    }
}

// ===================== UTILITAS JAM =====================

int jamKeMenit(string jam) {
    for (int i = 0; i < (int)jam.length(); i++)
        if (jam[i] == ':') jam[i] = '.';
    int pos = jam.find('.');
    if (pos == (int)string::npos || pos == 0 || pos >= (int)jam.length() - 1) return -1;
    string hStr = jam.substr(0, pos), mStr = jam.substr(pos + 1);
    bool valT = true;
    for (char c : hStr) if (!isdigit(c)) valT = false;
    for (char c : mStr) if (!isdigit(c)) valT = false;
    if (!valT || hStr.length() > 2 || mStr.length() > 2) return -1;
    return stoi(hStr) * 60 + stoi(mStr);
}

string menitKeJam(int menit) {
    int jamH = (menit / 60) % 24;
    int jamM = menit % 60;
    return (jamH < 10 ? "0" : "") + to_string(jamH) + "." +
           (jamM < 10 ? "0" : "") + to_string(jamM);
}

int maxHariBulan(int bln, int thn = 2026) {
    int hari[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (bln == 2 && (thn % 4 == 0 && (thn % 100 != 0 || thn % 400 == 0))) hari[2] = 29;
    return hari[bln];
}

// Meminta input tanggal (bulan + tanggal) dan jam mulai, lalu mengisinya ke parameter output.
// Jika user mengetik 'exit', bln akan bernilai -1 sebagai tanda pembatalan.
void inputTanggalDanJam(int &bln, int &tgl, int &jamH, int &jamM) {
    int thn = 2026;
    cout << "Tahun             : 2026\n";

    bln = inputAngka("Bulan (6-12)      : ", 6, 12, "Bulan yang tersedia hanya Juni-Desember.");
    if (bln == -1) return;

    int maxHari = maxHariBulan(bln, thn);
    int minHari = (bln == 6) ? 23 : 1;
    string promptTgl = (bln == 6)
        ? "Tanggal (" + to_string(minHari) + "-" + to_string(maxHari) + ")  : "
        : "Tanggal (1-" + to_string(maxHari) + ")     : ";
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
        if (toLower(username) == "exit") { cout << "\nSign up dibatalkan.\n"; pause(); return; }

        if (username.empty())
            cout << MERAH << "Username tidak boleh kosong.\n" << RESET;
        else if (toLower(username) == toLower(adminUsn))
            cout << MERAH << "Username tidak boleh sama dengan akun admin, gunakan username lain.\n" << RESET;
        else if (cariUser(username) != -1)
            cout << MERAH << "Username sudah digunakan, gunakan username lain.\n" << RESET;
    } while (username.empty() || toLower(username) == toLower(adminUsn) || cariUser(username) != -1);

    do {
        cout << "Password : ";
        getline(cin >> ws, password);
        if (toLower(password) == "exit") { cout << "\nSign up dibatalkan.\n"; pause(); return; }
        if (password.empty()) cout << MERAH << "Password tidak boleh kosong.\n" << RESET;
    } while (password.empty());

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
        cout << "\nUsername : "; cin >> username;
        if (toLower(username) == "exit") { cout << "\nSign in dibatalkan.\n"; pause(); return 0; }

        cout << "Password : "; cin >> password;
        if (toLower(password) == "exit") { cout << "\nSign in dibatalkan.\n"; pause(); return 0; }

        if (username == adminUsn && password == adminPsw) {
            cout << HIJAU_TERANG << "\nLogin admin berhasil.\n" << RESET;
            pause(); return 1;
        }

        int indeks = cariUser(username);
        if (indeks != -1 && user[indeks].password == password) {
            indeksUserAktif = indeks;
            cout << HIJAU_TERANG << "\nSelamat datang, " << user[indeks].username << "!\n" << RESET;
            pause(); return 2;
        }

        cout << MERAH << "\nUsername atau password salah, silakan coba lagi.\n" << RESET;
    }
    return 0;
}
