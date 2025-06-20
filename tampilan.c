#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // Untuk strcasecmp

// =====================================================================
// --- BAGIAN 1: DEFINISI STRUKTUR DATA & KONSTANTA ---
// =====================================================================

#define JUMLAH_HARI 30
#define SHIFT_PER_HARI 3
#define MAX_DOKTER_PER_SHIFT 6
#define TOTAL_SHIFT_MAX (JUMLAH_HARI * SHIFT_PER_HARI * MAX_DOKTER_PER_SHIFT)
#define MAX_DOKTER 50
#define NAMA_FILE_JADWAL_OUTPUT "jadwal_final_rapi.csv" // Nama file diubah
#define NAMA_FILE_DATA_DOKTER "datadokter.csv"

typedef enum { PAGI, SIANG, MALAM } TipeShift;

typedef struct {
    int id;
    char nama[50];
    char preferensi[100];
    int total_shift_dijadwalkan;
    int pelanggaran_preferensi_terjadi;
} Dokter;

typedef struct {
    int hari;
    TipeShift tipe;
    int id_dokter;
} Shift;

struct datadokter {
    char *nama;
    int shift_awal;
    int shift;
    char *preferensi;
    struct datadokter* next;
} *head = NULL, *tail = NULL;


// =====================================================================
// --- BAGIAN 2: MANAJEMEN DOKTER & FILE ---
// =====================================================================

void tambahNode(char *nama, int shift, char *preferensi) {
    struct datadokter* current = (struct datadokter*) malloc(sizeof(struct datadokter));
    current->nama = (char *)malloc(strlen(nama) + 1);
    current->preferensi = (char *)malloc(strlen(preferensi) + 1);
    strcpy(current->nama, nama);
    strcpy(current->preferensi, preferensi);
    current->shift_awal = current->shift = shift;
    current->next = NULL;

    if (head == NULL) { head = tail = current; } 
    else { tail->next = current; tail = current; }
}

void bacaFileDokter() {
    FILE *fptr = fopen(NAMA_FILE_DATA_DOKTER, "r");
    if (!fptr) {
        printf("File %s tidak ditemukan. Daftar dokter kosong.\n", NAMA_FILE_DATA_DOKTER);
        return;
    }
    char buf[256];
    fgets(buf, sizeof(buf), fptr);
    while (fgets(buf, sizeof(buf), fptr)) {
        if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';
        if (strlen(buf) < 3) continue;
        char *nama = strtok(buf, ",");
        char *shiftStr = strtok(NULL, ",");
        char *pref = strtok(NULL, ",");
        if (nama && shiftStr && pref) {
            tambahNode(nama, atoi(shiftStr), pref);
        }
    }
    fclose(fptr);
    printf("Data dokter berhasil dimuat dari %s\n", NAMA_FILE_DATA_DOKTER);
}

void tampilkanDaftarDokter() {
    struct datadokter *curr = head;
    int i = 1;
    if (!curr) { printf("Belum ada data dokter.\n"); return; }
    printf("\n--- Daftar Dokter Tersedia ---\n");
    while (curr != NULL) {
        printf("%d. Dr. %-20s (Maks Shift/Minggu: %d, Pref: %s)\n", i++, curr->nama, curr->shift_awal, curr->preferensi);
        curr = curr->next;
    }
}

void bersihkanMemoriDokter() {
    struct datadokter *curr = head;
    while (curr != NULL) {
        struct datadokter *next = curr->next;
        free(curr->nama);
        free(curr->preferensi);
        free(curr);
        curr = next;
    }
    head = NULL;
}


// =====================================================================
// --- BAGIAN 3: ALGORITMA PENJADWALAN (LOGIKA INTI) ---
// =====================================================================

int cekPreferensi(const char *preferensi_dokter, const char *shift) {
    return (strcasecmp(preferensi_dokter, "semua") == 0 || strstr(preferensi_dokter, shift) != NULL);
}

void resetShiftMingguan() {
    for (struct datadokter *curr = head; curr != NULL; curr = curr->next) {
        curr->shift = curr->shift_awal;
    }
}

int sudahTerjadwalHariIni(const char* nama, int hari, char jadwal[30][3][MAX_DOKTER_PER_SHIFT][100]) {
    for (int s = 0; s < 3; s++) {
        for (int d = 0; d < MAX_DOKTER_PER_SHIFT; d++) {
            if (strcmp(jadwal[hari][s][d], nama) == 0) return 1;
        }
    }
    return 0;
}

