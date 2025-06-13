#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

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
    for (int i = 0; i < side; i++) printf("%c", border);
    printf(" %s ", text);
    for (int i = 0; i < side; i++) printf("%c", border);
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

// Eror Display kalo blm ada file dimuat **
void eror(){
    printf("\nBelum ada File yang dimuat, Fitur Tidak bisa digunakan!!\n\n");
}

//Untuk Menghapus semua link list yang tersimpan**
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

//Membuat File dari List*
void list2file(char *nama_file){
    struct pegawai *temp = first;
    FILE *a;
    a = fopen(nama_file,"w");
    fprintf(a,"\"Nama\",\"shift_maks\",\"Preferensi_shift\"\n");
    while (temp!= NULL){
        fprintf(a,"%s,%d,%s\n",temp->nama,temp->maks_shift,temp->preferensi_shift);
        temp = temp->next;
    }
    fclose(a);
    printf("Data telah tersimpan dalam file %s!!\n\n",nama_file);
}

//Penentu hari*
void preferensi_hari(char *arr,char *dest){
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
    if (strstr(arr,"bebas")!=NULL||strstr(arr,"Bebas")!=NULL){
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

//Fungsi Status Display*
void status_disp(char *nama_file, int autosave_param,int *choice){
    if (autosave_param){
       printf("Status:\nFile Yang Dimuat< %s\nAutosave Status< Aktif\n\nPerintah< ",nama_file);
       *choice = input_integer();
    }
    else{
        printf("Status:\nFile Yang Dimuat< %s\nAutosave Status< Nonaktif\n\nPerintah< ",nama_file);
        *choice = input_integer();
    }
}

//Mengubah File Menjadi Linked List*
void file2list(char *data){
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
    //Ambil Preferensi Shift
    token = strtok(NULL,",");
    preferensi_hari(token,temp->preferensi_shift);
    Data_Pegawai  = temp;
}

//Fungsi utama Ambil data dari file csv*
void load_data(char *nama_file,char *nama_file_default){
    if (strcasecmp(nama_file,"NONE")==0){
        eror();
    }
    else{
        FILE *csv1;
        char data[256];

        csv1 = fopen(nama_file,"r");
        if (csv1 == NULL){
            int choice=0;
            printf("Data belum ada, Tidak Berhasil Memuat File!!\n\n");
            strcpy(nama_file,nama_file_default);
            while (choice!=2&&choice!=1){
                printf("Apakah ingin membuat file baru??\n[1]Ya\n[2]Tidak\n\n");
                status_disp(nama_file,0,&choice);
                switch (choice){
                case 2:
                    printf("\n");
                    break;
                case 1:
                    reset(&first,&Data_Pegawai);
                    printf("\nMasukkan nama File yang ingin dibuat< ");
                    input_string(nama_file);printf("\n");
                    strcpy(nama_file_default,nama_file);
                    csv1 = fopen(nama_file,"w");
                    fprintf(csv1,"\"nama\",\"shift_maks\",\"Preferensi_shift\"\n");
                    fclose(csv1);
                    break;
                default:
                    printf("Perintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n\n");
                    break;
                }
            }
        }
        else{
            reset(&first,&Data_Pegawai);
            strcpy(nama_file_default,nama_file);
            fgets(data,256,csv1);
            while (fgets(data,256,csv1)!=NULL){
                file2list(data);
            }
            printf("Data Berhasi Dimuat!!\n\n");
            fclose(csv1);
        }
    }
}

//Fungsi Utama Menampilkan linked list*
void Tampil(char *nama_file){
    if (strcasecmp(nama_file,"NONE")==0){
        eror();
    }
    else if (Data_Pegawai == NULL){
        printf("\nFile masih belum memiliki data!!\n\n");
    }
    else{
        struct pegawai *temp = first;
        printBanner("DATA PEGAWAI",'=',100);
        printf("%-36s%-39s%-25s\n","Nama Pegawai","Maksimal Shift(Per Minggu)","Preferensi Shift");
        while (temp != NULL){
            printf("%-36s%11s%d%27s%-25s\n",temp->nama," ",temp->maks_shift," ",temp->preferensi_shift);
            temp = temp -> next;
        }
        printf("\n");
    }
}

//Fungsi Menambah data*
void tambah(int autosave_param,char *nama_file){
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
    preferensi_hari(response,temp->preferensi_shift);
    Data_Pegawai = temp;
    printf("Data pegawai atas nama %s berhasil ditambahkan",temp->nama);
    //Jika Autosave menyala, akan langsung update csv 
    if (autosave_param){
        list2file(nama_file);
    }
}

//Fungsi Menghapus Data*
void hapus(int autosave_param,char *nama_file){
    if (Data_Pegawai == NULL){
       printf("\nFile masih belum memiliki data!!");
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
                printf("[%d]%s\n",i,temp->nama);
                temp = temp->next;
                i++;
            }
            printf("[0]Ketik \"Kembali\" untuk kembali ke menu utama\n");
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
                printf("\nNama yang ingin dihapus tidak terdaftar dalam data!!\n");
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
            //Kalo autosave nyala akan langsung update file
            if (autosave_param){
                list2file(nama_file);
                }
            printf("Data pegawai atas nama %s berhasil dihapus",temp->nama);
            free(temp);
        }
    }  
}

