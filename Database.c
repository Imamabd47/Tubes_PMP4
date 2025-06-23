#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "Jadwal_Maker.h"
#define ITALIC "\x1b[3m"
#define RESET  "\x1b[0m"
#define BOLD   "\x1b[1m"
#define RED    "\x1b[31m"
#define YELLOW  "\x1b[93m"
#define F_YELLOW  "\x1b[33m"
#define GREEN   "\x1b[32m"

//Struct

struct pegawai{
    char nama[50];
    int maks_shift;
    char preferensi_shift[25];
    struct pegawai *next;
    struct pegawai *before;
} ;

struct pegawai *Data_Pegawai = NULL; //Data Pegawai 
struct pegawai *first = NULL;//Menyimpan node awal dari Data_Pegawai

//Utility Umum**
void printBanner(const char *text, char border, int totalWidth) {
    printf("\n");
    int textLen = strlen(text);
    int side = (totalWidth - textLen - 2) / 2; // -2 for spaces around text
    for (int i = 0; i < side; i++) printf( "%c" , border);
    printf(YELLOW BOLD" %s "RESET, text);
    for (int i = 0; i < side; i++) printf( "%c" , border);
    printf("\n");
}

//Utility Umum**
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

//Utility Umum**
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
                printf(RED"Input tidak boleh kosong. Silakan coba lagi.\n"RESET);
                printf("\nperintah< ");
                continue;
            }
            
            // Cek tanda minus di depan (boleh 1 kali)
            if (buffer[0] == '-') i = 1;
            
            valid = 1; // Asumsi valid
            for (; i < (int)strlen(buffer); i++) {
                if (!isdigit(buffer[i])) {
                    valid = 0;
                    printf(RED"Input harus berupa angka bulat.\n"RESET);
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
            printf(RED"Terjadi kesalahan input. Silakan coba lagi.\n"RESET);
            printf("\nperintah< ");
            // Clear error stdin
            clearerr(stdin);
        }
    }
    return 0; // Tidak akan sampai sini, tapi agar compiler aman
}

//Utility Umum**
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
                printf(RED"\nInput tidak boleh kosong. Silakan coba lagi.\n"RESET);
                printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
                continue;
            }

            int i = 0;
            isValid = 1;
            for (; buffer[i] != '\0'; i++) {
                if (!isdigit(buffer[i])) {
                    isValid = 0;
                    printf(RED"\nInput harus berupa angka bulat positif.\n"RESET);
                    printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
                    break;
                }
            }

            if (isValid) {
                hasil = atoi(buffer);
                if (hasil <= 0) {
                    printf(RED"\nInput harus lebih besar dari nol.\n"RESET);
                    printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
                    isValid = 0;
                } else {
                    return hasil;
                }
            }
        } else {
            printf(RED"\nTerjadi kesalahan input. Silakan coba lagi.\n"RESET);
            printf("\nMasukkan jumlah maksimal shift yang disanggupi pegawai< ");
            clearerr(stdin); // reset status error stdin
        }
    }
    return 0; // fallback
}

// Eror Display kalo blm ada file dimuat **
void eror(){
    printf(RED"\nBelum ada File yang dimuat, Fitur Tidak bisa digunakan!!\n\n"RESET);
}

//Untuk Menghapus semua link list yang tersimpan**
void reset(){
    struct pegawai *temp = first;
    while(temp != NULL){
        struct pegawai *next = temp -> next;
        free(temp);
        temp = next;
    }
    first = NULL;
    Data_Pegawai = NULL;
}

