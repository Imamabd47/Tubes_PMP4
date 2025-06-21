#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =====================================================================
// --- BAGIAN 1: DEFINISI STRUKTUR DATA & KONSTANTA ---
// =====================================================================

#define FileNamaDokter "datadokter.csv"
#define JUMLAH_HARI 30
#define SHIFT_PER_HARI 3
#define MAX_DOKTER_PER_SHIFT 6 // Kuota maksimal, akan disesuaikan di logika
#define MAX_DOKTER 100
#define TOTAL_SHIFT_MAX (JUMLAH_HARI * SHIFT_PER_HARI * MAX_DOKTER_PER_SHIFT)

// Struct untuk tampilan dan laporan (fitur tambahan di main.c)
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

// Struct asli dari jadwalotomatis.c untuk proses penjadwalan inti
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
    FILE *fptr = fopen(FileNamaDokter, "r");
    if (!fptr)
    {
        printf("File '%s' tidak dapat dibuka. Pastikan file tersebut ada.\n", FileNamaDokter);
        return;
    }
    char buf[256];
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
    printf("\n--- Daftar Dokter Tersedia ---\n");
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

int cekPreferensi(const char *preferensi_dokter, const char *shift)
{
    return (strstr(preferensi_dokter, shift) != NULL);
}

int hitungPelanggaranPreferensi(char jadwal[30][3][6][100])
{
    int total_pelanggaran = 0;
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    int max_dokter[] = {6, 6, 5};

    for (int hari = 0; hari < 30; hari++)
    {
        for (int s = 0; s < 3; s++)
        {
            for (int d = 0; d < max_dokter[s]; d++)
            {
                if (strlen(jadwal[hari][s][d]) == 0)
                    continue;

                struct datadokter *curr = head;
                while (curr != NULL)
                {
                    if (strcmp(curr->nama, jadwal[hari][s][d]) == 0)
                    {
                        if (!cekPreferensi(curr->preferensi, shiftLabel[s]))
                        {
                            total_pelanggaran++;
                        }
                        break;
                    }
                    curr = curr->next;
                }
            }
        }
    }
    return total_pelanggaran;
}

void jadwalotomatis30hari(char jadwal[30][3][6][100])
{
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    int max_dokter[] = {6, 6, 5};

    for (int minggu = 0; minggu < 30; minggu += 7)
    {
        resetShiftMingguan();

        for (int hari = minggu; hari < minggu + 7 && hari < 30; hari++)
        {
            int count[3] = {0};

            for (int s = 0; s < 3; s++)
            {
                struct datadokter *curr = head;
                while (curr && count[s] < max_dokter[s])
                {
                    if (curr->shift > 0 && cekPreferensi(curr->preferensi, shiftLabel[s]))
                    {
                        strcpy(jadwal[hari][s][count[s]++], curr->nama);
                        curr->shift--;
                    }
                    curr = curr->next;
                }
            }

            for (int s = 0; s < 3; s++)
            {
                struct datadokter *curr = head;
                while (curr && count[s] < max_dokter[s])
                {
                    if (curr->shift > 0)
                    {
                        int alreadyScheduled = 0;
                        for (int x = 0; x < 3; x++)
                        {
                            for (int y = 0; y < max_dokter[x]; y++)
                            {
                                if (strcmp(jadwal[hari][x][y], curr->nama) == 0)
                                {
                                    alreadyScheduled = 1;
                                    break;
                                }
                            }
                            if (alreadyScheduled)
                                break;
                        }

                        if (!alreadyScheduled)
                        {
                            strcpy(jadwal[hari][s][count[s]++], curr->nama);
                            curr->shift--;
                        }
                    }
                    curr = curr->next;
                }
            }
        }
    }

    int pelanggaran = hitungPelanggaranPreferensi(jadwal);
    printf("\nTotal pelanggaran preferensi shift: %d\n", pelanggaran);
}

// =====================================================================
// --- BAGIAN ADHI & LUNA (FITUR TAMBAHAN & UI) ---
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