void buatJadwalOtomatis(char jadwal[30][3][MAX_DOKTER_PER_SHIFT][100]) {
    memset(jadwal, 0, sizeof(char) * 30 * 3 * MAX_DOKTER_PER_SHIFT * 100);
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    struct datadokter *giliran = head;
    printf("\nMembuat jadwal otomatis dengan strategi baru...\n");
    for (int minggu = 0; minggu < 30; minggu += 7) {
        resetShiftMingguan();
        for (int hari = minggu; hari < minggu + 7 && hari < 30; hari++) {
            int count[3] = {0};
            for (int s = 0; s < 3; s++) {
                int max_dokter = (s == 2) ? 5 : MAX_DOKTER_PER_SHIFT;
                for (struct datadokter* pencari = head; pencari != NULL; pencari = pencari->next) {
                    if (count[s] < max_dokter && pencari->shift > 0 &&
                        !sudahTerjadwalHariIni(pencari->nama, hari, jadwal) &&
                        cekPreferensi(pencari->preferensi, shiftLabel[s])) {
                        strcpy(jadwal[hari][s][count[s]++], pencari->nama);
                        pencari->shift--;
                    }
                }
            }
            for (int s = 0; s < 3; s++) {
                int max_dokter = (s == 2) ? 5 : MAX_DOKTER_PER_SHIFT;
                while (count[s] < max_dokter) {
                    int dokterDitemukan = 0;
                    struct datadokter *penanda_awal_loop = giliran; 
                    do {
                        if (giliran->shift > 0 && !sudahTerjadwalHariIni(giliran->nama, hari, jadwal)) {
                            strcpy(jadwal[hari][s][count[s]++], giliran->nama);
                            giliran->shift--;
                            dokterDitemukan = 1;
                            giliran = giliran->next ? giliran->next : head;
                            break; 
                        }
                        giliran = giliran->next ? giliran->next : head;
                    } while (giliran != penanda_awal_loop);
                    if (!dokterDitemukan) break;
                }
            }
        }
    }
    printf("Jadwal baru berhasil dibuat.\n");
}


// =====================================================================
// --- BAGIAN 4: FUNGSI JEMBATAN (KONVERSI DATA) ---
// =====================================================================

int konversiDataDokter(Dokter daftarDokter[]) {
    int jumlahDokter = 0;
    for (struct datadokter* node = head; node != NULL && jumlahDokter < MAX_DOKTER; node = node->next, jumlahDokter++) {
        daftarDokter[jumlahDokter].id = jumlahDokter;
        strcpy(daftarDokter[jumlahDokter].nama, node->nama);
        strcpy(daftarDokter[jumlahDokter].preferensi, node->preferensi);
        daftarDokter[jumlahDokter].total_shift_dijadwalkan = 0;
        daftarDokter[jumlahDokter].pelanggaran_preferensi_terjadi = 0;
    }
    return jumlahDokter;
}

