#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define namafile "datadokter.csv"

struct datadokter {
    char *nama;
    int shift_awal;
    int shift;
    char *preferensi;
    struct datadokter* next;
} *head = NULL, *tail = NULL, *current;

void tambahNode(char *nama, int shift, char *preferensi) {
    current = (struct datadokter*) malloc(sizeof(struct datadokter));
    current->nama = (char *)malloc(strlen(nama) + 1);
    current->preferensi = (char *)malloc(strlen(preferensi) + 1);
    strcpy(current->nama, nama);
    strcpy(current->preferensi, preferensi);
    current->shift_awal = current->shift = shift;
    current->next = NULL;

    if (head == NULL) {
        head = tail = current;
    } else {
        tail->next = current;
        tail = current;
    }
}

void readFile() {
    FILE *fptr = fopen(namafile, "r");
    if (!fptr) {
        printf("File tidak dapat dibuka.\n");
        return;
    }

    char buf[256];
    while (fgets(buf, sizeof(buf), fptr)) {
        if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0';

        char *nama = strtok(buf, ",");
        char *shiftStr = strtok(NULL, ",");
        char *pref = strtok(NULL, ",");

        tambahNode(nama, atoi(shiftStr), pref);
    }

    fclose(fptr);
}

void tampilkanDokter() {
    struct datadokter *curr = head;
    int i = 1;
    while (curr != NULL) {
        printf("%d. Dr. %s - Maks: %d, Pref: %s\n", i++, curr->nama, curr->shift_awal, curr->preferensi);
        curr = curr->next;
    }
}

void tambahDokterManual() {
    char nama[100], preferensi[100];
    int shift;
    printf("Nama dokter: ");
    scanf(" %[^\n]", nama);
    printf("Maksimal shift/minggu: ");
    scanf("%d", &shift);
    printf("Preferensi shift (contoh: pagi/siang/malam/pagi dan siang): ");
    scanf(" %[^\n]", preferensi);

    tambahNode(nama, shift, preferensi);
}

void hapusDokter() {
    char target[100];
    printf("Masukkan nama dokter yang ingin dihapus: ");
    scanf(" %[^\n]", target);

    struct datadokter *curr = head, *prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->nama, target) == 0) {
            if (prev == NULL)
                head = curr->next;
            else
                prev->next = curr->next;

            free(curr->nama);
            free(curr->preferensi);
            free(curr);
            printf("Dokter berhasil dihapus.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    printf("Dokter tidak ditemukan.\n");
}

void bersihkanMemori() {
    struct datadokter *curr = head;
    while (curr != NULL) {
        struct datadokter *next = curr->next;
        free(curr->nama);
        free(curr->preferensi);
        free(curr);
        curr = next;
    }
}

void resetShiftMingguan() {
    struct datadokter *curr = head;
    while (curr != NULL) {
        curr->shift = curr->shift_awal;
        curr = curr->next;
    }
}

int cekPreferensi(const char *preferensi_dokter, const char *shift) {
    // Gunakan strstr untuk mencari substring
    return (strstr(preferensi_dokter, shift) != NULL);
}

int hitungPelanggaranPreferensi(char jadwal[30][3][6][100]) {
    int total_pelanggaran = 0;
    char *shiftLabel[] = {"pagi", "siang", "malam"};

    for (int hari = 0; hari < 30; hari++) {
        for (int s = 0; s < 3; s++) {
            int max_dokter = (s == 2) ? 5 : 6;
            
            for (int d = 0; d < max_dokter; d++) {
                if (strlen(jadwal[hari][s][d]) == 0) continue;
                
                // Cari dokter di linked list
                struct datadokter *curr = head;
                while (curr != NULL) {
                    if (strcmp(curr->nama, jadwal[hari][s][d]) == 0) {
                        // Gunakan strstr untuk cek preferensi
                        if (!cekPreferensi(curr->preferensi, shiftLabel[s])) {
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

void jadwalotomatis30hari(char jadwal[30][3][6][100]) {
    char *shiftLabel[] = {"pagi", "siang", "malam"};

    for (int minggu = 0; minggu < 30; minggu += 7) {
        resetShiftMingguan();

        for (int hari = minggu; hari < minggu + 7 && hari < 30; hari++) {
            int count[3] = {0};  // count[0]=pagi, count[1]=siang, count[2]=malam

            // Alokasi berdasarkan preferensi
            for (int s = 0; s < 3; s++) {
                int max_dokter = (s == 2) ? 5 : 6;
                struct datadokter *curr = head;
                while (curr && count[s] < max_dokter) {
                    if (curr->shift > 0 && cekPreferensi(curr->preferensi, shiftLabel[s])) {
                        strcpy(jadwal[hari][s][count[s]++], curr->nama);
                        curr->shift--;
                    }
                    curr = curr->next;
                }
            }

            // Isi sisa jika belum penuh
            for (int s = 0; s < 3; s++) {
                int max_dokter = (s == 2) ? 5 : 6;
                struct datadokter *curr = head;
                while (curr && count[s] < max_dokter) {
                    if (curr->shift > 0) {
                        int alreadyScheduled = 0;
                        for (int x = 0; x < 3; x++) {
                            for (int y = 0; y < ((x == 2) ? 5 : 6); y++) {
                                if (strcmp(jadwal[hari][x][y], curr->nama) == 0) {
                                    alreadyScheduled = 1;
                                    break;
                                }
                            }
                            if (alreadyScheduled) break;
                        }
                        if (!alreadyScheduled) {
                            strcpy(jadwal[hari][s][count[s]++], curr->nama);
                            curr->shift--;
                        }
                    }
                    curr = curr->next;
                }
            }
        }
    }
    
    // Hitung dan tampilkan pelanggaran preferensi
    int pelanggaran = hitungPelanggaranPreferensi(jadwal);
    printf("\nTotal pelanggaran preferensi shift: %d\n", pelanggaran);
}

void tampilkanJadwal30hari(char jadwal[30][3][6][100]) {
    char* shiftNama[] = {"Pagi", "Siang", "Malam"};
    for (int hari = 0; hari < 30; hari++) {
        printf("\nHari ke-%d:\n", hari + 1);
        for (int s = 0; s < 3; s++) {
            int max_dokter = (s == 2) ? 5 : 6;
            printf("  Shift %s (%d dokter):\n", shiftNama[s], max_dokter);
            for (int j = 0; j < max_dokter; j++) {
                if (strlen(jadwal[hari][s][j]) > 0)
                    printf("    - %s\n", jadwal[hari][s][j]);
                else
                    printf("    - (Kosong)\n");
            }
        }
    }
    
    // Hitung dan tampilkan pelanggaran preferensi
    int pelanggaran = hitungPelanggaranPreferensi(jadwal);
    printf("\nTotal pelanggaran preferensi shift: %d\n", pelanggaran);
}

int main() {
    int pilihan;
    readFile();

    do {
        printf("\nMenu:\n");
        printf("1. Tampilkan dokter\n");
        printf("2. Tambah dokter\n");
        printf("3. Hapus dokter\n");
        printf("4. Jadwal Otomatis 30 Hari\n");
        printf("0. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1: tampilkanDokter(); break;
            case 2: tambahDokterManual(); break;
            case 3: hapusDokter(); break;
            case 4: {
                char shift[30][3][6][100] = {{{{0}}}};
                jadwalotomatis30hari(shift);
                tampilkanJadwal30hari(shift);
                break;
            }
            case 0: bersihkanMemori(); break;
            default: printf("Pilihan tidak valid.\n");
        }
    } while (pilihan != 0);

    return 0;
}