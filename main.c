#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "Database.h"
#define ITALIC "\x1b[3m"
#define RESET  "\x1b[0m"
#define BOLD   "\x1b[1m"
#define RED    "\x1b[31m"
#define YELLOW  "\x1b[93m"
#define F_YELLOW  "\x1b[33m"

void fitur_display(int *choice){
    printf("\nSilahkan pilih Opsi Dari bagian berikut!!\n");
    printf("[1]%-35s||"BOLD ITALIC RED"Untuk masuk kedalam sistem database\n"RESET,"Database");
    printf("[2]%-35s||"BOLD ITALIC RED"Untuk masuk kedalam sistem pembuatan jadwal\n"RESET,"Buat Jadwal");
    printf("[3]%-35s||"BOLD ITALIC RED"Untuk melihat jadwal yang sudah dibuat\n"RESET,"Lihat Jadwal");
    printf("[0]Keluar\n\n");
    printf("Masukkan< ");
    *choice = input_integer();
}

int main(){
    printBanner("Selamat Datang",'=',136);
    int choice;
    fitur_display(&choice);
    while (choice!=0){
        switch (choice){
        case 1:
            Database();
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
        }
        fitur_display(&choice);
    }
}