//Penentu hari dari file
void preferensi_hari(int  *arr_preferensi,char *dest){    
    //Cek bebas
    if (arr_preferensi[2]==1&&arr_preferensi[1]==1&&arr_preferensi[0]==1)
        strcpy(dest,"pagi siang malam") ; //0 Bebas
    else if (arr_preferensi[2]==1&&arr_preferensi[1]==1)
        strcpy(dest,"siang malam");  //6 siang dan malam
    else if (arr_preferensi[0]==1&&arr_preferensi[2]==1)
        strcpy(dest,"pagi malam");  //5 pagi dan malam
    else if (arr_preferensi[0]==1&&arr_preferensi[1]==1)
        strcpy(dest,"pagi siang");  //4 pagi dan Siang
    else if (arr_preferensi[2]==1)
        strcpy(dest,"malam");  //3 malam
    else if (arr_preferensi[1]==1)
        strcpy(dest,"siang");  //2 siang
    else 
        strcpy(dest,"pagi");  //1 pagi
}

//Penentu  hari dari kata ke boolean
void preferensi_hari_inverse(char *source, int *dest){
    //Cek pagi
    if (strstr(source,"pagi")!=NULL||strstr(source,"Pagi")!=NULL)
        dest[0] = 1;
    //Cek Siang
    if (strstr(source,"siang")!=NULL||strstr(source,"Siang")!=NULL)
        dest[1] = 1;
    //Cek malam
    if (strstr(source,"malam")!=NULL||strstr(source,"Malam")!=NULL)
        dest[2] = 1;
}

//Penentu hari untuk tambah
void preferensi_hari_tambah(char *arr,char *dest){
    int T[3]={0};
    //Cek pagi
    if (strstr(arr,"pagi")!=NULL||strstr(arr,"Pagi")!=NULL)
        T[0] = 1;
    //Cek Siang
    if (strstr(arr,"siang")!=NULL||strstr(arr,"Siang")!=NULL)
        T[1] = 1;
    //Cek malam
    if (strstr(arr,"malam")!=NULL||strstr(arr,"Malam")!=NULL)
        T[2] = 1;
    //Cek bebas
    if (strstr(arr,"bebas")!=NULL||strstr(arr,"Bebas")!=NULL||strstr(arr,"Tidak Ada Preferensi")!=NULL||strstr(arr,"Fleksibel")!=NULL){
         T[0]=1;T[1]=1;T[2]=1;
    }    
    /*printf("T[0]:%d |T[1]:%d |T[2]%d\n",T[0],T[1],T[2]);*/
    if (T[2]==1&&T[1]==1&&T[0]==1)
        strcpy(dest,"Pagi Siang dan Malam") ; //0 Bebas
    else if (T[2]==1&&T[1]==1)
        strcpy(dest,"Siang dan Malam");  //6 siang dan malam
    else if (T[0]==1&&T[2]==1)
        strcpy(dest,"Pagi dan Malam");  //5 pagi dan malam
    else if (T[0]==1&&T[1]==1)
        strcpy(dest,"Pagi dan Siang");  //4 pagi dan Siang
    else if (T[2]==1)
        strcpy(dest,"Malam");  //3 malam
    else if (T[1]==1)
        strcpy(dest,"Siang");  //2 siang
    else 
        strcpy(dest,"Pagi");  //1 pagi
}

//Membuat File dari List data raw lama //Update Raw
void list2file(char *nama_file){
    struct pegawai *temp = first;
    FILE *a;
    a = fopen(nama_file,"w");
    fprintf(a,"\"Nama\",\"shift_maks\",\"Pagi\",\"Siang\",\"Malam\"\n");
    while (temp!= NULL){
        int arr[3]={0};
        preferensi_hari_inverse(temp->preferensi_shift,arr);
        fprintf(a,"%s,%d,%d,%d,%d\n",temp->nama,temp->maks_shift,arr[0],arr[1],arr[2]);
        temp = temp->next;
    }
    fclose(a);
    printf("Data telah tersimpan dalam file %s!!\n\n",nama_file);
}

