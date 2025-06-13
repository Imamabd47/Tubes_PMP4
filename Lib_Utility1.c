#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "Lib_Utility1.h"

struct pegawai{
    char nama[50];
    int maks_shift;
    char preferensi_shift[25];
    struct pegawai *next;
    struct pegawai *before;
} ;

//Fungsi PrintfBanner
void printBanner(const char *text, char border, int totalWidth) {
    printf("\n");
    int textLen = strlen(text);
    int side = (totalWidth - textLen - 2) / 2; // -2 for spaces around text
    for (int i = 0; i < side; i++) printf("%c", border);
    printf(" %s ", text);
    for (int i = 0; i < side; i++) printf("%c", border);
    printf("\n");
}

//Fungsi input_string
void input_string(char *arr){
    fgets(arr,100,stdin);
    int i=0;
    while (1){
        if (*(arr+i)=='\n'){
            *(arr+i)='\0';
            break;
        }
        i++;
    }  
}

//Fungsi input_integer
int input_integer() {
    char buffer[100];
    int valid = 0;
    int angka;

    while (!valid) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Hapus newline jika ada
            buffer[strcspn(buffer, "\n")] = 0;
            
            int i = 0;
            // Cek string kosong
            if (buffer[0] == '\0') {
                printf("Input tidak boleh kosong. Silakan coba lagi.\n");
                printf("\nperintah< ");
                continue;
            }
            
            // Cek tanda minus di depan (boleh 1 kali)
            if (buffer[0] == '-') i = 1;
            
            valid = 1; // Asumsi valid
            for (; i < (int)strlen(buffer); i++) {
                if (!isdigit(buffer[i])) {
                    valid = 0;
                    printf("Input harus berupa angka bulat.\n");
                    printf("\nperintah< ");
                    break;
                }
            }
            
            if (valid) {
                angka = atoi(buffer);
                return angka;
            }
        }
        else {
            printf("Terjadi kesalahan input. Silakan coba lagi.\n");
            printf("\nperintah< ");
            // Clear error stdin
            clearerr(stdin);
        }
    }
    return 0; // Tidak akan sampai sini, tapi agar compiler aman
}

//Fungsi input_integer_positif
int input_integer_positif() {
    char buffer[100];
    int isValid = 0;
    int hasil;

    while (!isValid) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Hapus karakter newline
            buffer[strcspn(buffer, "\n")] = '\0';

            // Cek apakah string kosong
            if (buffer[0] == '\0') {
                printf("Input tidak boleh kosong. Silakan coba lagi.\n");
                printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
                continue;
            }

            int i = 0;
            isValid = 1;
            for (; buffer[i] != '\0'; i++) {
                if (!isdigit(buffer[i])) {
                    isValid = 0;
                    printf("Input harus berupa angka bulat positif.\n");
                    printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
                    break;
                }
            }

            if (isValid) {
                hasil = atoi(buffer);
                if (hasil <= 0) {
                    printf("Input harus lebih besar dari nol.\n");
                    printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
                    isValid = 0;
                } else {
                    return hasil;
                }
            }
        } else {
            printf("Terjadi kesalahan input. Silakan coba lagi.\n");
            printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
            clearerr(stdin); // reset status error stdin
        }
    }
    return 0; // fallback
}

//Fungsi eror
void eror(){
    printf("\nBelum ada File yang dimuat, Fitur Tidak bisa digunakan!!\n\n");
}

//Fungsi Reset
void reset(struct pegawai **first, struct pegawai **List){
    struct pegawai *temp = *first;
    while(temp != NULL){
        struct pegawai *next = temp -> next;
        free(temp);
        temp = next;
    }
    *first = NULL;
    *List = NULL;
}

