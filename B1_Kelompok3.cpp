#include <iostream> //untuk input dan output standar pada C++
#include <istream>  //untuk membaca input dari keyboard atau sumber input lainnya
#include <fstream>  //untuk membaca database
#include <string.h>     //untuk operasi-operasi string pada C
#include <vector>   //untuk menyimpan dan manipulasi data secara dinamis
#include <conio.h>  //untuk operasi-operasi pada console seperti getch()
#include <sstream>  //untuk mengoperasikan string seperti stream
#include <windows.h> //Library untuk pemrograman Windows
#include <limits>   //menyediakan informasi tentang batas-batas tipe data
#include <cmath>    //Library matematika pada C++
#include <iomanip> // Untuk menggunakan manipulator setw
#include <queue>    // menyediakan implementasi struktur data antrian (queue)
#include <cctype>  // untuk std::tolower
using namespace std;


//untuk menyimpan informasi pengguna 
struct User
{
    string username;
    string password;
};

// untuk menyimpan informasi kamar
struct room
{
    int id;
    string jenis_kamar;
    int no_kamar;
    string status;
    int harga;
    room *next = NULL;
    room *prev = NULL;
};

// untuk menyimpan informasi reservasi
struct Reservation {
    int reservationId;
    string username;
    int no_kamar;
    time_t timestamp; // Add timestamp field
    Reservation* next;
};

//membentuk node dalam linked list kamar
struct Node
{
    room data; // data kamar
    Reservation* reservations; // daftar reservasi
    Node *next; //pointer ke node selanjutnya
};

int nextRoomId = 1; //untuk memberikan ID unik untuk kamar berikutnya
int nextReservationId = 1; //untuk memberikan ID unik untuk reservasi berikutnya
Node *head = nullptr; //pointer ke node pertama dalam linked list kamar

//menyimpan informasi pencarian
struct SearchResult
{
    int id;
    string jenis_kamar;
    int no_kamar;
    string status;
    int harga;
};

//Membaca data kamar dari pengguna dan mengembalikan struktur kamar (room)
room readRoomData()
{
    room data;
    data.id = nextRoomId++;
    cout << "Jenis Kamar : ";
    cin >> ws;
    getline(cin, data.jenis_kamar);
    cout << "Nomor Kamar : ";
    cin >> data.no_kamar;
    cout << "Status Kamar : ";
    cin >> ws;
    getline(cin, data.status);
    cout << "Harga Kamar : ";
    cin >> data.harga;
    return data;
};

//Mengupdate ID kamar dalam linked list berdasarkan urutan node
void updateIds(Node *head)
{
    Node *temp = head;
    int newId = 1;
    while (temp != nullptr)
    {
        temp->data.id = newId++;
        temp = temp->next;
    }
    nextRoomId = newId;
}

//Memeriksa ketersediaan kamar berdasarkan nomor kamar.
bool isRoomAvailable(Node* head, int no_kamar) {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.no_kamar == no_kamar) {
            Reservation* reservation = current->reservations;
            while (reservation != nullptr) {
                if (reservation->no_kamar == no_kamar) {
                    return false;
                }
                reservation = reservation->next;
            }
            return true;
        }
        current = current->next;
    }
    return false;
}

//menu untuk menampilkan data kamar
void display(Node *head) {
    if (head == nullptr) //mengecek apakah data ada 
    {
        cout << "Data Kamar Masih Kosong." << endl;
        return;
    }

    // Menentukan lebar kolom
    const int idWidth = 4;
    const int jenisKamarWidth = 11;
    const int nomorKamarWidth = 11;
    const int statusWidth = 17;
    const int hargaWidth = 8;

    cout << "\n===================================================================" << endl;
    cout << "|                    D A T A   S A A T   I N I                    |" << endl;
    cout << "===================================================================" << endl;
    cout << "| ID   | Jenis Kamar | Nomor Kamar | Status            | Harga    |" << endl;
    cout << "-------------------------------------------------------------------" << endl;

    Node *current = head;
    while (current != nullptr)
    {
        string statusLabel;
        string statusLowerCase; // Untuk menyimpan versi status dalam huruf kecil

        // Ubah status menjadi huruf kecil
        for (char &c : current->data.status) {
            statusLowerCase += tolower(c);
        }

        // Periksa status yang sudah diubah menjadi huruf kecil
        if (statusLowerCase == "available") {
            statusLabel = "Available";
        } else if (statusLowerCase == "reserved") {
            statusLabel = "Reserved";
        } else {
            statusLabel = "Unknown";
        }

        cout << "| " << setw(idWidth) << current->data.id << " | "
            << setw(jenisKamarWidth) << current->data.jenis_kamar << " | "
            << setw(nomorKamarWidth) << current->data.no_kamar << " | "
            // << setw(statusWidth) << current->data.status << " | "
            << setw(statusWidth) << statusLabel << " | "
            << setw(hargaWidth) << current->data.harga << " |" << endl;
        current = current->next;
    }
    cout << "===================================================================" << endl;
    
}


