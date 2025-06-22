#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =====================================================================
// --- BAGIAN PENDEFINISIAN DAN INISIASI STRUCT ---
// =====================================================================

#define MAX_DOKTER 500 // Bebas diubah asal jangan kurang dari jumlah dokter yang ada di .CSV

typedef enum
{
    PAGI,
    SIANG,
    MALAM
} TipeShift;

typedef struct
{
    int id;
    char nama[50];
    char preferensi[100];
    int total_shift_dijadwalkan;
    int pelanggaran_preferensi_terjadi;
} Dokter;

typedef struct
{
    int hari;
    TipeShift tipe;
    int id_dokter;
} Shift;

struct datadokter
{
    char *nama;
    int shift_awal;
    int shift;
    char *preferensi;
    struct datadokter *next;
} *head = NULL, *tail = NULL, *current;

// =====================================================================
// --- BAGIAN OJAN & DZIKRI ---
// =====================================================================

void tambahNode(char *nama, int shift, char *preferensi)
{
    current = (struct datadokter *)malloc(sizeof(struct datadokter));
    current->nama = (char *)malloc(strlen(nama) + 1);
    current->preferensi = (char *)malloc(strlen(preferensi) + 1);
    strcpy(current->nama, nama);
    strcpy(current->preferensi, preferensi);
    current->shift_awal = current->shift = shift;
    current->next = NULL;

    if (head == NULL)
    {
        head = tail = current;
    }
    else
    {
        tail->next = current;
        tail = current;
    }
}

void readFile()
{
    FILE *fptr = fopen("Data_Dokter.csv", "r");
    if (!fptr)
    {
        printf("File tidak dapat dibuka. \n");
        return;
    }
    char buf[256];
    fgets(buf, sizeof(buf), fptr);fgets(buf, sizeof(buf), fptr);
    while (fgets(buf, sizeof(buf), fptr))
    {
        if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0';
        char *nama = strtok(buf, ",");
        char *shiftStr = strtok(NULL, ",");
        char *pref = strtok(NULL, ",");
        if (nama && shiftStr && pref)
        {
            tambahNode(nama, atoi(shiftStr), pref);
        }
    }
    fclose(fptr);
}

void tampilkanDokter()
{
    struct datadokter *curr = head;
    int i = 1;
    while (curr != NULL)
    {
        printf("%d. Dr. %s - Maks: %d/minggu, Pref: %s\n", i++, curr->nama, curr->shift_awal, curr->preferensi);
        curr = curr->next;
    }
}

void bersihkanMemori()
{
    struct datadokter *curr = head;
    while (curr != NULL)
    {
        struct datadokter *next = curr->next;
        free(curr->nama);
        free(curr->preferensi);
        free(curr);
        curr = next;
    }
}

void resetShiftMingguan()
{
    struct datadokter *curr = head;
    while (curr != NULL)
    {
        curr->shift = curr->shift_awal;
        curr = curr->next;
    }
}

void acakUrutanDokter() {
    if (head == NULL || head->next == NULL) return;
    
    // Hitung jumlah dokter
    int count = 0;
    struct datadokter *curr = head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    
    // Konversi linked list ke array
    struct datadokter **arr = malloc(count * sizeof(struct datadokter));
    curr = head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1); // rand() untuk mengacak indeks dokter 
        struct datadokter *temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    // Buat linked list kembali dari array yang sudah diacak
    head = arr[0];
    tail = arr[count-1];
    for (int i = 0; i < count-1; i++) {
        arr[i]->next = arr[i+1];
    }
    arr[count-1]->next = NULL;
    
    free(arr);
}

int cekPreferensi(const char *preferensi_dokter, const char *shift)
{
    return (strstr(preferensi_dokter, shift) != NULL);
}

