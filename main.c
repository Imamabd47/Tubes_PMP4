#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "Database.h"
#include "jadwal_maker.h"
#define ITALIC "\x1b[3m"
#define RESET  "\x1b[0m"
#define BOLD   "\x1b[1m"
#define RED    "\x1b[31m"
#define YELLOW  "\x1b[93m"
#define F_YELLOW  "\x1b[33m"

//Fungsi untuk display Fitur
void fitur_display(int *choice,char *nama_file,int jadwal_maker_param){
    printf("\nSilahkan pilih Opsi Dari bagian berikut!!\n");
    printf("[1]%-35s||"BOLD ITALIC RED"Untuk masuk kedalam sistem database\n"RESET,"Database");
    printf("[2]%-35s||"BOLD ITALIC RED"Untuk masuk kedalam sistem pembuatan jadwal\n"RESET,"Buat Jadwal");
    printf("[3]%-35s||"BOLD ITALIC RED"Untuk melihat jadwal yang sudah dibuat\n"RESET,"Lihat Jadwal");
    printf("[0]Keluar\n\n");
    if (jadwal_maker_param) printf("Status:\nFile Yang Dimuat< %s\nJadwal Tersimpan< SAVE\n\nPerintah< ",nama_file);
    else printf("Status:\nFile Yang Dimuat< %s\nJadwal Tersimpan< UNSAVE\n\nPerintah< ",nama_file);
    *choice = input_integer();
}

//Fungsi untuk memberi pesan eror ketika data dokter  kurang yang menyebabkan fitur 2 & 3 tidak bisa digunakan
void eror1(){
    printf("\nData dokter kurang, Fitur ini Tidak bisa digunakan!!\n\n");
}

//Fungsi untuk mengecek apakah fitur "Lihat jadwal", "Buat Jadwal" available
void avail(char *nama_file,char *nama_file_default,int *fitur_param,int *jadwal_maker_param){
    FILE *fptr;
    fptr = fopen("Data_Dokter.csv","r");
    //Cek File ada atau tidak(output database)
    if (fptr!=NULL){
        char buffer[256];
        fgets(buffer,256,fptr);
        char *token;
        token = strtok(buffer,",");
        token = strtok(NULL,",");
        sscanf(token,"%d",jadwal_maker_param);
        token = strtok(NULL,",");
        int i=0;
        while (1){
            if (*(token+i)=='\n'){
                *(token+i)='\0';
                break;
            }
            i++;
        }
        strcpy(nama_file,token);
        strcpy(nama_file_default,nama_file);
        //Cek apakah ada file yang dimuat
        if (strcasecmp(nama_file,"NONE")!=0){
            *(fitur_param)=1;
        }
        //Menghitung total shift
        fgets(buffer,256,fptr);
        int sum=0;int src;
        while (fgets(buffer,256,fptr)!=NULL){
            token = strtok(buffer,",");
            token = strtok(NULL,",");
            sscanf(token,"%d",&src);
            sum += src;
        }
        fclose(fptr);
        //Mengecek Total Shift
        if (sum>20){
            *(fitur_param+1)=1;
        }
    }
}

//Fungsi buat jadwal dengan semisal file atau total shift kurang
void buat_jadwal(int *fitur_param,char *nama_file,int *jadwal_maker_param,int *jadwal_dibuat){
    if (!(*(fitur_param))){
        eror();
    }
    else if (!(*(fitur_param+1))){
        eror1();
    }
    else{
        menu_jadwal_maker(nama_file,jadwal_maker_param,jadwal_dibuat);
    }
}

//Fungsi lihat_jadwal dengan semisal file atau total shift kurang
void lihat_jadwal(char *nama_file,int *jadwal_maker_param, int jadwalDibuat){
    if (jadwalDibuat||*jadwal_maker_param){
        menu_lihat_jadwal(nama_file,jadwal_maker_param);
    }
    else printf("\nBelum membuat jadwal, Fitur ini Tidak bisa digunakan!!\n\n");
}

int main(){
    char nama_file[50] = "NONE";
    char nama_file_default[50] = "NONE";
    int jadwalDibuat = 0;
    int jadwal_maker_param = 0;
    int fitur_param[2] = {0};
    avail(nama_file,nama_file_default,fitur_param,&jadwal_maker_param);
    printBanner("Selamat Datang",'=',136);
    int choice;
    fitur_display(&choice,nama_file,jadwal_maker_param);
    while (choice!=0){
        switch (choice){
        case 1:
            Database(&jadwalDibuat);
            break;
        case 2:
            buat_jadwal(fitur_param,nama_file,&jadwal_maker_param,&jadwalDibuat);
            break;
        case 3:
            lihat_jadwal(nama_file,&jadwal_maker_param,jadwalDibuat);
            break;
        default:
            break;
        }
        avail(nama_file,nama_file_default,fitur_param,&jadwal_maker_param);
        fitur_display(&choice,nama_file,jadwal_maker_param);
    }
}