//membuat reservasi
void createReservation(Node* head, string username) {
    int no_kamar;
    display(head);
    cout << "Masukkan nomor kamar yang ingin Anda pesan: ";
    cin >> no_kamar; // Meminta input nomor kamar dari pengguna

    // Memeriksa apakah kamar tersedia
    if (isRoomAvailable(head, no_kamar)) {
        // Membuat objek baru Reservation untuk menyimpan informasi reservasi
        Reservation* newReservation = new Reservation{
            nextReservationId++, // reservationId
            username,
            no_kamar,
            time(nullptr), // Inisialisasi timestamp dengan waktu saat ini
            nullptr
        };

        // Mencari kamar dengan nomor kamar yang sesuai
        Node* current = head;
        while (current != nullptr) {
            if (current->data.no_kamar == no_kamar) {
                // Menambahkan reservasi ke dalam daftar reservasi kamar
                if (current->reservations == nullptr) {
                    current->reservations = newReservation;
                } else {
                    Reservation* temp = current->reservations;
                    while (temp->next != nullptr) {
                        temp = temp->next;
                    }
                    temp->next = newReservation;
                }
                // Menyimpan informasi reservasi ke dalam file CSV
                ofstream reservationFile("reservasi_info.csv", ios::app);
                if (reservationFile.is_open()) {
                    reservationFile << newReservation->reservationId << "," << newReservation->username << "," << newReservation->no_kamar << "," << newReservation->timestamp << endl;
                    reservationFile.close();
                } else {
                    cout << "Gagal membuka file reservasi_info.csv." << endl;
                }
                
                // Menandai kamar sebagai "reserved"
                current->data.status = "reserved";
                cout << "Reservasi berhasil! Nomor reservasi Anda adalah: " << newReservation->reservationId << endl;
                return;
            }
            current = current->next;
        }
    } else {
        cout << "Maaf, kamar yang Anda pilih sudah dipesan. Silakan pilih kamar lain.\n";
    }
}

//menambahkan data baru
void create(Node *&head, room data)
{
    // Membuat node baru dengan data kamar yang diberikan
    Node *newNode = new Node{data, nullptr};
    // Jika linked list kosong, node baru menjadi head
    if (head == nullptr)
    {
        head = newNode;
    }
    else
    {
        // Jika linked list tidak kosong, mencari node terakhir
        Node *temp = head;
        while (temp->next != nullptr)
        {
            temp = temp->next;
        }
        // Menambahkan node baru ke akhir linked list
        temp->next = newNode;
        // Memperbarui ID setiap node untuk menjamin urutan yang benar
        updateIds(head);
    }
    display(head); // Menampilkan linked list setelah penambahan
}

void update(Node *&head)
{
    int id;
    // Menampilkan data kamar yang ada
    display(head);
    // Meminta input ID kamar yang ingin diubah
    cout << "Masukkan ID Kamar yang ingin diubah : ";
    cin >> id;

    // Mencari kamar dengan ID yang sesuai
    Node *temp = head;
    while (temp != nullptr)
    {
        if (temp->data.id == id)
        {
            int pilihUbah;
            // Menampilkan menu pilihan untuk mengubah data kamar
            cout << "\n================================================" << endl;
            cout << "|     Pilih data Kamar yang ingin diubah :     |" << endl;
            cout << "------------------------------------------------" << endl;
            cout << "|      [ 1 ]   Jenis Kamar                     |" << endl;
            cout << "|      [ 2 ]   Nomor Kamar                     |" << endl;
            cout << "|      [ 3 ]   Status Kamar                    |" << endl;
            cout << "|      [ 4 ]   Harga Kamar                     |" << endl;
            cout << "|      [ 5 ]   Semua Data Kamar                |" << endl;
            cout << "================================================" << endl;
            cout << "Pilihan : ";
            cin >> pilihUbah;

            // Membersihkan buffer
            cin.ignore();
            switch (pilihUbah) // Menangani pilihan pengguna
            {
            case 1:
                cout << "Jenis Kamar yang Baru : ";
                getline(cin, temp->data.jenis_kamar);
                break;
            case 2:
                cout << "Nomor Kamar yang Baru : ";
                cin >> temp->data.no_kamar;
                break;
            case 3:
                cout << "Status Kamar yang Baru : ";
                getline(cin, temp->data.status);
                break;
            case 4:
                cout << "Harga Kamar yang Baru : ";
                cin >> temp->data.harga;
                break;
            case 5:
                cout << "Jenis Kamar yang Baru : ";
                getline(cin, temp->data.jenis_kamar);
                cout << "Nomor Kamar yang Baru : ";
                cin >> temp->data.no_kamar;
                cout << "Status Kamar yang Baru : ";
                getline(cin, temp->data.status);
                cout << "Harga Kamar yang Baru : ";
                cin >> temp->data.harga;
                break;
            default:
                cout << "Pilihan tidak valid.\n";
                return;
            }
            // Memberi konfirmasi bahwa data kamar telah diubah
            cout << "Berhasil Mengubah Data Kamar^^\n";
            return; // Keluar dari fungsi setelah mengubah data
        }
        temp = temp->next;
    }
    cout << "ID kamar tidak valid.\n";
}