void jadwalotomatis30hari(char jadwal[30][3][6][100]) {
    char *shift[] = {"pagi", "siang", "malam"};
    int maks[] = {6, 6, 5}; // Maksimal jumlah dokter per shift (pagi dan siang 6 dokter, malam 5 dokter)
    char pola_mingguan[7][3][6][100] = {{{{0}}}}; // 
    
    // Array untuk mengecek apakah sudah terdapat dokter di setiap shift
    char dokter_per_shift[3][100][100];
    int jumlah_dokter_per_shiift[3] = {0};

    // Assign jadwal dokter untuk 7 hari pertama
    for (int hari = 0; hari < 7; hari++) {
        acakUrutanDokter();
        
        // Inisialisasi jumlah dokter di setiap shiftnya masih 0
        for (int s = 0; s < 3; s++) {
            jumlah_dokter_per_shiift[s] = 0;
        }

        int count[3] = {0};
        
        // Alokasi berdasarkan preferensi shift dokter
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                // Cek apakah dokter sudah bekerja di shift ini hari ini
                int sudah_kerja = 0;
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0 && cekPreferensi(curr->preferensi, shift[s])) {
                    strcpy(pola_mingguan[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
        
        // Alokasi sisa slot dengan dokter yang belum kerja di shift ini
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                int sudah_kerja = 0;
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0) {
                    strcpy(pola_mingguan[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
    }

    // Pola repetitif per minggu untuk 4 minggu (28 hari)
    for (int minggu = 0; minggu < 4; minggu++) {
        for (int hari = 0; hari < 7; hari++) {
            int hari_ini = minggu * 7 + hari;
            if (hari_ini >= 30) break;
            
            for (int s = 0; s < 3; s++) {
                for (int d = 0; d < maks[s]; d++) {
                    strcpy(jadwal[hari_ini][s][d], pola_mingguan[hari][s][d]);
                }
            }
        }
    }
    resetShiftMingguan(); // Untuk mereset minggu berikutnya

    // Untuk sisa 2 hari
    for (int hari = 28; hari < 30; hari++) {
        acakUrutanDokter();
        
        // Inisialisasi jumlah dokter di setiap shiftnya masih 0
        for (int s = 0; s < 3; s++) {
            jumlah_dokter_per_shiift[s] = 0;
        }

        int count[3] = {0};
        
        // Alokasi berdasarkan preferensi shift dokter
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                // Cek apakah dokter sudah bekerja di shift ini hari ini
                int sudah_kerja = 0; 
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0 && cekPreferensi(curr->preferensi, shift[s])) {
                    strcpy(jadwal[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
        
        // Alokasi sisa slot dengan dokter yang belum kerja di shift ini
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                int sudah_kerja = 0;
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0) {
                    strcpy(jadwal[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
    }
}

// =====================================================================
// --- BAGIAN ADHI & LUNA ---
// =====================================================================

void bersihkanBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int konversiDataDokter(Dokter daftarDokter[])
{
    int jumlahDokter = 0;
    for (struct datadokter *node = head; node != NULL && jumlahDokter < MAX_DOKTER; node = node->next, jumlahDokter++)
    {
        daftarDokter[jumlahDokter].id = jumlahDokter;
        strcpy(daftarDokter[jumlahDokter].nama, node->nama);
        strcpy(daftarDokter[jumlahDokter].preferensi, node->preferensi);
        daftarDokter[jumlahDokter].total_shift_dijadwalkan = 0;
        daftarDokter[jumlahDokter].pelanggaran_preferensi_terjadi = 0;
    }
    return jumlahDokter;
}

int konversiJadwal(char jadwalOtomatis[30][3][6][100], Shift jadwalFinal[], Dokter daftarDokter[], int jumlahDokter)
{
    int totalShiftTerisi = 0;
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    int max_dokter_per_shift_arr[] = {6, 6, 5};

    for (int i = 0; i < jumlahDokter; i++)
    {
        daftarDokter[i].total_shift_dijadwalkan = 0;
        daftarDokter[i].pelanggaran_preferensi_terjadi = 0;
    }

    for (int h = 0; h < 30; h++)
    {
        for (int s = 0; s < 3; s++)
        {
            for (int d = 0; d < max_dokter_per_shift_arr[s]; d++)
            {
                if (strlen(jadwalOtomatis[h][s][d]) > 0)
                {
                    char *namaDokter = jadwalOtomatis[h][s][d];
                    int idDokter = -1;
                    for (int i = 0; i < jumlahDokter; i++)
                    {
                        if (strcmp(daftarDokter[i].nama, namaDokter) == 0)
                        {
                            idDokter = i;
                            break;
                        }
                    }
                    if (idDokter != -1)
                    {
                        jadwalFinal[totalShiftTerisi].hari = h + 1;
                        jadwalFinal[totalShiftTerisi].tipe = (TipeShift)s;
                        jadwalFinal[totalShiftTerisi].id_dokter = idDokter;
                        daftarDokter[idDokter].total_shift_dijadwalkan++;
                        if (!cekPreferensi(daftarDokter[idDokter].preferensi, shiftLabel[s]))
                        {
                            daftarDokter[idDokter].pelanggaran_preferensi_terjadi++;
                        }
                        totalShiftTerisi++;
                    }
                }
            }
        }
    }
    return totalShiftTerisi;
}

const char *getNamaShift(TipeShift tipe)
{
    switch (tipe)
    {
    case PAGI:
        return "Pagi";
    case SIANG:
        return "Siang";
    case MALAM:
        return "Malam";
    default:
        return "N/A";
    }
}

void tampilkanJadwalHarian(int hari, const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[])
{
    if (hari < 1 || hari > 30)
    {
        printf("Hari tidak valid.\n");
        return;
    }
    printf("\n--- Jadwal Jaga Hari ke-%d ---\n", hari);
    for (int s = 0; s < 3; s++)
    {
        printf("Shift %-5s : ", getNamaShift((TipeShift)s));
        int count = 0;
        for (int i = 0; i < totalShiftTerisi; i++)
        {
            if (jadwal[i].hari == hari && jadwal[i].tipe == (TipeShift)s)
            {
                printf("%s; ", daftarDokter[jadwal[i].id_dokter].nama);
                count++;
            }
        }
        if (count == 0)
            printf("(Kosong)");
        printf("\n");
    }
}

void tampilkanJadwalMingguan(int minggu, const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[])
{
    if (minggu < 1 || minggu > 5)
    {
        printf("Minggu tidak valid.\n");
        return;
    }
    int hariMulai = (minggu - 1) * 7 + 1;

    int hariSelesai;
    if (hariMulai + 6 > 30)
    {
        hariSelesai = 30;
    }
    else
    {
        hariSelesai = hariMulai + 6;
    }

    printf("\n\n===============================================");
    printf("\n--- JADWAL MINGGU KE-%d (HARI %d s/d %d) ---", minggu, hariMulai, hariSelesai);
    printf("\n===============================================\n");

    for (int h = hariMulai; h <= hariSelesai; h++)
    {
        if (h > 30)
            break;
        tampilkanJadwalHarian(h, jadwal, totalShiftTerisi, daftarDokter);
    }
}

void tampilkanJadwalSebulan(const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[])
{
    printf("\n\n*************************************************");
    printf("\n*** TAMPILAN JADWAL SEBULAN PENUH ***");
    printf("\n*************************************************\n");
    for (int m = 1; m <= 5; m++)
    {
        int hariMulai = (m - 1) * 7 + 1;
        if (hariMulai > 30)
            break;
        tampilkanJadwalMingguan(m, jadwal, totalShiftTerisi, daftarDokter);
    }
}

void tampilkanLaporanPelanggaran(const Dokter daftarDokter[], int jumlahDokter)
{
    printf("\n--- Laporan Detail Pelanggaran Preferensi Shift ---\n");
    int totalPelanggaran = 0;
    int dokterdilanggar = 0;
    for (int i = 0; i < jumlahDokter; i++)
    {
        if (daftarDokter[i].pelanggaran_preferensi_terjadi > 0)
        {
            printf("Dr. %-20s | Jumlah Pelanggaran: %d\n",
                   daftarDokter[i].nama,
                   daftarDokter[i].pelanggaran_preferensi_terjadi);
            totalPelanggaran += daftarDokter[i].pelanggaran_preferensi_terjadi;
            dokterdilanggar++;
        }
    }
    if (dokterdilanggar == 0)
    {
        printf("Tidak ditemukan pelanggaran preferensi dalam jadwal ini.\n");
    }
    else
    {
        printf("---------------------------------------------------\n");
        printf("Total dokter yang melanggar: %d\n", dokterdilanggar);
        printf("Total semua pelanggaran: %d\n", totalPelanggaran);
    }
}

void simpanJadwalKeCSV(const char *namaFile, const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[])
{
    FILE *fptr = fopen(namaFile, "w");
    if (!fptr)
    {
        printf("ERROR: Tidak dapat membuat file '%s' untuk penulisan.\n", namaFile);
        return;
    }

    printf("\nMenyimpan jadwal ke file '%s' dalam format data mentah...", namaFile);

    fprintf(fptr, "Hari,Tipe_Shift,Nama_Dokter\n");

    for (int i = 0; i < totalShiftTerisi; i++)
    {
        int hari = jadwal[i].hari;
        const char *namaShift = getNamaShift(jadwal[i].tipe);
        int idDokter = jadwal[i].id_dokter;
        
        if (idDokter >= 0) 
        {
            const char *namaDokter = daftarDokter[idDokter].nama;
            
            fprintf(fptr, "%d,%s,\"%s\"\n", hari, namaShift, namaDokter);
        }
    }

    fclose(fptr);
    printf(" Berhasil.\n");
}

void cariJadwalDokter(const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[], int jumlahDokter)
{
    char namaCari[100];
    int idDokter = -1;
    int shiftDitemukan = 0;

    printf("\nMasukkan nama lengkap dokter yang ingin dicari: ");
    scanf(" %[^\n]", namaCari);
    bersihkanBuffer();

    for (int i = 0; i < jumlahDokter; i++)
    {
        if (strcmp(daftarDokter[i].nama, namaCari) == 0)
        {
            idDokter = daftarDokter[i].id;
            break;
        }
    }

    if (idDokter == -1)
    {
        printf("Dokter dengan nama '%s' tidak ditemukan dalam data.\n", namaCari);
        return;
    }

    printf("\n--- Jadwal Jaga Lengkap untuk Dr. %s ---\n", daftarDokter[idDokter].nama);
    for (int i = 0; i < totalShiftTerisi; i++)
    {
        if (jadwal[i].id_dokter == idDokter)
        {
            printf(" -> Hari ke-%-2d, Shift %s\n", jadwal[i].hari, getNamaShift(jadwal[i].tipe));
            shiftDitemukan++;
        }
    }

    if (shiftDitemukan == 0)
    {
        printf("Dr. %s tidak memiliki jadwal jaga pada bulan ini.\n", daftarDokter[idDokter].nama);
    }
    else
    {
        printf("----------------------------------------------\n");
        printf("Total shift bulan ini: %d\n", shiftDitemukan);
    }
}

// =====================================================================
// --- FUNGSI UTAMA (MAIN) & KONTROL MENU ---
// =====================================================================

void Menu()
{
    printf("\n=========================================");
    printf("\n|   Sistem Penjadwalan Dokter Otomatis  |");
    printf("\n=========================================\n");
    printf("1. Tampilkan Daftar Dokter\n");
    printf("2. Buat Jadwal Otomatis Baru\n");
    printf("-----------------------------------------\n");
    printf("3. Tampilkan Jadwal Harian\n");
    printf("4. Tampilkan Jadwal Mingguan\n");
    printf("5. Tampilkan Jadwal Sebulan Penuh\n");
    printf("6. Cari Jadwal Dokter\n");
    printf("-----------------------------------------\n");
    printf("7. Tampilkan Laporan Pelanggaran Preferensi Shift\n");
    printf("8. Simpan Jadwal ke File CSV\n");
    printf("-----------------------------------------\n");
    printf("0. Keluar\n");
    printf("=========================================\n");
}

int main()
{
    char jadwalOtomatis[30][3][6][100] = {0};
    Dokter daftarDokter[MAX_DOKTER];
    Shift jadwalFinal[30 * 3 * 6];
    int jumlahDokter = 0;
    int totalShiftTerisi = 0;
    int pilihan = -1;
    int jadwalSudahDibuat = 0;
    char namaFileInput[100];

    printf("=========================================\n");
    printf("|   Selamat Datang di Program Penjadwalan Dokter  |\n");
    printf("=========================================\n");

    do
    {
        printf("\nMasukkan nama file sumber data dokter (contoh: datadokter.csv): ");
        scanf(" %[^\n]", namaFileInput);
        bersihkanBuffer();
        readFile();

        if (head == NULL)
        {
            printf("[!] Gagal memuat data. Pastikan nama file benar dan file tidak kosong. Silakan coba lagi.\n");
        }

    } while (head == NULL);

    jumlahDokter = konversiDataDokter(daftarDokter);

    printf("\nData %d dokter berhasil dimuat.\n", jumlahDokter);

    do
    {
        Menu();
        printf(">> Pilihan Anda: ");
        if (scanf("%d", &pilihan) != 1)
        {
            printf("[!] Input tidak valid. Harap masukkan angka.\n");
            bersihkanBuffer();
            continue;
        }
        bersihkanBuffer();

        switch (pilihan)
        {
        case 1:
            tampilkanDokter();
            break;
        case 2:
            printf("\nMembuat jadwal baru...\n");
            jadwalotomatis30hari(jadwalOtomatis);
            totalShiftTerisi = konversiJadwal(jadwalOtomatis, jadwalFinal, daftarDokter, jumlahDokter);
            jadwalSudahDibuat = 1;
            printf("Jadwal telah berhasil dibuat.\n");
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            if (jadwalSudahDibuat)
            {
                if (pilihan == 3)
                {
                    int hari;
                    printf("Masukkan hari (1-30): ");
                    scanf("%d", &hari);
                    bersihkanBuffer();
                    tampilkanJadwalHarian(hari, jadwalFinal, totalShiftTerisi, daftarDokter);
                }
                else if (pilihan == 4)
                {
                    int minggu;
                    printf("Masukkan minggu (1-5): ");
                    scanf("%d", &minggu);
                    bersihkanBuffer();
                    tampilkanJadwalMingguan(minggu, jadwalFinal, totalShiftTerisi, daftarDokter);
                }
                else if (pilihan == 5)
                {
                    tampilkanJadwalSebulan(jadwalFinal, totalShiftTerisi, daftarDokter);
                }
                else if (pilihan == 6)
                {
                    cariJadwalDokter(jadwalFinal, totalShiftTerisi, daftarDokter, jumlahDokter);
                }
                else if (pilihan == 7)
                {
                    tampilkanLaporanPelanggaran(daftarDokter, jumlahDokter);
                }
                else if (pilihan == 8)
                {
                    char namaFileOutput[100];
                    printf("\nMasukkan nama file untuk menyimpan jadwal (misal: jadwal_bulan.csv): ");
                    scanf(" %[^\n]", namaFileOutput);
                    bersihkanBuffer();
                    simpanJadwalKeCSV(namaFileOutput, jadwalFinal, totalShiftTerisi, daftarDokter);
                }
            }
            else
            {
                printf("\n[!] Silakan buat jadwal terlebih dahulu (Pilihan 2).\n");
            }
            break;
        case 0:
            printf("\nTerima kasih, program ditutup.\n");
            break;
        default:
            printf("\n[!] Pilihan tidak valid.\n");
        }
    } while (pilihan != 0);

    bersihkanMemori();
    return 0;
}