int konversiJadwal(char jadwalOtomatis[30][3][MAX_DOKTER_PER_SHIFT][100], Shift jadwalFinal[], Dokter daftarDokter[], int jumlahDokter) {
    int totalShiftTerisi = 0;
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    for(int i = 0; i < jumlahDokter; i++) {
        daftarDokter[i].total_shift_dijadwalkan = 0;
        daftarDokter[i].pelanggaran_preferensi_terjadi = 0;
    }
    for (int h = 0; h < JUMLAH_HARI; h++) {
        for (int s = 0; s < SHIFT_PER_HARI; s++) {
            int max_dokter_per_shift = (s == 2) ? 5 : MAX_DOKTER_PER_SHIFT;
            for (int d = 0; d < max_dokter_per_shift; d++) {
                if (strlen(jadwalOtomatis[h][s][d]) > 0) {
                    char *namaDokter = jadwalOtomatis[h][s][d];
                    int idDokter = -1;
                    for (int i = 0; i < jumlahDokter; i++) {
                        if (strcmp(daftarDokter[i].nama, namaDokter) == 0) { idDokter = i; break; }
                    }
                    if (idDokter != -1) {
                        jadwalFinal[totalShiftTerisi].hari = h + 1;
                        jadwalFinal[totalShiftTerisi].tipe = (TipeShift)s;
                        jadwalFinal[totalShiftTerisi].id_dokter = idDokter;
                        daftarDokter[idDokter].total_shift_dijadwalkan++;
                        if (!cekPreferensi(daftarDokter[idDokter].preferensi, shiftLabel[s])) {
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

// =====================================================================
// --- BAGIAN 5: TAMPILAN JADWAL & PENYIMPANAN KE CSV ---
// =====================================================================

const char* getNamaShift(TipeShift tipe) {
    switch (tipe) {
        case PAGI: return "Pagi";
        case SIANG: return "Siang";
        case MALAM: return "Malam";
        default: return "N/A";
    }
}

void tampilkanJadwalHarian(int hari, const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[]) {
    if (hari < 1 || hari > JUMLAH_HARI) { printf("Hari tidak valid.\n"); return; }
    printf("\n--- Jadwal Jaga Hari ke-%d ---\n", hari);
    for (int s = 0; s < SHIFT_PER_HARI; s++) {
        printf("Shift %-5s : ", getNamaShift((TipeShift)s));
        int count = 0;
        for (int i = 0; i < totalShiftTerisi; i++) {
            if (jadwal[i].hari == hari && jadwal[i].tipe == (TipeShift)s) {
                printf("%s; ", daftarDokter[jadwal[i].id_dokter].nama);
                count++;
            }
        }
        if (count == 0) printf("(Kosong)");
        printf("\n");
    }
}

void tampilkanJadwalMingguan(int minggu, const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[]) {
    if (minggu < 1 || minggu > 5) { printf("Minggu tidak valid.\n"); return; }
    int hariMulai = (minggu - 1) * 7 + 1;
    int hariSelesai = (hariMulai + 6 > JUMLAH_HARI) ? JUMLAH_HARI : hariMulai + 6;
    
    printf("\n\n===============================================");
    printf("\n--- JADWAL MINGGU KE-%d (HARI %d s/d %d) ---", minggu, hariMulai, hariSelesai);
    printf("\n===============================================\n");
    
    for (int h = hariMulai; h <= hariSelesai; h++) {
        if (h > JUMLAH_HARI) break;
        tampilkanJadwalHarian(h, jadwal, totalShiftTerisi, daftarDokter);
    }
}

// *** FUNGSI BARU UNTUK TAMPILAN SEBULAN ***
void tampilkanJadwalSebulan(const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[]){
    printf("\n\n*************************************************");
    printf("\n*** TAMPILAN JADWAL SEBULAN          ***");
    printf("\n*************************************************\n");
    for(int m = 1; m <= 5; m++){
        int hariMulai = (m - 1) * 7 + 1;
        if(hariMulai > JUMLAH_HARI) break;
        tampilkanJadwalMingguan(m, jadwal, totalShiftTerisi, daftarDokter);
    }
}

// *** FUNGSI BARU UNTUK LAPORAN PELANGGARAN ***
void tampilkanLaporanPelanggaran(const Dokter daftarDokter[], int jumlahDokter) {
    printf("\n--- Laporan Pelanggaran Preferensi Shift ---\n");
    int totalPelanggaran = 0;
    for(int i = 0; i < jumlahDokter; i++){
        if(daftarDokter[i].pelanggaran_preferensi_terjadi > 0) {
            printf("Dr. %-20s | Jumlah Pelanggaran: %d\n", 
                    daftarDokter[i].nama, 
                    daftarDokter[i].pelanggaran_preferensi_terjadi);
            totalPelanggaran++;
        }
    }
    if (totalPelanggaran == 0) {
        printf("Selamat! Tidak ditemukan pelanggaran preferensi dalam jadwal ini.\n");
    }
}

// *** FUNGSI CSV YANG DIPERBARUI ***
void simpanJadwalKeCSV(const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[]) {
    FILE *fptr = fopen(NAMA_FILE_JADWAL_OUTPUT, "w");
    if (!fptr) {
        printf("ERROR: Tidak dapat membuat file %s untuk penulisan.\n", NAMA_FILE_JADWAL_OUTPUT);
        return;
    }

    printf("\nMenyimpan jadwal ke file %s...", NAMA_FILE_JADWAL_OUTPUT);

    for (int minggu = 0; minggu < 5; minggu++) {
        int hariMulai = minggu * 7 + 1;
        if (hariMulai > JUMLAH_HARI) break;

        fprintf(fptr, "JADWAL MINGGU %d (Hari %d - %d)\n", minggu + 1, hariMulai, hariMulai + 6 > JUMLAH_HARI ? JUMLAH_HARI : hariMulai + 6);
        fprintf(fptr, "Shift");
        for (int i = 0; i < 7; i++) {
            int hariIni = hariMulai + i;
            if (hariIni > JUMLAH_HARI) break;
            fprintf(fptr, ",Hari %d", hariIni);
        }
        fprintf(fptr, "\n");

        for (int s = 0; s < SHIFT_PER_HARI; s++) {
            fprintf(fptr, "%s", getNamaShift((TipeShift)s));
            for (int i = 0; i < 7; i++) {
                int hariIni = hariMulai + i;
                if (hariIni > JUMLAH_HARI) break;
                char selBuffer[2048] = "";
                int dokterDiSel = 0;
                for (int j = 0; j < totalShiftTerisi; j++) {
                    if (jadwal[j].hari == hariIni && jadwal[j].tipe == (TipeShift)s) {
                        strcat(selBuffer, daftarDokter[jadwal[j].id_dokter].nama);
                        strcat(selBuffer, "\n"); // Menggunakan newline sebagai pemisah
                        dokterDiSel++;
                    }
                }
                if (dokterDiSel > 0) {
                    selBuffer[strlen(selBuffer) - 1] = '\0'; // Hapus newline terakhir
                }
                fprintf(fptr, ",\"%s\"", selBuffer);
            }
            fprintf(fptr, "\n");
        }
        fprintf(fptr, "\n");
    }
    fclose(fptr);
    printf(" Berhasil.\n");
}


// =====================================================================
// --- BAGIAN 6: FUNGSI UTAMA (MAIN) & KONTROL MENU ---
// =====================================================================

void tampilkanMenu() {
    printf("\n=========================================");
    printf("\n|   Sistem Penjadwalan Dokter Otomatis  |");
    printf("\n=========================================\n");
    printf("1. Tampilkan Daftar Dokter\n");
    printf("2. Buat Jadwal Otomatis Baru\n");
    printf("-----------------------------------------\n");
    printf("3. Tampilkan Jadwal Harian\n");
    printf("4. Tampilkan Jadwal Mingguan\n");
    printf("5. Tampilkan Jadwal Sebulan Penuh\n"); // OPSI BARU
    printf("-----------------------------------------\n");
    printf("6. Simpan Jadwal ke File CSV\n");
    printf("7. Tampilkan Laporan Pelanggaran\n"); // OPSI YANG DIUBAH
    printf("-----------------------------------------\n");
    printf("0. Keluar\n");
    printf("=========================================\n");
}

void bersihkanBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    char jadwalOtomatis[30][3][MAX_DOKTER_PER_SHIFT][100] = {0};
    Dokter daftarDokter[MAX_DOKTER];
    Shift jadwalFinal[TOTAL_SHIFT_MAX];
    int jumlahDokter = 0;
    int totalShiftTerisi = 0;
    int jadwalSudahDibuat = 0;
    int pilihan = -1;

    bacaFileDokter();
    if (head == NULL) {
        printf("Tidak ada data dokter untuk diproses. Program berhenti.\n");
        return 1;
    }
    jumlahDokter = konversiDataDokter(daftarDokter);

    do {
        tampilkanMenu();
        printf(">> Pilihan Anda: ");
        if (scanf("%d", &pilihan) != 1) {
            printf("Input tidak valid. Harap masukkan angka.\n");
            bersihkanBuffer();
            continue;
        }
        bersihkanBuffer();

        switch (pilihan) {
            case 1:
                tampilkanDaftarDokter();
                break;
            case 2:
                buatJadwalOtomatis(jadwalOtomatis);
                totalShiftTerisi = konversiJadwal(jadwalOtomatis, jadwalFinal, daftarDokter, jumlahDokter);
                jadwalSudahDibuat = 1;
                break;
            case 3:
                if (jadwalSudahDibuat) {
                    int hari;
                    printf("Masukkan hari yang ingin dilihat (1-30): ");
                    scanf("%d", &hari);
                    bersihkanBuffer();
                    tampilkanJadwalHarian(hari, jadwalFinal, totalShiftTerisi, daftarDokter);
                } else {
                    printf("\n[!] Silakan buat jadwal terlebih dahulu (Pilihan 2).\n");
                }
                break;
            case 4:
                if (jadwalSudahDibuat) {
                    int minggu;
                    printf("Masukkan minggu yang ingin dilihat (1-5): ");
                    scanf("%d", &minggu);
                    bersihkanBuffer();
                    tampilkanJadwalMingguan(minggu, jadwalFinal, totalShiftTerisi, daftarDokter);
                } else {
                    printf("\n[!] Silakan buat jadwal terlebih dahulu (Pilihan 2).\n");
                }
                break;
            case 5: // *** KASUS BARU ***
                if (jadwalSudahDibuat) {
                    tampilkanJadwalSebulan(jadwalFinal, totalShiftTerisi, daftarDokter);
                } else {
                    printf("\n[!] Silakan buat jadwal terlebih dahulu (Pilihan 2).\n");
                }
                break;
            case 6: // Nomor urut bergeser
                if (jadwalSudahDibuat) {
                    simpanJadwalKeCSV(jadwalFinal, totalShiftTerisi, daftarDokter);
                } else {
                    printf("\n[!] Silakan buat jadwal terlebih dahulu (Pilihan 2).\n");
                }
                break;
            case 7: // *** KASUS YANG DIUBAH ***
                if (jadwalSudahDibuat) {
                    tampilkanLaporanPelanggaran(daftarDokter, jumlahDokter);
                } else {
                    printf("\n[!] Silakan buat jadwal terlebih dahulu (Pilihan 2).\n");
                }
                break;
            case 0:
                printf("\nTerima kasih, program ditutup.\n");
                break;
            default:
                printf("\n[!] Pilihan tidak valid. Silakan coba lagi.\n");
                break;
        }
    } while (pilihan != 0);

    bersihkanMemoriDokter();
    return 0;
}