//fungsi untuk menghapus data
void delMiddle(Node *&head, int position)
{
    // Memeriksa kevalidan posisi
    if (position <= 0 || position >= nextRoomId)
    {
        cout << "Posisi tidak valid.\n";
        return;
    }

    // Memeriksa apakah linked list kosong
    if (head == nullptr)
    {
        cout << "Pet Hotel Masih Kosong :((" << endl;
        return;
    }

    // Mencari node pada posisi yang ditentukan
    Node *temp = head;
    Node *prev = nullptr;

    for (int i = 1; i < position && temp != nullptr; ++i)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr) // Memeriksa kevalidan posisi
    {
        cout << "Posisi tidak valid.\n";
        return;
    }

    // Menghapus node pada posisi tertentu
    prev->next = temp->next; 
    delete temp;

    // Memberi konfirmasi bahwa data kamar telah dihapus
    cout << "Data kamar berhasil dihapus.\n";
    updateIds(head); // Memperbarui ID setiap node untuk menjamin urutan yang benar
    display(head);  // Menampilkan linked list setelah penghapusan
}

// Fungsi untuk menampilkan histori reservasi
void viewReservationHistory() {
    ifstream reservationFile("reservasi_info.csv");
    if (!reservationFile.is_open()) {
        cout << "Gagal membuka file reservasi_info.csv." << endl;
        return;
    }

    const int idResWidth = 13;
    const int usnWidth = 14;
    const int noKamarWidth = 12;
    const int waktuWidth = 25;

    cout << "=============================================================================" << endl;
    cout << "|                     H I S T O R I   P E M E S A N A N                     |" << endl;
    cout << "=============================================================================" << endl;
    cout << "| ID Reservasi  | Username       | Nomor Kamar  | Waktu Reservasi           |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;

    // Gunakan queue untuk menyimpan data reservasi
    queue<Reservation> reservationQueue;

    Reservation reservasi;
    while (reservationFile >> reservasi.reservationId) {
        // Membaca username sebagai string
        reservationFile.ignore(); // Mengabaikan koma setelah ID Reservasi
        getline(reservationFile, reservasi.username, ',');

        // Membaca nomor kamar sebagai int
        reservationFile >> reservasi.no_kamar;

        // Menangani newline setelah membaca nomor kamar
        reservationFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Mendapatkan waktu reservasi saat ini
        reservasi.timestamp = time(nullptr);

        // Menambahkan reservasi ke dalam queue
        reservationQueue.push(reservasi);

        // Jika ukuran queue melebihi 5, hapus yang paling awal
        if (reservationQueue.size() > 5) {
            reservationQueue.pop();
        }
    }

    // Menampilkan data reservasi dari queue
    while (!reservationQueue.empty()) {
        Reservation currentReservation = reservationQueue.front();
        reservationQueue.pop();

        // Mendapatkan string waktu reservasi
        string reservationTime = asctime(localtime(&currentReservation.timestamp));
        reservationTime.pop_back();  // Menghapus karakter newline terakhir

        // Menampilkan data reservasi
        cout << "| " << setw(idResWidth) << currentReservation.reservationId << " | "
             << setw(usnWidth) << currentReservation.username << " | "
             << setw(noKamarWidth) << currentReservation.no_kamar << " | "
             << setw(waktuWidth) << reservationTime << " |" << endl;
    }

    cout << "=============================================================================" << endl;

    reservationFile.close();
}

// Fungsi untuk membandingkan dua Node berdasarkan jenis kamar
bool compareByJenis(const Node &a, const Node &b) {
    return a.data.jenis_kamar < b.data.jenis_kamar;
}

// Fungsi untuk membandingkan dua Node berdasarkan nomor kamar
bool compareByNomorKamar(const Node &a, const Node &b) {
    return a.data.no_kamar < b.data.no_kamar;
}

// Fungsi untuk membandingkan dua Node berdasarkan harga kamar
bool compareByHarga(const Node &a, const Node &b) {
    return a.data.harga < b.data.harga;
}

// Fungsi pertukaran elemen
void swap(Node &a, Node &b) {
    Node temp = a;
    a = b;
    b = temp;
}