//Membuat File dari List untuk output File yang digunakan ke algoritma pengacakan
void list2file_output(char *nama_file_raw,int jadwal_maker_param){
    struct pegawai *temp = first;
    FILE *a;
    a = fopen("Data_Dokter.csv","w");
    fprintf(a,"Status_Param:,%d,%s\n\"Nama\",\"shift_maks\",\"Preferensi_shift\"\n",jadwal_maker_param,nama_file_raw);
    while (temp!= NULL){
        fprintf(a,"%s,%d,%s\n",temp->nama,temp->maks_shift,temp->preferensi_shift);
        temp = temp->next;
    }
    fclose(a);
}

//Fungsi Status Display
void status_disp(char *nama_file,int jadwal_maker_param,int *choice){
    if (strcasecmp(nama_file,"NONE")==0) printf("Status:\nFile Yang Dimuat< "RED"%s\n"RESET,nama_file);
    else printf("Status:\nFile Yang Dimuat< "GREEN"%s\n"RESET,nama_file);
    if (jadwal_maker_param){
        printf("Jadwal Tersimpan< "GREEN"%s\n\n"RESET"Perintah< ","SAVE");
        *choice = input_integer();
    }
    else{
        printf("Jadwal Tersimpan< "RED"%s\n\n"RESET"Perintah< ","UNSAVE");
        *choice = input_integer();
    }
}

//Mengubah File Menjadi Linked List*
void file2list(char *data){
    int arr_preferensi[3]={0};
    struct pegawai *temp = malloc(sizeof(struct pegawai));
    //Beri node next
    temp->next = NULL;
    //Beri node before 
    if (Data_Pegawai == NULL){
       temp -> before = NULL;
       first = temp;
    }
    else{
        temp -> before = Data_Pegawai;
        Data_Pegawai -> next = temp;
    }
    //Ambil nama
    char *token ;
    char buffer_data[256];
    strcpy(buffer_data,data);
    token = strtok(buffer_data,",");
    strcpy(temp->nama,token);
    //Ambil Maks Shift
    token = strtok(NULL,",");
    sscanf(token,"%d",&(temp->maks_shift));
    //Ambil Preferensi Shift pagi
    token = strtok(NULL,",");
    sscanf(token,"%d",&arr_preferensi[0]);
    //Ambil Preferensi Shift siang
    token = strtok(NULL,",");
    sscanf(token,"%d",&arr_preferensi[1]);
    //Ambil Preferensi Shift malam
    token = strtok(NULL,",");
    sscanf(token,"%d",&arr_preferensi[2]);
    preferensi_hari(arr_preferensi,temp->preferensi_shift);
    Data_Pegawai  = temp;
}

//Fungsi utama Ambil data dari file csv*
void load_data(char *nama_file,char *nama_file_default,int *jadwal_maker_param,int *jadwalDibuat){
    if (strcasecmp(nama_file,"NONE")==0){
        eror();
    }
    else{
        FILE *csv1;
        char data[256];
        csv1 = fopen(nama_file,"r");
        if (csv1 == NULL){
            int choice=0;
            printf(RED"\nData belum ada, Tidak Berhasil Memuat File!!\n\n"RESET);
            strcpy(nama_file,nama_file_default);
            while (choice!=2&&choice!=1){
                printf("Apakah ingin membuat file baru??\n[1]Ya\n[2]Tidak\n\n");
                status_disp(nama_file,*jadwal_maker_param,&choice);
                switch (choice){
                case 2:
                    printf("\n");
                    break;
                case 1:
                    reset();
                    FILE *a;
                    a = fopen("Jadwal_Dokter.csv","w");
                    if (a!=NULL){
                        fprintf(a,"idDokter,Hari,Tipe_Shift,Nama_Dokter\n");
                        fclose(a);
                    }
                    *jadwal_maker_param= 0;
                    *jadwalDibuat = 0;
                    printf("\nMasukkan nama File yang ingin dibuat< ");
                    input_string(nama_file);printf("\n");
                    strcpy(nama_file_default,nama_file);
                    csv1 = fopen(nama_file,"w");
                    fprintf(csv1,"\"nama\",\"shift_maks\",\"Pagi\",\"Siang\",\"Malam\"\n");
                    fclose(csv1);
                    list2file_output(nama_file,*jadwal_maker_param);
                    break;
                default:
                    printf(RED"\nPerintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n\n"RESET);
                    break;
                }
            }
        }
        else{
            reset();
            if (strcasecmp(nama_file,nama_file_default)!=0){
                *jadwal_maker_param = 0;
            }
            *jadwalDibuat=0;
            strcpy(nama_file_default,nama_file);
            fgets(data,256,csv1);
            while (fgets(data,256,csv1)!=NULL){
                file2list(data);
            }
            printf("Data Berhasi Dimuat!!\n\n");
            fclose(csv1);
            list2file_output(nama_file_default,*jadwal_maker_param);
            FILE *a;
            a = fopen("Jadwal_Dokter.csv","w");
            if (a!=NULL){
                fprintf(a,"idDokter,Hari,Tipe_Shift,Nama_Dokter\n");
                fclose(a);
            }   
            printf("Data telah tersimpan dalam file %s!!\n\n","Data_Dokter.csv");
        }
    }
}

