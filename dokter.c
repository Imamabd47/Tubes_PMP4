#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dokter.h"

struct pegawai *Data_Pegawai = NULL;
struct pegawai *first = NULL;

void reset(struct pegawai **first, struct pegawai **List) {
    struct pegawai *temp = *first;
    while (temp != NULL) {
        struct pegawai *next = temp->next;
        free(temp);
        temp = next;
    }
    *first = NULL;
    *List = NULL;
}

void file2list(char *data) {
    struct pegawai *temp = malloc(sizeof(struct pegawai));
    temp->next = NULL;
    if (Data_Pegawai == NULL) {
        temp->before = NULL;
        first = temp;
    } else {
        temp->before = Data_Pegawai;
        Data_Pegawai->next = temp;
    }

    char *token;
    char buffer_data[256];
    strcpy(buffer_data, data);
    token = strtok(buffer_data, ",");
    strcpy(temp->nama, token);

    token = strtok(NULL, ",");
    temp->maks_shift = atoi(token);
    temp->shift_tersisa = temp->maks_shift;

    token = strtok(NULL, ",");
    strcpy(temp->preferensi_shift, token);

    Data_Pegawai = temp;
}

void load_data(char *nama_file) {
    FILE *fp = fopen(nama_file, "r");
    if (!fp) {
        printf("Gagal membuka file %s\n", nama_file);
        return;
    }
    reset(&first, &Data_Pegawai);
    char line[256];
    fgets(line, sizeof(line), fp); // skip header
    while (fgets(line, sizeof(line), fp)) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        file2list(line);
    }
    fclose(fp);
    printf("Data dokter berhasil dimuat.\n");
}

void tampilkan_dokter() {
    struct pegawai *curr = first;
    int i = 1;
    while (curr) {
        printf("%d. %s - Maks/Week: %d - Pref: %s\n", i++, curr->nama, curr->maks_shift, curr->preferensi_shift);
        curr = curr->next;
    }
}

void tambah_dokter_manual() {
    struct pegawai *temp = malloc(sizeof(struct pegawai));
    temp->next = NULL;
    if (Data_Pegawai == NULL) {
        temp->before = NULL;
        first = temp;
    } else {
        temp->before = Data_Pegawai;
        Data_Pegawai->next = temp;
    }

    printf("Nama Dokter: ");
    scanf(" %[^\n]", temp->nama);
    printf("Maks Shift per minggu: ");
    scanf("%d", &temp->maks_shift);
    temp->shift_tersisa = temp->maks_shift;
    printf("Preferensi shift (pagi/siang/malam/bebas): ");
    scanf(" %[^\n]", temp->preferensi_shift);

    Data_Pegawai = temp;
    printf("Dokter %s berhasil ditambahkan.\n", temp->nama);
}

void simpan_ke_file(const char *nama_file) {
    FILE *fp = fopen(nama_file, "w");
    if (!fp) {
        printf("Gagal membuka file untuk menyimpan data.\n");
        return;
    }
    fprintf(fp, "nama,shift_maks,preferensi_shift\n");
    struct pegawai *curr = first;
    while (curr) {
        fprintf(fp, "%s,%d,%s\n", curr->nama, curr->maks_shift, curr->preferensi_shift);
        curr = curr->next;
    }
    fclose(fp);
    printf("Data dokter berhasil disimpan ke %s\n", nama_file);
}

void edit_dokter() {
    char target[50];
    printf("Masukkan nama dokter yang ingin diedit: ");
    scanf(" %[^\n]", target);
    struct pegawai *curr = first;
    while (curr) {
        if (strcmp(curr->nama, target) == 0) {
            printf("Edit data untuk %s:\n", curr->nama);
            printf("Nama baru (tekan Enter untuk tidak mengubah): ");
            char buffer[50];
            getchar(); // consume leftover newline
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0) strcpy(curr->nama, buffer);

            printf("Maks shift baru (-1 untuk tidak mengubah): ");
            int s;
            scanf("%d", &s);
            if (s > 0) {
                curr->maks_shift = s;
                curr->shift_tersisa = s;
            }

            printf("Preferensi shift baru (tekan Enter untuk tidak mengubah): ");
            getchar();
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0) strcpy(curr->preferensi_shift, buffer);

            printf("Data dokter berhasil diperbarui.\n");
            return;
        }
        curr = curr->next;
    }
    printf("Dokter dengan nama '%s' tidak ditemukan.\n", target);
}

void hapus_dokter() {
    char target[50];
    printf("Masukkan nama dokter yang ingin dihapus: ");
    scanf(" %[^\n]", target);

    struct pegawai *curr = first;
    while (curr) {
        if (strcmp(curr->nama, target) == 0) {
            if (curr->before) curr->before->next = curr->next;
            if (curr->next) curr->next->before = curr->before;
            if (curr == first) first = curr->next;
            if (curr == Data_Pegawai) Data_Pegawai = curr->before;

            free(curr);
            printf("Dokter %s berhasil dihapus.\n", target);
            return;
        }
        curr = curr->next;
    }
    printf("Dokter dengan nama '%s' tidak ditemukan.\n", target);
}