//Fungsi Utama dari Edit*
void edit(char *nama_file, int autosave_param){
    if (strcasecmp(nama_file,"NONE")==0){
        eror();
    }
    else{
        int choice;
        printf("\nPilih Opsi Pengeditan Data dibawah ini!!!\n[1]Tambah Data\n[2]Hapus Data\n[0]Kembali Ke Menu Utama Database\n\nPerintah< ");
        choice = input_integer();
        switch (choice){
        case 2:
            hapus(autosave_param,nama_file);
            printf("\n\n");
            break;
        case 1:
            tambah(autosave_param,nama_file);
            printf("\n\n");
            break;
        case 0:
            printf("\n");
            break;  
        default:
            printf("Perintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n");
            edit(nama_file,autosave_param);
            break;
        }
    }
}

//Fungsi Utama Simpan*
void save(char *nama_file_def){
    if (strcasecmp(nama_file_def,"NONE")==0){
        eror();
    }
    else{
        int choice;
        printf("\nPilih Opsi Berikut: \n[1]Simpan dalam file baru\n[2]Update File lama\n[0]Kembali ke Menu Utama Database\n\nPerintah< ");
        choice = input_integer();
        char nama_file[50];
        switch (choice){
        case 1:
            printf("\nMasukkan nama file< ");
            input_string(nama_file);
            list2file(nama_file);
            break;
        case 2:
            list2file(nama_file_def);
            break;
        default:
            break;
        } 
    }
}

//Fungsi untuk menampilkan arahan fitur*
void Fitur_display(int  autosave_param){
    if (autosave_param){
        printf("Perintah yang bisa anda Masukkan!!!\n");
        printf("[1]Muat File\n[2]Tampilkan Data\n[3]Edit Data\n[4]Simpan Data\n[5]Muat Ulang File\n[6]Matikan Autosave\n[0]Keluar Database\n\n");
    }
    else{
        printf("Perintah yang bisa anda Masukkan!!!\n");
        printf("[1]Muat File\n[2]Tampilkan Data\n[3]Edit Data\n[4]Simpan Data\n[5]Muat Ulang File\n[6]Nyalakan Autosave\n[0]Keluar Database\n\n");
    }
    
}

//Program Utama
int main(){
    int input ;
    int autosave_param=0;
    char nama_file[100]="NONE";
    char nama_file_default[100]="NONE";
    printBanner("SELAMAT DATANG",'*',100);
    Fitur_display(autosave_param);
    status_disp(nama_file,autosave_param,&input);
    while(input!=0){
        switch (input){
        case 1:
            //Memasukkan nama file
            printf("\nMasukkan nama file pegawai< ");
            input_string(nama_file);
            load_data(nama_file,nama_file_default);
            break;
        case 2:
            Tampil(nama_file);
            break;
        case 3:
            edit(nama_file,autosave_param);
            break;
        case 4:
            save(nama_file);
            break;
        case 5:
            reset(&first,&Data_Pegawai);
            load_data(nama_file,nama_file_default);
            break;
        case 6:
            if (strcasecmp(nama_file,"NONE")==0){
                eror();
            }
            else{
                autosave_param = !autosave_param;
                if (autosave_param){
                    printf("\nFitur Autosave telah dinyalakan\n\n");
                }
                else {
                    printf("\nFitur Autosave telah dimatikan\n\n");
                }
            }
            break;
        case 0:
            break;
        default:
            printf("Perintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n\n");
            break;
        }
        Fitur_display(autosave_param);
        status_disp(nama_file,autosave_param,&input);
    }
}