//Load khusus buat di valid
void  load_valid(char *nama_file,int jadwal_maker_param){
    FILE *csv1;
    csv1 = fopen(nama_file,"r");
    char data[256];
    fgets(data,256,csv1);
    while (fgets(data,256,csv1)!=NULL){
        file2list(data);
    }
    fclose(csv1);
    
    list2file_output(nama_file,jadwal_maker_param);
    reset();
}

//Fungsi Utama Menampilkan linked list*
void Tampil(char *nama_file){
    if (strcasecmp(nama_file,"NONE")==0){
        eror();
    }
    else if (Data_Pegawai == NULL){
        printf(RED"\nFile masih belum memiliki data!!\n\n"RESET);
    }
    else{
        struct pegawai *temp = first;
        int i = 1;
        printBanner("DATA DOKTER",'=',136);
        printf("%-6s%-50s%-39s%-20s\n","No","Nama Pegawai","Maksimal Shift(Per Minggu)","Preferensi Shift","");
        while (temp != NULL){
            if (temp->maks_shift>9){
                printf("%-6d%-50s%11s%d%26s%-25s\n",i,temp->nama," ",temp->maks_shift," ",temp->preferensi_shift);   
            }
            else{
                printf("%-6d%-50s%11s%d%27s%-25s\n",i,temp->nama," ",temp->maks_shift," ",temp->preferensi_shift);
            }
            temp = temp -> next;
            i++;
        }
        printf("\n");
    }
}

//Fungsi Menambah data*
void tambah(char *nama_file,int jadwal_maker_param,int *jadwalDibuat){
    struct pegawai *temp = malloc(sizeof(struct pegawai));
    char response[100];
    if (Data_Pegawai == NULL){
        temp-> before = NULL;
        first = temp;
    }
    else{
        temp -> before = Data_Pegawai;
        Data_Pegawai -> next = temp;
    }
    temp->next = NULL;
    //Memasukkan nama dari pegawai
    printf("\nMasukkan Nama dari Pegawai Baru< ");
    ;input_string(temp->nama);
    //Memasukkan jumlah maksimal shift yang diminta pegawai
    printf("Masukkan jumlah maksimal shift yang disanggupi pegawai< ");
    temp->maks_shift = input_integer_positif();
    //Memasukkan Preferensi Hari dari pegawai
    printf("Masukkan preferensi bagian shift dari pegawai< ");
    input_string(response);
    preferensi_hari_tambah(response,temp->preferensi_shift);
    Data_Pegawai = temp;
    printf("Data pegawai atas nama %s berhasil ditambahkan\n\n",temp->nama);
    //Update data dokter
    list2file_output(nama_file,jadwal_maker_param);
    list2file(nama_file);
    *jadwalDibuat = 0;
}