int konversiJadwal(char jadwalOtomatis[JUMLAH_HARI][SHIFT_PER_HARI][MAX_DOKTER_PER_SHIFT][100], Shift jadwalFinal[], Dokter daftarDokter[], int jumlahDokter)
{
    int totalShiftTerisi = 0;
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    int max_dokter_per_shift_arr[] = {6, 6, 5};

    for (int i = 0; i < jumlahDokter; i++)
    {
        daftarDokter[i].total_shift_dijadwalkan = 0;
        daftarDokter[i].pelanggaran_preferensi_terjadi = 0;
    }

    for (int h = 0; h < JUMLAH_HARI; h++)
    {
        for (int s = 0; s < SHIFT_PER_HARI; s++)
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
    if (hari < 1 || hari > JUMLAH_HARI)
    {
        printf("Hari tidak valid.\n");
        return;
    }
    printf("\n--- Jadwal Jaga Hari ke-%d ---\n", hari);
    for (int s = 0; s < SHIFT_PER_HARI; s++)
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
    int hariSelesai = (hariMulai + 6 > JUMLAH_HARI) ? JUMLAH_HARI : hariMulai + 6;

    printf("\n\n===============================================");
    printf("\n--- JADWAL MINGGU KE-%d (HARI %d s/d %d) ---", minggu, hariMulai, hariSelesai);
    printf("\n===============================================\n");

    for (int h = hariMulai; h <= hariSelesai; h++)
    {
        if (h > JUMLAH_HARI)
            break;
        tampilkanJadwalHarian(h, jadwal, totalShiftTerisi, daftarDokter);
    }
}

void tampilkanJadwalSebulan(const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[])
{
    printf("\n\n*************************************************");
    printf("\n*** TAMPILAN JADWAL SEBULAN PENUH       ***");
    printf("\n*************************************************\n");
    for (int m = 1; m <= 5; m++)
    {
        int hariMulai = (m - 1) * 7 + 1;
        if (hariMulai > JUMLAH_HARI)
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

// DIUBAH: Fungsi sekarang menerima nama file sebagai parameter
void simpanJadwalKeCSV(const char *namaFile, const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[])
{
    FILE *fptr = fopen(namaFile, "w");
    if (!fptr)
    {
        // DIUBAH: Pesan error menampilkan nama file yang gagal dibuat
        printf("ERROR: Tidak dapat membuat file '%s' untuk penulisan.\n", namaFile);
        return;
    }

    // DIUBAH: Pesan proses menampilkan nama file yang sedang dibuat
    printf("\nMenyimpan jadwal ke file '%s'...", namaFile);

    for (int minggu = 0; minggu < 5; minggu++)
    {
        int hariMulai = minggu * 7 + 1;
        if (hariMulai > JUMLAH_HARI)
            break;

        fprintf(fptr, "JADWAL MINGGU %d (Hari %d - %d)\n", minggu + 1, hariMulai, hariMulai + 6 > JUMLAH_HARI ? JUMLAH_HARI : hariMulai + 6);
        fprintf(fptr, "Shift");
        for (int i = 0; i < 7; i++)
        {
            int hariIni = hariMulai + i;
            if (hariIni > JUMLAH_HARI)
                break;
            fprintf(fptr, ",Hari %d", hariIni);
        }
        fprintf(fptr, "\n");

        for (int s = 0; s < SHIFT_PER_HARI; s++)
        {
            fprintf(fptr, "%s", getNamaShift((TipeShift)s));
            for (int i = 0; i < 7; i++)
            {
                int hariIni = hariMulai + i;
                if (hariIni > JUMLAH_HARI)
                    break;
                char selBuffer[2048] = "";
                for (int j = 0; j < totalShiftTerisi; j++)
                {
                    if (jadwal[j].hari == hariIni && jadwal[j].tipe == (TipeShift)s)
                    {
                        strcat(selBuffer, daftarDokter[jadwal[j].id_dokter].nama);
                        strcat(selBuffer, "\n");
                    }
                }
                if (strlen(selBuffer) > 0)
                {
                    selBuffer[strlen(selBuffer) - 1] = '\0';
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

void cariJadwalDokter(const Shift jadwal[], int totalShiftTerisi, const Dokter daftarDokter[], int jumlahDokter)
{
    char namaCari[50];
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

void tampilkanMenu()
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
    printf("7. Tampilkan Laporan Pelanggaran\n");
    printf("8. Simpan Jadwal ke File CSV\n");
    printf("-----------------------------------------\n");
    printf("0. Keluar\n");
    printf("=========================================\n");
}

int main()
{
    char jadwalOtomatis[JUMLAH_HARI][SHIFT_PER_HARI][MAX_DOKTER_PER_SHIFT][100] = {0};
    Dokter daftarDokter[MAX_DOKTER];
    Shift jadwalFinal[TOTAL_SHIFT_MAX];

    int jumlahDokter = 0;
    int totalShiftTerisi = 0;
    int pilihan = -1;
    int jadwalSudahDibuat = 0;

    readFile();
    if (head == NULL)
    {
        printf("Tidak ada data dokter untuk diproses. Program berhenti.\n");
        return 1;
    }
    jumlahDokter = konversiDataDokter(daftarDokter);

    printf("Selamat datang di Sistem Penjadwalan Dokter!\n");
    printf("Data %d dokter berhasil dimuat.\n", jumlahDokter);

    do
    {
        tampilkanMenu();
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
                    // BARU: Logika untuk meminta input nama file dari pengguna
                    char namaFileOutput[100];
                    printf("\nMasukkan nama file untuk menyimpan jadwal (misal: jadwal_bulan_ini.csv): ");
                    scanf(" %[^\n]", namaFileOutput);
                    bersihkanBuffer();

                    // Panggil fungsi simpan dengan nama file dari input pengguna
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