// Fungsi partisi untuk Quick Sort
int partition(Node *arr, int low, int high, bool compareFunction(const Node &a, const Node &b)) {
    Node pivot = arr[high];
    int i = (low - 1);

    // Memulai proses partisi
    for (int j = low; j <= high - 1; j++) {
        if (compareFunction(arr[j], pivot)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Fungsi Quick Sort untuk pengurutan array dari low ke high
void quickSort(Node *arr, int low, int high, bool compareFunction(const Node &a, const Node &b)) {
    if (low < high) {
        // Memperoleh posisi partisi
        int pi = partition(arr, low, high, compareFunction);

        // Memanggil rekursif untuk sub-array sebelum dan setelah partisi
        quickSort(arr, low, pi - 1, compareFunction);
        quickSort(arr, pi + 1, high, compareFunction);
    }
}

// Fungsi pengurutan berdasarkan jenis kamar
void sortListByName(Node *head) {
    // Memeriksa keberadaan data kamar
    if (head == nullptr) {
        cout << "Data Kamar Masih Kosong." << endl;
        return;
    }

    // Mengonversi linked list menjadi array untuk mempermudah pengurutan
    vector<Node> roomList;
    Node *current = head;
    while (current != nullptr) {
        roomList.push_back(*current);
        current = current->next;
    }

    // Menggunakan Quick Sort untuk mengurutkan array berdasarkan jenis kamar
    quickSort(roomList.data(), 0, roomList.size() - 1, compareByJenis);

    // Menampilkan data yang sudah diurutkan
    cout << "\nHasil Pengurutan Berdasarkan Jenis Kamar:" << endl;
    cout << "================================================================" << endl;
    cout << "| ID   | Jenis Kamar | Nomor Kamar | Status            | Harga    |" << endl;
    cout << "----------------------------------------------------------------" << endl;
    for (const Node &room : roomList) {
        cout << "| " << setw(4) << room.data.id << " | "
             << setw(11) << room.data.jenis_kamar << " | "
             << setw(11) << room.data.no_kamar << " | "
             << setw(17) << room.data.status << " | "
             << setw(8) << room.data.harga << " |" << endl;
    }
    cout << "================================================================" << endl;
}

// Fungsi pengurutan berdasarkan nomor kamar
void sortListByNumber(Node *head) {
    // Memeriksa keberadaan data kamar
    if (head == nullptr) {
        cout << "Data Kamar Masih Kosong." << endl;
        return;
    }

    // Mengonversi linked list menjadi array untuk mempermudah pengurutan
    vector<Node> roomList;
    Node *current = head;
    while (current != nullptr) {
        roomList.push_back(*current);
        current = current->next;
    }

    // Menggunakan Quick Sort untuk mengurutkan array berdasarkan nomor kamar
    quickSort(roomList.data(), 0, roomList.size() - 1, compareByNomorKamar);

    // Menampilkan data yang sudah diurutkan
    cout << "Hasil Pengurutan Berdasarkan Nomor Kamar:" << endl;
    cout << "================================================================" << endl;
    cout << "| ID   | Jenis Kamar | Nomor Kamar | Status            | Harga    |" << endl;
    cout << "----------------------------------------------------------------" << endl;
    for (const Node &room : roomList) {
        cout << "| " << setw(4) << room.data.id << " | "
             << setw(11) << room.data.jenis_kamar << " | "
             << setw(11) << room.data.no_kamar << " | "
             << setw(17) << room.data.status << " | "
             << setw(8) << room.data.harga << " |" << endl;
    }
    cout << "================================================================" << endl;
}

// Fungsi pengurutan berdasarkan harga kamar
void sortListByPrice(Node *head) {
    // Memeriksa keberadaan data kamar
    if (head == nullptr) {
        cout << "Data Kamar Masih Kosong." << endl;
        return;
    }

    // Mengonversi linked list menjadi array untuk mempermudah pengurutan
    vector<Node> roomList;
    Node *current = head;
    while (current != nullptr) {
        roomList.push_back(*current);
        current = current->next;
    }

    // Menggunakan Quick Sort untuk mengurutkan array berdasarkan harga kamar
    quickSort(roomList.data(), 0, roomList.size() - 1, compareByHarga);

    // Menampilkan data yang sudah diurutkan
    cout << "Hasil Pengurutan Berdasarkan Harga Kamar:" << endl;
    cout << "================================================================" << endl;
    cout << "| ID   | Jenis Kamar | Nomor Kamar | Status            | Harga    |" << endl;
    cout << "----------------------------------------------------------------" << endl;
    for (const Node &room : roomList) {
        cout << "| " << setw(4) << room.data.id << " | "
             << setw(11) << room.data.jenis_kamar << " | "
             << setw(11) << room.data.no_kamar << " | "
             << setw(17) << room.data.status << " | "
             << setw(8) << room.data.harga << " |" << endl;
    }
    cout << "================================================================" << endl;
}

// Fungsi sortingMenu untuk memudahkan pengguna memilih jenis pengurutan
void sortingMenu(Node *head) {
    char sortChoice; 
    do {
        system("cls"); // Membersihkan layar konsol
        cout << "======================================" << endl;
        cout << "|       PILIH JENIS PENGURUTAN       |" << endl;
        cout << "--------------------------------------" << endl;
        cout << "|   [ 1 ] Berdasarkan Jenis Kamar    |" << endl;
        cout << "|   [ 2 ] Berdasarkan Nomor Kamar    |" << endl;
        cout << "|   [ 3 ] Berdasarkan Harga Kamar    |" << endl;
        cout << "|   [ 0 ] Kembali ke Menu Utama      |" << endl;
        cout << "--------------------------------------" << endl;
        cout << "Pilihan Anda: ";
        cin >> sortChoice;

        switch (sortChoice) {
            case '1':
                sortListByName(head);
                break;

            case '2':
                sortListByNumber(head);
                break;

            case '3':
                sortListByPrice(head);
                break;

            case '0':
                // Kembali ke menu utama
                break;

            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
                break;
        }

        system("pause"); // Tunggu hingga pengguna menekan tombol sebelum melanjutkan
    } while (sortChoice != '0');
}

// Fungsi untuk menginisialisasi array badchar dengan informasi terakhir kemunculan karakter dalam pola
void badCharHeuristic(const string &str, int length, int badchar[256])
{
    for (int i = 0; i < 256; i++)
    {
        badchar[i] = -1; // Menginisialisasi semua entri dengan -1
    }

    for (int i = 0; i < length; i++)
    {
        badchar[(int)str[i]] = i;
    }
}

// Fungsi pencarian kamar berdasarkan jenis menggunakan algoritma Boyer-Moore
Node *searchByJenis(const string &pattern, Node *head)
{
    Node *current;
    for (current = head; current != NULL; current = current->next)
    {
        int n = current->data.jenis_kamar.length();
        int badchar[256];
        badCharHeuristic(current->data.jenis_kamar, n, badchar);

        int s = 0;
        int j = n - 1;
        while (j >= 0 && pattern[j] == current->data.jenis_kamar[s + j])
        {
            j--;
        }

        if (j < 0)
        {
            return current; // Jika pola cocok, kembalikan node saat ini
        } 

        int x = (s + j < n) ? j - badchar[(int)current->data.jenis_kamar[s + j]] : 1;
        s += max(1, x);
    }
    return NULL; // Jika tidak ditemukan, kembalikan NULL
}

// Fungsi pencarian kamar berdasarkan nomor kamar menggunakan algoritma Jump Search
Node *jumpSearchByNumber(Node *head, const int &key)
{
    int n = 0;
    Node *temp = head;
    while (temp)
    {
        n++;
        temp = temp->next;
    }

    int step = sqrt(n);
    Node *current = head;
    temp = nullptr; // Initialize temp to nullptr

    while (current && current->data.no_kamar < key)
    {
        temp = current;
        for (int i = 0; i < step && current; i++)
        {
            temp = current;
            current = current->next;
        }
    }

    if (!current || current->data.no_kamar != key)
        return nullptr;

    return current; // Jika ditemukan, kembalikan node saat ini
}

// Fungsi pencarian kamar berdasarkan harga menggunakan algoritma Jump Search
Node *jumpSearchByPrice(Node *head, double key)
{
    int n = 0;
    Node *temp = head;
    while (temp)
    {
        n++;
        temp = temp->next;
    }

    int step = sqrt(n);
    Node *current = head;

    while (current && current->data.harga < key)
    {
        temp = current;
        for (int i = 0; i < step && current; i++)
        {
            temp = current;
            current = current->next;
        }
    }

    if (!current || current->data.harga != key)
        return NULL;

    return current; // Jika ditemukan, kembalikan node saat ini
}

// Menampilkan hasil pencarian kamar
void displaySearchResult(const string &searchType, Node *result)
{
    Node *current = head;
    if (result)
    {
        // Lakukan sesuatu dengan hasil pencarian, misalnya, tampilkan informasinya.
        cout << "Data ditemukan: " << searchType << endl;
        cout << "Data tambahan:" << endl;
                    cout << "================================================================" << endl;
                    cout << "| ID   | Jenis Kamar | Nomor Kamar | Status            | Harga    |" << endl;
                    cout << "----------------------------------------------------------------" << endl;
                    cout << "| " << setw(4) << current->data.id << " | "
                         << setw(11) << current->data.jenis_kamar << " | "
                         << setw(11) << current->data.no_kamar << " | "
                         << setw(17) << current->data.status << " | "
                         << setw(8) << current->data.harga << " |" << endl;
                    cout << "================================================================" << endl;

        // Menampilkan informasi tambahan berdasarkan jenis pencarian
        if (searchType == "Jenis Kamar")
        {
            cout << result->data.jenis_kamar << endl;
        }
        else if (searchType == "Nomor Kamar")
        {
            cout << result->data.no_kamar << endl;
        }
        else if (searchType == "Harga Kamar")
        {
            cout << result->data.harga << endl;
        }
    }
    else
    {
        cout << "Data tidak ditemukan." << endl;
    }
}

// Menampilkan informasi pengguna dari file CSV
void viewUsers()
{
    ifstream file("user.csv");
    if (!file.is_open())
    {
        cout << "Gagal membuka file pengguna." << endl;
        return;
    }

    cout << "Data Pengguna:" << endl;
    cout << "========================" << endl;

    User user;
    while (file >> user.username >> user.password)
    {
        cout << "Username: " << user.username << endl;
        cout << "Password: " << user.password << endl;
        cout << "========================" << endl;
    }

    file.close();
}

// Memeriksa apakah username sudah ada dalam file CSV
bool isUsernameExists(string username)
{
    ifstream file("user.csv");
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string data;
        getline(ss, data, ',');
        if (data == username)
        {
            return true;
        }
    }
    return false;
}

// Fungsi untuk registrasi pengguna baru
void regis() {
    string ruserId, rpassword, rid, rpass;
    system("cls");
    cout << "==============================================\n";
    cout << "|              FORM  REGISTRASI              |\n";
    cout << "----------------------------------------------\n";
    cout << "||      ANDA AKAN MELAKUKAN REGISTRASI      ||\n";
    cout << "==============================================\n";
    cout << "Masukkan username : ";
    cin >> ruserId;
    cout << "Masukkan password : ";
    cin >> rpassword;
    bool isExists = isUsernameExists(ruserId);
    if (isExists)
    {
        cout << "Username already exists." << endl;
    }
    else
    {
        ofstream f1("user.csv", ios::app);
        f1 << ruserId << ' ' << rpassword << endl;
        cout << "Registrasi Berhasil\n";
        cout << "Silahkan ke menu login pelanggan^^\n";
        getch(); // Menunggu hingga pengguna menekan tombol sebelum melanjutkan
    }
}

// Fungsi untuk menu utama pelanggan
void user(string username)
{
    int menu_pilihan, idx, key, cari;
    string pilih;
    do
    {
        Sleep(1000);
        system("cls");
        cout << "====================================================\n";
        cout << "||    ^^ SELAMAT DATANG PELANGGAN SETIA KAMI ^^   ||\n";
        cout << "====================================================\n";
        cout << "====================================================\n";
        cout << "|        PILIHLAH  MENU  PILIHAN  BERIKUT :        |\n";
        cout << "----------------------------------------------------\n";
        cout << "||       [ 1 ]  Melihat Daftar Kamar              ||\n";
        cout << "||       [ 2 ]  Mencari Data Kamar                ||\n";
        cout << "||       [ 3 ]  Membuat Reservasi                 ||\n";
        cout << "||       [ 4 ]  Kembali                           ||\n";
        cout << "====================================================\n";
        cout << "Masukkan menu pilihan : ";
        cin >> menu_pilihan;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Inputan harus berupa angka." << endl;
            Sleep(2000);
            // user();
        }
        switch (menu_pilihan)
        {
        case 1:
            display(head);
            char sortChoice;
            cout << "Apakah Anda ingin melakukan pengurutan? (y/n): ";
            cin >> sortChoice;

            if (sortChoice == 'y' || sortChoice == 'Y')
            {
                sortingMenu(head); // Panggil fungsi untuk menu pengurutan
            }
            else if (sortChoice == 'n' || sortChoice == 'N')
            {
                // Kembali ke menu utama
            }
            else
            {
                cout << "Pilihan tidak valid. Kembali ke menu utama." << endl;
                // Kembali ke menu utama
            }
            break;
        case 2:
            cout << "1. Jenis Kamar" << endl;
            cout << "2. Nomor Kamar" << endl;
            cout << "3. Harga Kamar" << endl;
            cout << "Pilih : ";
            int pilih;
            cin >> pilih;
            cin.ignore(); // Membersihkan karakter newline yang tersisa dalam buffer

            if (pilih == 1)
            {
                string cari;
                cout << "Cari jenis: ";
                getline(cin, cari); // Menggunakan getline untuk membaca jenis kamar yang mengandung spasi
                Node *result = searchByJenis(cari, head);
                displaySearchResult("Jenis Kamar", result);
                system("pause");
            }
            else if (pilih == 2)
            {
                int cari;
                cout << "Cari nomor: ";
                cin >> cari;
                Node *result = jumpSearchByNumber(head, cari);
                displaySearchResult("Nomor Kamar", result);
                system("pause");
            }
            else if (pilih == 3)
            {
                int cari;
                cout << "Cari harga: ";
                cin >> cari;
                Node *result = jumpSearchByPrice(head, cari);
                displaySearchResult("Harga Kamar", result);
                system("pause");
            }

            break;
        case 3:
            cout << "Masukkan Nama Anda : ";
            cin >> username;
            createReservation(head, username);
            break;
        case 4:
            // Kembali ke menu utama
            break;
        default:
            cout << "Hanya dapat memilih salah satu menu diatas" << endl;
            Sleep(1000);
            break;
        }
    } while (menu_pilihan != 4);
}

// Fungsi untuk login pengguna
void login_user()
{
    int count = 0;
    string userId, password, id, pass;
    system("cls");
    cout << "==============================================================" << endl;
    cout << "||              L O G I N    P E L A N G G A N              ||" << endl;
    cout << "--------------------------------------------------------------" << endl;
    cout << "||             SILAHKAN LOGIN  TERLEBIH DAHULU              ||" << endl;
    cout << "==============================================================" << endl;

    do
    { // Loop hingga pengguna berhasil login atau melebihi batasan percobaan
        cout << "Masukkan username : ";
        cin >> userId;
        cout << "Masukkan password : ";
        cin >> password;

        ifstream input("user.csv");
        bool found = false; // Tambahkan variabel boolean untuk mengecek keberhasilan login

        while (input >> id >> pass)
        {
            if (id == userId && pass == password)
            {
                cout << userId << "\nLogin Berhasil\nThanks for logging in!\n";
                input.close();
                user(userId); // Panggil fungsi untuk menu pengguna dengan parameter username
                return;       // Keluar dari fungsi setelah login berhasil
            }
        }

        input.close();

        if (!found)
        {
            cout << "Username dan/atau password salah. Coba lagi.\n";
            count++; // Tambahkan jumlah percobaan yang gagal
        }

        if (count >= 3)
        {
            cout << "Anda telah mencoba login sebanyak 3 kali.\n";
            cout << "Kembali ke menu utama.\n";
            // Keluar dari loop jika mencapai batasan percobaan
            break;
        }
    } while (count < 3);

    // Program akan kembali ke int main setelah batasan percobaan
}

// fungsi untuk menu utama admmin
void admin()
{
    // Node *head = nullptr;
    int pilihan, key, idx, cari;
    string pilih;
    do
    {
        system("cls");
        cout << "=============================================================" << endl;
        cout << "||                ^^ SELAMAT DATANG ADMIN ^^               ||" << endl;
        cout << "=============================================================" << endl;
        cout << "|          P I L I H    M E N U    B E R I K U T :          |" << endl;
        cout << "-------------------------------------------------------------" << endl;
        cout << "||       [ 1 ]   Melihat Data User                         ||" << endl;
        cout << "||       [ 2 ]   Menambah Data Kamar                       ||" << endl;
        cout << "||       [ 3 ]   Melihat Data Kamar                        ||" << endl;
        cout << "||       [ 4 ]   Memperbarui Data Kamar                    ||" << endl;
        cout << "||       [ 5 ]   Menghapus Data Kamar                      ||" << endl;
        cout << "||       [ 6 ]   Mencari Data Kamar                        ||" << endl;
        cout << "||       [ 7 ]   Melihat Histori Pemesanan Kamar           ||" << endl;
        cout << "||       [ 0 ]   Kembali                                   ||" << endl;
        cout << "=============================================================" << endl;
        cout << "Masukkan pilihan menu : ";
        cin >> pilihan;
         if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Inputan harus berupa angka." << endl;
            Sleep(2000);
            admin();
        }
        
        switch (pilihan)
        {
        case 1:
            viewUsers();
            break;
        case 2:
        {
            room dataRoom = readRoomData();
            create(head, dataRoom);
            break;
        }
        case 3:
            if (head == nullptr) {
                cout << "Data Kamar Masih Kosong." << endl;
                } else {
                    display(head);
                    char sortChoice;
                    cout << "Apakah Anda ingin melakukan pengurutan? (y/n): ";
                    cin >> sortChoice;

                    if (sortChoice == 'y' || sortChoice == 'Y') {
                        sortingMenu(head); // Panggil fungsi untuk menu pengurutan
                    } else if (sortChoice != 'n' && sortChoice != 'N') {
                        cout << "Pilihan tidak valid. Kembali ke menu utama." << endl;
                        // Kembali ke menu utama
                    }
                }
            break;
        case 4:
            update(head);
            display(head);
            break;
        case 5:
        {
            int position;
            cout << "Masukkan ID kamar yang ingin dihapus (1 - " << nextRoomId - 1 << "): ";
            cin >> position;
            delMiddle(head, position);
            break;
        }
        case 6:
            cout << "1. Jenis Kamar" << endl;
            cout << "2. Nomor Kamar" << endl;
            cout << "3. Harga Kamar" << endl;
            cout << "Pilih : ";
            int pilih;
            cin >> pilih;
            cin.ignore(); // Membersihkan karakter newline yang tersisa dalam buffer

            if (pilih == 1)
            {
                string cari;
                cout << "Cari jenis: ";
                getline(cin, cari); // Menggunakan getline untuk membaca jenis kamar yang mengandung spasi
                Node *result = searchByJenis(cari, head);
                displaySearchResult("Jenis Kamar", result);
                system("pause");
            }
            else if (pilih == 2)
            {
                int cari;
                cout << "Cari nomor: ";
                cin >> cari;
                Node *result = jumpSearchByNumber(head, cari);
                displaySearchResult("Nomor Kamar", result);
                system("pause");
            }
            else if (pilih == 3)
            {
                int cari;
                cout << "Cari harga: ";
                cin >> cari;
                Node *result = jumpSearchByPrice(head, cari);
                displaySearchResult("Harga Kamar", result);
                system("pause");
            }

            break;
        case 7:
            viewReservationHistory();
            break;
        default:
            cout << "Pilihan tidak valid, silahkan pilih sesuai menu yang tersedia" << endl;
        }
        system("pause");
    } while (pilihan != 0);
}

