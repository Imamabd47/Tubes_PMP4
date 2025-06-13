#ifndef LibUtility1_H
#define LibUtility1_H

struct pegawai{
    char nama[50];
    int maks_shift;
    char preferensi_shift[25];
    struct pegawai *next;
    struct pegawai *before;
} ;

    void printBanner(const char *text, char border, int totalWidth); //Untuk membuat Tampilan panjang
    void input_string(char *arr); //Untuk validasi input string
    int input_integer();// Untuk validasi input integer
    int input_integer_positif();// Untuk validasi input integer positif
    void eror();// Untuk memberikan pesan eror ketika file tidak berhasil dimuat
    void reset(struct pegawai **first, struct pegawai **List);//  Untuk mereset link list..Butuh elemen pertama link list, dan listnya

#endif
