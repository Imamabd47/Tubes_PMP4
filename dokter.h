#ifndef DOKTER_H
#define DOKTER_H

struct pegawai {
    char nama[50];
    int maks_shift;
    char preferensi_shift[25];
    int shift_tersisa;
    struct pegawai *next;
    struct pegawai *before;
};

extern struct pegawai *Data_Pegawai;
extern struct pegawai *first;

void reset(struct pegawai **first, struct pegawai **List);
void file2list(char *data);
void load_data(char *nama_file);
void tampilkan_dokter();
void tambah_dokter_manual();
void simpan_ke_file(const char *nama_file);
void hapus_dokter();
void edit_dokter();

#endif