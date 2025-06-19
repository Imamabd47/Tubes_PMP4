#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dokter.h"
#include "jadwal.h"

const char* label_shift[] = {"pagi", "siang", "malam"};

void reset_shift_mingguan() {
    struct pegawai *curr = first;
    while (curr) {
        curr->shift_tersisa = curr->maks_shift;
        curr = curr->next;
    }
}

int total_dokter() {
    int count = 0;
    struct pegawai *curr = first;
    while (curr) {
        count++;
        curr = curr->next;
    }
    return count;
}

void pegawai_to_array(struct pegawai *arr[], int n) {
    struct pegawai *curr = first;
    int i = 0;
    while (curr && i < n) {
        arr[i++] = curr;
        curr = curr->next;
    }
}

#include <stdlib.h>
#include <time.h>

void buat_jadwal_otomatis(char jadwal[30][3][6][50]) {
    memset(jadwal, 0, sizeof(char) * 30 * 3 * 6 * 50);
    int n = total_dokter();
    if (n == 0) return;

    struct pegawai *arr[n];
    pegawai_to_array(arr, n);

    // Inisialisasi random
    srand(time(0));

    // Fungsi untuk mengurutkan berdasarkan shift tersisa (descending)
    int compare(const void *a, const void *b) {
        struct pegawai *dokterA = *(struct pegawai **)a;
        struct pegawai *dokterB = *(struct pegawai **)b;
        return dokterB->shift_tersisa - dokterA->shift_tersisa;
    }

    for (int hari = 0; hari < 30; hari++) {
        // Reset status shift harian
        int sudah_shift_hari_ini[n];
        memset(sudah_shift_hari_ini, 0, sizeof(sudah_shift_hari_ini));

        // Urutkan dokter berdasarkan shift tersisa (yang paling sedikit diutamakan)
        qsort(arr, n, sizeof(struct pegawai *), compare);

        // Shift pagi (6 dokter)
        for (int s = 0; s < 3; s++) {
            int dokter_dibutuhkan = (s == 2) ? 5 : 6; // malam 5, lainnya 6
            int count = 0;
            int percobaan = 0;
            int max_percobaan = n * 2;

            while (count < dokter_dibutuhkan && percobaan < max_percobaan) {
                for (int i = 0; i < n && count < dokter_dibutuhkan; i++) {
                    // Skip jika sudah dapat shift hari ini atau kuota habis
                    if (sudah_shift_hari_ini[i] || arr[i]->shift_tersisa <= 0)
                        continue;

                    // Cek preferensi shift
                    if (strstr(arr[i]->preferensi_shift, label_shift[s]) ||
                        strstr(arr[i]->preferensi_shift, "bebas")) {

                        // Pastikan dokter belum ada di shift ini
                        int sudah_ada = 0;
                        for (int y = 0; y < 4; y++) {
                            if (strcmp(jadwal[hari][s][y], arr[i]->nama) == 0) {
                                sudah_ada = 1;
                                break;
                            }
                        }

                        if (!sudah_ada) {
                            strcpy(jadwal[hari][s][count], arr[i]->nama);
                            count++;
                            arr[i]->shift_tersisa--;
                            sudah_shift_hari_ini[i] = 1;
                        }
                    }
                }
                percobaan++;
                
                // Jika belum terpenuhi, longgarkan preferensi pada percobaan kedua
                if (count < dokter_dibutuhkan && percobaan == max_percobaan/2) {
                    for (int i = 0; i < n && count < dokter_dibutuhkan; i++) {
                        if (!sudah_shift_hari_ini[i] && arr[i]->shift_tersisa > 0) {
                            int sudah_ada = 0;
                            for (int y = 0; y < 4; y++) {
                                if (strcmp(jadwal[hari][s][y], arr[i]->nama) == 0) {
                                    sudah_ada = 1;
                                    break;
                                }
                            }
                            if (!sudah_ada) {
                                strcpy(jadwal[hari][s][count], arr[i]->nama);
                                count++;
                                arr[i]->shift_tersisa--;
                                sudah_shift_hari_ini[i] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    // Pastikan semua dokter dapat shift minimal 1
    for (int i = 0; i < n; i++) {
        int dapat_shift = 0;
        for (int hari = 0; hari < 30 && !dapat_shift; hari++) {
            for (int s = 0; s < 3 && !dapat_shift; s++) {
                for (int j = 0; j < 4; j++) {
                    if (strcmp(jadwal[hari][s][j], arr[i]->nama) == 0) {
                        dapat_shift = 1;
                        break;
                    }
                }
            }
        }

        if (!dapat_shift) {
            // Cari slot kosong untuk dokter ini
            for (int hari = 0; hari < 30 && !dapat_shift; hari++) {
                for (int s = 0; s < 3 && !dapat_shift; s++) {
                    for (int j = 0; j < 4; j++) {
                        if (strlen(jadwal[hari][s][j]) == 0) {
                            // Cek preferensi
                            if (strstr(arr[i]->preferensi_shift, label_shift[s]) ||
                                strstr(arr[i]->preferensi_shift, "bebas")) {
                                strcpy(jadwal[hari][s][j], arr[i]->nama);
                                arr[i]->shift_tersisa--;
                                dapat_shift = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void export_jadwal_ke_csv(char jadwal[30][3][6][50], const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Gagal membuat file jadwal CSV.\n");
        return;
    }
    fprintf(fp, "Hari,Shift,Dokter1,Dokter2,Dokter3,Dokter4\n");
    for (int hari = 0; hari < 30; hari++) {
        for (int s = 0; s < 3; s++) {
            fprintf(fp, "%d,%s", hari + 1, label_shift[s]);
            for (int j = 0; j < 4; j++) {
                fprintf(fp, ",%s", jadwal[hari][s][j]);
            }
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
    printf("Jadwal berhasil disimpan ke %s\n", filename);
}