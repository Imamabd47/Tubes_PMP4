#ifndef Database_H
#define Database_H

    struct pegawai{
        char nama[50];
        int maks_shift;
        char preferensi_shift[25];
        struct pegawai *next;
        struct pegawai *before;
    } ;
    extern struct pegawai *Data_Pegawai ; //Data Pegawai 
    extern struct pegawai *first ; //Data_pegawai paling pertama
    void input_string(char *arr);
    int input_integer();
    int input_integer_positif();
    void printBanner(const char *text, char border, int totalWidth);
    void Database();

#endif