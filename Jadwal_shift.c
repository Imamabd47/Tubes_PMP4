#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct shift;
struct hari{
    char hari[20];
    struct shift *pagi;
    struct shift *siang;
    struct shift *malam;
};

struct shift{
    char nama_pegawai1[100];
    char nama_pegawai2[100];
    char nama_pegawai3[100];
    char nama_pegawai4[100];
    char nama_pegawai5[100];
    char nama_pegawai6[100];
};

int main(){
    
}