//fungsi login untuk admin
int login_admin()
{
    string username, password;
    int attempts = 0;
    system("cls");
    cout << "==============================================================" << endl;
    cout << "||                  L O G I N    A D M I N                  ||" << endl;
    cout << "--------------------------------------------------------------" << endl;
    cout << "||             SILAHKAN LOGIN  TERLEBIH DAHULU              ||" << endl;
    cout << "==============================================================" << endl;

    while (attempts < 3)
    {
        cout << "Masukkan Username : ";
        getline(cin >> ws, username);
        cout << "Masukkan Password : ";
        getline(cin >> ws, password);

        if (username == "admin" && password == "admin")
        {
            cout << "Berhasil Login! " << endl;
            cout << "Welcome, " << username << "!" << endl;
            Sleep(2000);
            break;
        }
        else
        {
            attempts++; // percobaan
            cout << "Username atau password salah. Anda telah gagal " << attempts << " kali." << endl;
            if (attempts >= 3)
            {
                cout << "Anda telah gagal login sebanyak 3 kali. Silakan coba lagi nanti." << endl;
                return 0;
            }
        }
    }
    admin();
}

//fungsi untuk memproses login
void login()
{
    int pilih;
    do 
    {
        system("CLS");
        cout << "================================" << endl;
        cout << "|          FORM LOGIN          |" << endl;
        cout << "================================" << endl;
        cout << "|    [ 1 ] Login Admin         |" << endl;
        cout << "|    [ 2 ] Login Pengguna      |" << endl;
        cout << "|    [ 3 ] Kembali             |" << endl;
        cout << "================================" << endl;
        cout << "Pilihan : ";
        cin >> pilih;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Inputan harus berupa angka." << endl;
            Sleep(2000);
            login(); // Jika input bukan angka, panggil kembali fungsi login
        }
        switch (pilih)
        {
        case 1:
            login_admin(); // Panggil fungsi login admin
            break;
        case 2:
            login_user(); // Panggil fungsi login pengguna
            break;
        case 3:
            cout << "Kembali ke menu sebelumnya" << endl;
            Sleep(1000);
            break;
        default:
            cout << "Hanya dapat memilih salah satu menu diatas" << endl;
            Sleep(1000);
            break;
        }
    } while (pilih != 3);
}