//Fungsi Menghapus Data*
void hapus(char *nama_file,int jadwal_maker_param,int *jadwalDibuat){
    if (Data_Pegawai == NULL){
       printf(RED"\nFile masih belum memiliki data!!"RESET);
    }
    else{
        int kembali = 0;
        struct pegawai *temp ;
        int find = 0;
        int i;
        char nama_pegawai[100];
        while (!find){
            printf("\nNama Pegawai yang ada dalam data:\n");
            temp = first;
            i = 1;
            while (temp != NULL){
                printf("[%d] %s\n",i,temp->nama);
                temp = temp->next;
                i++;
            }
            printf("\n[0] Ketik \"Kembali\" untuk kembali ke menu utama\n");
            printf("\nMasukkan nama pegawai yang ingin dihapus dari data< ");
            input_string(nama_pegawai);
            temp = first;
            while (temp!=NULL){
                if (strcasecmp(nama_pegawai,temp->nama)==0){
                    find = 1;
                    break;
                }
                temp = temp->next;
            }
            if (strcasecmp(nama_pegawai,"Kembali")==0){
                find = 1;
                kembali= 1;
            }
            if (!find){
                printf(RED"\nNama yang ingin dihapus tidak terdaftar dalam data!!\n"RESET);
            }
        }
        if (!kembali){
            struct pegawai *temp_r;
            struct pegawai *temp_l;
            if (first == Data_Pegawai){ //Kalau data nya cuma 1 dan kenak hapus
                first = NULL;
                Data_Pegawai = NULL;
                }
            else if(temp==first){ //Kalau yang dihapus data paling kiri, data lebih dari 1
                temp_r = temp->next;
                temp_r -> before = NULL;
                temp -> next = NULL;
                first = temp_r;
                }
            else if (temp == Data_Pegawai){ //Kalau yang dihapus data paling kanan, data lebih dari 1
                Data_Pegawai = temp->before;
                Data_Pegawai -> next = NULL;
                temp -> before = NULL;
                }
            else{ //Kalau yang dihapus data ditengah, data lebih dari 1
                temp_l = temp->before;
                temp_r = temp->next;
                temp->next = NULL;
                temp->before =NULL;
                temp_l -> next = temp_r;
                temp_r -> before = temp_l;
            }
            //Menyimpan ke output
            printf("\nData pegawai atas nama %s berhasil dihapus'n",temp->nama);
            jadwal_maker_param = 0;
            list2file_output(nama_file,jadwal_maker_param);
            list2file(nama_file);
            *jadwalDibuat = 0;
            free(temp);
        }
    }  
}

//Fungsi Utama dari Edit*
void edit(char *nama_file,int *jadwal_maker_param,int *jadwalDibuat){
    if (strcasecmp(nama_file,"NONE")==0){
        eror();
    }
    else{
        int choice;
        printf("\nPilih Opsi Pengeditan Data dibawah ini!!!\n[1]Tambah Data\n[2]Hapus Data\n[0]Kembali Ke Menu Utama Database\n\nPerintah< ");
        choice = input_integer();
        switch (choice){
        case 2:
            *jadwal_maker_param = 0;                
            hapus(nama_file,*jadwal_maker_param,jadwalDibuat);
            printf("\n");
            break;
        case 1:
            *jadwal_maker_param = 0;    
            tambah(nama_file,*jadwal_maker_param,jadwalDibuat);
            printf("\n");
            break;
        case 0:
            printf("\n");
            break;  
        default:
            printf(RED"\nPerintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n"RESET);
            edit(nama_file,jadwal_maker_param,jadwalDibuat);
            break;
        }
    }
}

