#ifndef JADWAL_H
#define JADWAL_H

    typedef enum{
        PAGI,
        SIANG,
        MALAM
    } TipeShift;

    typedef struct{
        int id;
        char nama[50];
        char preferensi[100];
            int total_shift_dijadwalkan;
            int pelanggaran_preferensi_terjadi;
        } Dokter;

    typedef struct{
        int hari;
        TipeShift tipe;
        int id_dokter;
    } Shift;
    
    struct datadokter{
        char *nama;
        int shift_awal;
        int shift;
        char *preferensi;
        struct datadokter *next;
    } ;
    extern struct datadokter *head ;
    extern struct datadokter *tail ;
    extern struct datadokter *current ;
    extern int jadwalSudahDibuat ;
    extern int read_param;
    void cetak_bingkai(const char *judul);
    void menu_jadwal_maker(char *nama_file,int *jadwal_maker_param,int *jadwalDibuat);
    void menu_lihat_jadwal(char *nama_file,int *jadwal_maker_param);
#endif