//fungsi utama program
int main()
{
    int pilih;
    do
    {
        system("cls");
        cout << "==============================================================" << endl;
        cout << "||               PROYEK AKHIR STRUKTUR DATA                 ||" << endl;
        cout << "||                       KELOMPOK 3                         ||" << endl;
        cout << "||               SISTEM MANAJEMEN PENGINAPAN                ||" << endl;
        cout << "==============================================================" << endl;
        cout << "||                   Pilih Menu Dibawah                     ||" << endl;
        cout << "--------------------------------------------------------------" << endl;
        cout << "|               [ 1 ]   Registrasi                           |" << endl;
        cout << "|               [ 2 ]   Login                                |" << endl;
        cout << "|               [ 3 ]   Keluar Program                       |" << endl;
        cout << "==============================================================" << endl;
        cout << "Pilih Menu : ";
        cin >> pilih;
         if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Inputan harus berupa angka." << endl;
            Sleep(2000);
            main(); // Jika input bukan angka, panggil kembali fungsi main
        }
        switch (pilih)
        {
        case 1:
            regis(); // Panggil fungsi registrasi
            break;
        case 2:
            login(); // Panggil fungsi login
            break;
        case 3:
            cout << "Keluar dari program" << endl;
            break;
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            break;
        }
        system("pause"); // Tunggu hingga pengguna menekan tombol sebelum melanjutkan
    } while (pilih != 3);
    cout << "Program selesai";
    return 0;
}