//Fungsi untuk menampilkan arahan fitur*
void Fitur_display(){
    printf("Perintah yang bisa anda Masukkan!!!\n");
    printf("[1]%-35s||"BOLD ITALIC GREEN"Untuk memuat file Kedalam Sistem\n"RESET,"Muat File");
    printf("[2]%-35s||"BOLD ITALIC GREEN"Untuk menampilkan isi data yang dimuat\n"RESET,"Tampilkan Data");
    printf("[3]%-35s||"BOLD ITALIC GREEN"Untuk mengedit data\n"RESET,"Edit Data");
    printf("[4]%-35s||"BOLD ITALIC GREEN"Untuk menghapus data yang di-muat\n"RESET,"Reset Data");
    printf("[0]%-35s||"BOLD ITALIC GREEN"Untuk kembali ke fitur utama\n\n"RESET,"Keluar Database");
}

//Simpan Nama dari file
void Load_Nama_File(char *nama_file,char *nama_file_default,int *jadwal_maker_param){
    FILE *csv;
    csv = fopen("Data_Dokter.csv","r");
    if (csv != NULL){
        char *token;
        char buffer[100];
        fgets(buffer,100,csv);
        token = strtok(buffer,",");token=strtok(NULL,",");
        sscanf(token,"%d",jadwal_maker_param);
        token=strtok(NULL,",");
        int i=0;
        while (1){
            if (*(token+i)=='\n'){
                *(token+i)='\0';
                break;
            }
            i++;
        }
        
        strcpy(nama_file_default,token);
        strcpy(nama_file,nama_file_default);
        fclose(csv);
        FILE *csv1;
        char data[256];
        csv1 = fopen(nama_file,"r");
        reset();
        strcpy(nama_file_default,nama_file);
        fgets(data,256,csv1);
        while (fgets(data,256,csv1)!=NULL){
                file2list(data);
            }
        list2file_output(nama_file,*jadwal_maker_param);
        fclose(csv1);
    }
}

void reset_data(char *nama_file,char *nama_file_default,int *jadwal_maker_param){
    if (strcasecmp(nama_file,"NONE")!=0){
        *jadwal_maker_param = 0;
        reset();
        strcpy(nama_file,"NONE");
        strcpy(nama_file_default,nama_file);
        list2file_output(nama_file,*jadwal_maker_param);
        FILE *a;
        a = fopen("Jadwal_Dokter.csv","w");
        if (a!=NULL){
            fprintf(a,"idDokter,Hari,Tipe_Shift,Nama_Dokter\n");
            printf("\n\nData dalam file %s dan %s telah berhasil di reset!!\n\n","Data_Dokter.csv","Jadwal_Dokter.csv");
            fclose(a);
        }
        else{
            printf("\n\nData dalam file %s telah berhasil di reset!!\n\n","Data_Dokter.csv");
        }
        
    }
    else{
        eror();
    }
}

//Buat Masukin ke main utama
void Database(int *JadwalDibuat){
    int input=1 ;
    cetak_bingkai("SELAMAT DATANG DI DATABASE PUSAT");
    int jadwal_maker_param = 0;
    char nama_file[100]="NONE";
    char nama_file_default[100] ="NONE";
    Load_Nama_File(nama_file,nama_file_default,&jadwal_maker_param);
    while(input!=0){
        Fitur_display();
        status_disp(nama_file,jadwal_maker_param,&input);
        switch (input){
        case 1:
            //Memasukkan nama file
            printf("\nMasukkan nama data file Dokter< ");
            input_string(nama_file);
            load_data(nama_file,nama_file_default,&jadwal_maker_param,JadwalDibuat);
            printBanner("!!!DATABASE PUSAT!!!",'*',136);
            break;
        case 2:
            Tampil(nama_file);
            break;
        case 3:
            edit(nama_file,&jadwal_maker_param,JadwalDibuat);
            printBanner("!!!DATABASE PUSAT!!!",'*',136);
            break;
        case 4:
            reset_data(nama_file,nama_file_default,&jadwal_maker_param);
            *JadwalDibuat = 0;
            printBanner("!!!DATABASE PUSAT!!!",'*',136);
            break;
        case 0:
            reset();
            break;
        default:
            printf(RED"\nPerintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n"RESET);
            printBanner("!!!DATABASE PUSAT!!!",'*',136);
            break;
        }
    }   
}

