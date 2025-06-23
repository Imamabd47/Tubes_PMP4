#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Database.h"
#define ITALIC "\x1b[3m"
#define RESET  "\x1b[0m"
#define BOLD   "\x1b[1m"
#define RED    "\x1b[31m"
#define YELLOW  "\x1b[93m"
#define F_YELLOW  "\x1b[33m"
#define GREEN   "\x1b[32m"
// =====================================================================
// --- BAGIAN PENDEFINISIAN DAN INISIASI STRUCT ---
// =====================================================================

#define MAX_DOKTER 500 // Bebas diubah asal jangan kurang dari jumlah dokter yang ada di .CSV

typedef enum
{
    PAGI,
    SIANG,
    MALAM
} TipeShift;

typedef struct
{
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

struct datadokter
{
    char *nama;
    int shift_awal;
    int shift;
    char *preferensi;
    struct datadokter *next;
} *head = NULL, *tail = NULL, *current;

int jadwalSudahDibuat = 0;
char jadwalOtomatis[30][3][6][100] = {0};
Dokter daftarDokter[MAX_DOKTER];
Shift jadwalFinal[30 * 3 * 6];
int jumlahDokter = 0;
int totalShiftTerisi = 0;
// =====================================================================
// --- BAGIAN OJAN & DZIKRI ---//Jadwal Maker
// =====================================================================

void cetak_bingkai(const char *judul) {
    const int panjang_total = 136;  // Total panjang baris
    int panjang_judul = strlen(judul);
    int sisa = panjang_total - 8 - panjang_judul;  // 8 karakter untuk "// ---  ---"
    int kiri = sisa / 2;
    int kanan = sisa - kiri;

    // Baris atas
    for (int i = 0; i < panjang_total; i++) printf("=");
    printf("\n");

    // Baris judul
    printf("// ");
    for (int i = 0; i < kiri; i++) printf("-");
    printf(YELLOW BOLD" %s "RESET, judul);
    for (int i = 0; i < kanan; i++) printf("-");
    printf(" //\n");

    // Baris bawah
    for (int i = 0; i < panjang_total; i++) printf("=");
    printf("\n");
}

void tambahNode(char *nama, int shift, char *preferensi)
{
    current = (struct datadokter *)malloc(sizeof(struct datadokter));
    current->nama = (char *)malloc(strlen(nama) + 1);
    current->preferensi = (char *)malloc(strlen(preferensi) + 1);
    strcpy(current->nama, nama);
    strcpy(current->preferensi, preferensi);
    current->shift_awal = current->shift = shift;
    current->next = NULL;

    if (head == NULL)
    {
        head = tail = current;
    }
    else
    {
        tail->next = current;
        tail = current;
    }
}

int konversiDataDokter()
{
    int jumlahDokter = 0;
    for (struct datadokter *node = head; node != NULL && jumlahDokter < MAX_DOKTER; node = node->next, jumlahDokter++)
    {
        daftarDokter[jumlahDokter].id = jumlahDokter;
        strcpy(daftarDokter[jumlahDokter].nama, node->nama);
        strcpy(daftarDokter[jumlahDokter].preferensi, node->preferensi);
        daftarDokter[jumlahDokter].total_shift_dijadwalkan = 0;
        daftarDokter[jumlahDokter].pelanggaran_preferensi_terjadi = 0;
    }
    return jumlahDokter;
}

void readFile()
{
    FILE *fptr = fopen("Data_Dokter.csv", "r");
    if (!fptr)
    {
        printf("File tidak dapat dibuka. \n");
        return;
    }
    char buf[256];
    fgets(buf, sizeof(buf), fptr);fgets(buf, sizeof(buf), fptr);
    while (fgets(buf, sizeof(buf), fptr))
    {
        if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0';
        char *nama = strtok(buf, ",");
        char *shiftStr = strtok(NULL, ",");
        char *pref = strtok(NULL, ",");
        if (nama && shiftStr && pref)
        {
            tambahNode(nama, atoi(shiftStr), pref);
        }
    }
    fclose(fptr);
    jumlahDokter = konversiDataDokter(daftarDokter);
}

void tampilkanDokter()
{
    struct datadokter *curr = head;
        int i = 1;
        printBanner("DATA DOKTER",'=',136);
        printf("%-6s%-50s%-39s%-20s\n","No","Nama Pegawai","Maksimal Shift(Per Minggu)","Preferensi Shift","");
        while (curr != NULL){
            if (curr->shift>9){
                printf("%-6d%-50s%11s%d%26s%-25s\n",i,curr->nama," ",curr->shift," ",curr->preferensi);   
            }
            else{
                printf("%-6d%-50s%11s%d%27s%-25s\n",i,curr->nama," ",curr->shift," ",curr->preferensi);
            }
            curr = curr -> next;
            i++;
        }
        printf("\n");
}

void bersihkanMemori()
{
    struct datadokter *curr = head;
    while (curr != NULL)
    {
        struct datadokter *next = curr->next;
        free(curr->nama);
        free(curr->preferensi);
        free(curr);
        curr = next;
    }
    current = NULL;
    tail = NULL;
    head = NULL;
}

void resetShiftMingguan()
{
    struct datadokter *curr = head;
    while (curr != NULL)
    {
        curr->shift = curr->shift_awal;
        curr = curr->next;
    }
}

void acakUrutanDokter() {
    if (head == NULL || head->next == NULL) return;
    
    // Hitung jumlah dokter
    int count = 0;
    struct datadokter *curr = head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    
    // Konversi linked list ke array
    struct datadokter **arr = malloc(count * sizeof(struct datadokter));
    curr = head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1); // rand() untuk mengacak indeks dokter 
        struct datadokter *temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    // Buat linked list kembali dari array yang sudah diacak
    head = arr[0];
    tail = arr[count-1];
    for (int i = 0; i < count-1; i++) {
        arr[i]->next = arr[i+1];
    }
    arr[count-1]->next = NULL;
    
    free(arr);
}

int cekPreferensi(const char *preferensi_dokter, const char *shift)
{
    return (strstr(preferensi_dokter, shift) != NULL);
}

int konversiJadwal()
{
    int totalShiftTerisi = 0;
    char *shiftLabel[] = {"pagi", "siang", "malam"};
    int max_dokter_per_shift_arr[] = {6, 6, 5};

    for (int i = 0; i < jumlahDokter; i++)
    {
        daftarDokter[i].total_shift_dijadwalkan = 0;
        daftarDokter[i].pelanggaran_preferensi_terjadi = 0;
    }

    for (int h = 0; h < 30; h++)
    {
        for (int s = 0; s < 3; s++)
        {
            for (int d = 0; d < max_dokter_per_shift_arr[s]; d++)
            {
                if (strlen(jadwalOtomatis[h][s][d]) > 0)
                {
                    char *namaDokter = jadwalOtomatis[h][s][d];
                    int idDokter = -1;
                    for (int i = 0; i < jumlahDokter; i++)
                    {
                        if (strcmp(daftarDokter[i].nama, namaDokter) == 0)
                        {
                            idDokter = i;
                            break;
                        }
                    }
                    if (idDokter != -1)
                    {
                        jadwalFinal[totalShiftTerisi].hari = h + 1;
                        jadwalFinal[totalShiftTerisi].tipe = (TipeShift)s;
                        jadwalFinal[totalShiftTerisi].id_dokter = idDokter;
                        daftarDokter[idDokter].total_shift_dijadwalkan++;
                        if (!cekPreferensi(daftarDokter[idDokter].preferensi, shiftLabel[s]))
                        {
                            daftarDokter[idDokter].pelanggaran_preferensi_terjadi++;
                        }
                        totalShiftTerisi++;
                    }
                }
            }
        }
    }
    return totalShiftTerisi;
}

void jadwalotomatis30hari() {
    char *shift[] = {"pagi", "siang", "malam"};
    int maks[] = {6, 6, 5}; // Maksimal jumlah dokter per shift (pagi dan siang 6 dokter, malam 5 dokter)
    char pola_mingguan[7][3][6][100] = {{{{0}}}}; // 
    
    // Array untuk mengecek apakah sudah terdapat dokter di setiap shift
    char dokter_per_shift[3][100][100];
    int jumlah_dokter_per_shiift[3] = {0};

    // Assign jadwal dokter untuk 7 hari pertama
    for (int hari = 0; hari < 7; hari++) {
        acakUrutanDokter();
        
        // Inisialisasi jumlah dokter di setiap shiftnya masih 0
        for (int s = 0; s < 3; s++) {
            jumlah_dokter_per_shiift[s] = 0;
        }

        int count[3] = {0};
        
        // Alokasi berdasarkan preferensi shift dokter
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                // Cek apakah dokter sudah bekerja di shift ini hari ini
                int sudah_kerja = 0;
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0 && cekPreferensi(curr->preferensi, shift[s])) {
                    strcpy(pola_mingguan[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
        
        // Alokasi sisa slot dengan dokter yang belum kerja di shift ini
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                int sudah_kerja = 0;
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0) {
                    strcpy(pola_mingguan[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
    }

    // Pola repetitif per minggu untuk 4 minggu (28 hari)
    for (int minggu = 0; minggu < 4; minggu++) {
        for (int hari = 0; hari < 7; hari++) {
            int hari_ini = minggu * 7 + hari;
            if (hari_ini >= 30) break;
            
            for (int s = 0; s < 3; s++) {
                for (int d = 0; d < maks[s]; d++) {
                    strcpy(jadwalOtomatis[hari_ini][s][d], pola_mingguan[hari][s][d]);
                }
            }
        }
    }
    resetShiftMingguan(); // Untuk mereset minggu berikutnya

    // Untuk sisa 2 hari
    for (int hari = 28; hari < 30; hari++) {
        acakUrutanDokter();
        
        // Inisialisasi jumlah dokter di setiap shiftnya masih 0
        for (int s = 0; s < 3; s++) {
            jumlah_dokter_per_shiift[s] = 0;
        }

        int count[3] = {0};
        
        // Alokasi berdasarkan preferensi shift dokter
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                // Cek apakah dokter sudah bekerja di shift ini hari ini
                int sudah_kerja = 0; 
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0 && cekPreferensi(curr->preferensi, shift[s])) {
                    strcpy(jadwalOtomatis[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
        
        // Alokasi sisa slot dengan dokter yang belum kerja di shift ini
        for (int s = 0; s < 3; s++) {
            struct datadokter *curr = head;
            while (curr && count[s] < maks[s]) {
                int sudah_kerja = 0;
                for (int i = 0; i < jumlah_dokter_per_shiift[s]; i++) {
                    if (strcmp(dokter_per_shift[s][i], curr->nama) == 0) {
                        sudah_kerja = 1;
                        break;
                    }
                }
                
                if (!sudah_kerja && curr->shift > 0) {
                    strcpy(jadwalOtomatis[hari][s][count[s]++], curr->nama);
                    strcpy(dokter_per_shift[s][jumlah_dokter_per_shiift[s]++], curr->nama);
                    curr->shift--;
                }
                curr = curr->next;
            }
        }
    }
}

//Fungsi untuk Pembuatan Jadwal Otomatis
void jadwal_maker(){
    bersihkanMemori();
    readFile();
    jadwalotomatis30hari();
    printf("\nMembuat jadwal baru...\n");
    totalShiftTerisi = konversiJadwal();
    jadwalSudahDibuat = 1;
    printf("Jadwal telah berhasil dibuat.\n");
    bersihkanMemori();
    //Mereset Jadwal_Dokter.csv
    FILE *csv;
    csv = fopen("Jadwal_Dokter.csv","w");
    if (csv!=NULL){
        fprintf(csv,"idDokter,Hari,Tipe_Shift,Nama_Dokter\n");
        fclose(csv);
    }
}

//Fungsi Fitur Display
void fitur_display_jadwal(int *choice,char *nama_file,int jadwal_maker_param){
    printf("\nSilahkan pilih Opsi Dari bagian berikut!!\n");
    printf("[1]%-35s||"BOLD ITALIC GREEN"Untuk melihat data\n"RESET,"Tampilkan Data");
    printf("[2]%-35s||"BOLD ITALIC GREEN"Untuk masuk kedalam sistem pembuatan jadwal\n"RESET,"Buat Jadwal Otomatis 30 Hari");
    printf("[0]%-35s||"BOLD ITALIC GREEN"Untuk kembali ke menu utama\n\n"RESET,"Keluar Jadwal_Maker");
    if (strcasecmp(nama_file,"NONE")==0) printf("Status:\nFile Yang Dimuat< "RED"%s\n"RESET,nama_file);
    else printf("Status:\nFile Yang Dimuat< "GREEN"%s\n"RESET,nama_file);
    if (jadwal_maker_param) printf("Jadwal Tersimpan< "GREEN"SAVE\n\n"RESET"Perintah< ");
    else printf("Jadwal Tersimpan< "RED"UNSAVE\n\n"RESET"Perintah< ",nama_file);
    *choice = input_integer();
}

//Fungsi menu_jadwal_maker : UTAMA BAGIAN OJAN DAN DZIKRI
void menu_jadwal_maker(char *nama_file,int *jadwal_maker_param,int *jadwalDibuat){
    int choice=1;
    bersihkanMemori();
    int adalist = 0;
    readFile();
    if (current != NULL) adalist = 1;
    if (adalist){
        cetak_bingkai("MENU BUAT JADWAL!!!");
        while (choice!=0){
            fitur_display_jadwal(&choice,nama_file,*jadwal_maker_param);
            bersihkanMemori();
            readFile();
            if (current==NULL){
                choice = 0;
                printf(RED"\nFile anda kosong, Tolong Untuk Masukkan File Yang sesuai Format!! T_T\n"RESET); 
            }
            switch (choice){
            case 1:
                tampilkanDokter();
                printBanner("MENU BUAT JADWAL",'*',136); 
                break;
            case 2:
                jadwal_maker();
                *jadwal_maker_param = 0;
                *jadwalDibuat = jadwalSudahDibuat;
                load_valid(nama_file,*jadwal_maker_param);
                printBanner("MENU BUAT JADWAL",'*',136); 
                break;
            case 0:
                break;
            default:
                printf(RED"\nPerintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n"RESET);
                printBanner("MENU BUAT JADWAL",'*',136); 
                break;
            }
        }
    }
    else  printf(RED"\nFile anda kosong, Tolong Untuk Masukkan File Yang sesuai Format!! T_T\n"RESET);  
}


// =====================================================================
// --- BAGIAN ADHI & LUNA ---
// =====================================================================

void bersihkanBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

const char *getNamaShift(TipeShift tipe)
{
    switch (tipe)
    {
    case PAGI:
        return "Pagi";
    case SIANG:
        return "Siang";
    case MALAM:
        return "Malam";
    default:
        return "N/A";
    }
}

void tampilkanJadwalHarian(int hari) {
    const int LEBAR_TOTAL = 137;

    // 1. Cetak Header dengan Nomor Hari
    char headerText[100];
    sprintf(headerText, BOLD YELLOW" JADWAL JAGA HARI KE-%d "RESET, hari);
    int lenHeaderText = strlen(headerText);
    int padding = ((LEBAR_TOTAL - lenHeaderText)+16) / 2;

    for (int i = 0; i < LEBAR_TOTAL; i++) { printf("="); }
    printf("\n");
    printf("%*s%s\n", padding, "", headerText);
    for (int i = 0; i < LEBAR_TOTAL; i++) { printf("-"); }
    printf("\n");

    // 2. Loop untuk setiap shift
    for (int s = 0; s < 3; s++) { // s=0 (Pagi), s=1 (Siang), s=2 (Malam)
        
        // Siapkan buffer untuk menampung daftar nama dokter
        char daftarNamaBuffer[1000] = {0}; // Inisialisasi ke nol agar string kosong
        int dokterDitemukan = 0;

        // Loop melalui semua data jadwal yang ada
        for (int i = 0; i < totalShiftTerisi; i++) {
            // KONDISI PENTING: cocokkan hari DAN tipe shift
            if (jadwalFinal[i].hari == hari && jadwalFinal[i].tipe == (TipeShift)s) {
                // Jika cocok, tambahkan nama dokter ke buffer
                strcat(daftarNamaBuffer, daftarDokter[jadwalFinal[i].id_dokter].nama);
                strcat(daftarNamaBuffer, "; ");
                dokterDitemukan++;
            }
        }
        
        // Siapkan string konten akhir yang akan dicetak
        char kontenCetak[1000];
        if (dokterDitemukan == 0) {
            strcpy(kontenCetak, "(Kosong)");
        } else {
            // Hapus "; " di akhir string agar lebih rapi
            daftarNamaBuffer[strlen(daftarNamaBuffer) - 2] = '\0';
            strcpy(kontenCetak, daftarNamaBuffer);
        }

        // 3. Cetak baris yang sudah diformat dengan rapi
        char prefix[30];
        sprintf(prefix, "| Shift %-5s : ", getNamaShift((TipeShift)s));
        
        int sisaRuang = LEBAR_TOTAL - strlen(prefix) - 2; // -2 untuk '|' di awal dan akhir
        
        printf("%s", prefix);
        printf("%-*s |\n", sisaRuang, kontenCetak);
    }

    // 4. Cetak garis bawah
    for (int i = 0; i < LEBAR_TOTAL; i++) { printf("="); }
    printf("\n\n");
}

void tampilkanJadwalMingguan(int minggu)
{
    if (minggu < 1 || minggu > 5)
    {
        printf(RED"Minggu tidak valid.\n"RESET);
        return;
    }
    int hariMulai = (minggu - 1) * 7 + 1;

    int hariSelesai;
    if (hariMulai + 6 > 30)
    {
        hariSelesai = 30;
    }
    else
    {
        hariSelesai = hariMulai + 6;
    }

    char buffer[256];
    sprintf(buffer,"--- JADWAL MINGGU KE-%d (HARI %d s/d %d) ---", minggu, hariMulai, hariSelesai);
    printBanner(buffer,'*',137);
    for (int h = hariMulai; h <= hariSelesai; h++)
    {
        if (h > 30)
            break;
        tampilkanJadwalHarian(h);
    }
}

void tampilkanJadwalSebulan()
{
    cetak_bingkai("*** TAMPILAN JADWAL SEBULAN PENUH ***");
    for (int m = 1; m <= 5; m++)
    {
        int hariMulai = (m - 1) * 7 + 1;
        if (hariMulai > 30)
            break;
        tampilkanJadwalMingguan(m);
    }
}

void tampilkanLaporanPelanggaran()
{
    printf("\n--- Laporan Detail Pelanggaran Preferensi Shift ---\n");
    int totalPelanggaran = 0;
    int dokterdilanggar = 0;
    for (int i = 0; i < jumlahDokter; i++)
    {
        if (daftarDokter[i].pelanggaran_preferensi_terjadi > 0)
        {
            printf("Dr. %-20s | Jumlah Pelanggaran: %d\n",
                   daftarDokter[i].nama,
                   daftarDokter[i].pelanggaran_preferensi_terjadi);
            totalPelanggaran += daftarDokter[i].pelanggaran_preferensi_terjadi;
            dokterdilanggar++;
        }
    }
    if (dokterdilanggar == 0)
    {
        printf("Tidak ditemukan pelanggaran preferensi dalam jadwal ini.\n");
    }
    else
    {
        printf("---------------------------------------------------\n");
        printf("Total dokter yang melanggar: %d\n", dokterdilanggar);
        printf("Total semua pelanggaran: %d\n", totalPelanggaran);
    }
}

void simpanJadwalKeCSV(){
    FILE *fptr = fopen("Jadwal_Dokter.csv", "w");
    if (!fptr)
    {
        printf("ERROR: Tidak dapat membuat file '%s' untuk penulisan.\n", "Jadwal_Dokter.csv");
        return;
    }

    printf("\nMenyimpan jadwal ke file '%s'", "Jadwal_Dokter.csv");

    fprintf(fptr, "idDokter,Hari,Tipe_Shift,Nama_Dokter\n");

    for (int i = 0; i < totalShiftTerisi; i++)
    {
        int hari = jadwalFinal[i].hari;
        const char *namaShift = getNamaShift(jadwalFinal[i].tipe);
        int idDokter = jadwalFinal[i].id_dokter;
        if (idDokter >= 0) 
        {

            const char *namaDokter = daftarDokter[idDokter].nama;
            
            fprintf(fptr, "%d,%d,%s,%s\n", idDokter,hari, namaShift, namaDokter);
        }
    }

    fclose(fptr);
    printf(" Berhasil.\n");
}

void cariJadwalDokter()
{
    char namaCari[100];
    int idDokter = -1;
    int shiftDitemukan = 0;

    printf("\nMasukkan nama lengkap dokter yang ingin dicari: ");
    scanf(" %[^\n]", namaCari);
    bersihkanBuffer();

    for (int i = 0; i < jumlahDokter; i++)
    {
        if (strcmp(daftarDokter[i].nama, namaCari) == 0)
        {
            idDokter = daftarDokter[i].id;
            break;
        }
    }

    if (idDokter == -1)
    {
        printf("Dokter dengan nama '%s' tidak ditemukan dalam data.\n", namaCari);
        return;
    }

    printf("\n--- Jadwal Jaga Lengkap untuk Dr. %s ---\n", daftarDokter[idDokter].nama);
    for (int i = 0; i < totalShiftTerisi; i++)
    {
        if (jadwalFinal[i].id_dokter == idDokter)
        {
            printf(" -> Hari ke-%-2d, Shift %s\n", jadwalFinal[i].hari, getNamaShift(jadwalFinal[i].tipe));
            shiftDitemukan++;
        }
    }

    if (shiftDitemukan == 0)
    {
        printf("Dr. %s tidak memiliki jadwal jaga pada bulan ini.\n", daftarDokter[idDokter].nama);
    }
    else
    {
        printf("----------------------------------------------\n");
        printf("Total shift bulan ini: %d\n", shiftDitemukan);
    }
}

void muat_jadwal() {
    readFile();
    FILE *fptr = fopen("Jadwal_Dokter.csv", "r");
    if (!fptr) {
        printf(RED "ERROR: Gagal membuka file jadwal CSV '%s'.\n" RESET, "Jadwal_Dokter.csv");
    }

    char buffer[256];
    // Lewati baris header
    if (fgets(buffer, sizeof(buffer), fptr) == NULL) {
        fclose(fptr);
    }

    int shiftCount = 0;
    // Baca setiap baris data jadwal
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        // Pecah baris menjadi 4 bagian
        char *idStr = strtok(buffer, ",");
        char *hariStr = strtok(NULL, ",");
        char *tipeStr = strtok(NULL, ",");
        // Kolom nama tidak perlu disimpan ke variabel, strtok akan melewatinya

        if (!idStr || !hariStr || !tipeStr) {
            continue; // Lewati baris yang formatnya salah
        }

        // --- Proses Konversi Langsung ---
        // 1. Konversi ID (string -> int)
        int idDokter = atoi(idStr);

        // 2. Konversi Hari (string -> int)
        int hari = atoi(hariStr);

        // 3. Konversi Tipe Shift (string -> enum)
        TipeShift tipe;
        if (strcmp(tipeStr, "Pagi") == 0) tipe = PAGI;
        else if (strcmp(tipeStr, "Siang") == 0) tipe = SIANG;
        else if (strcmp(tipeStr, "Malam") == 0) tipe = MALAM;
        else continue; // Lewati jika tipe shift tidak dikenal

        // Simpan hasil konversi langsung ke dalam array Shift
        jadwalFinal[shiftCount].id_dokter = idDokter;
        jadwalFinal[shiftCount].hari = hari;
        jadwalFinal[shiftCount].tipe = tipe;
        shiftCount++;
        
        // Pencegahan buffer overflow
        if (shiftCount >= (30 * 3 * 6)) {
             break;
        }
    }

    fclose(fptr);
    totalShiftTerisi = shiftCount;
    printf(F_YELLOW "\nBerhasil memuat %d entri jadwal dari file '%s' menggunakan ID.\n" RESET, shiftCount, "Jadwal_Dokter.csv");
}

//Fungsi fitur display lihat jadwal
void fitur_display_lihatJadwal(int *choice,char *nama_file,int jadwal_maker_param){
    printf("\nSilahkan pilih Opsi Dari bagian berikut!!\n");
    printf("[1]%-35s||"BOLD ITALIC GREEN"Untuk menampilkan jadwal per hari\n"RESET,"Tampilkan Jadwal Harian");
    printf("[2]%-35s||"BOLD ITALIC GREEN"Untuk menampilkan jadwal per minggu\n"RESET,"Tampilkan Jadwal Mingguan");
    printf("[3]%-35s||"BOLD ITALIC GREEN"Untuk menampilkan jadwal per bulan\n"RESET,"Tampilkan Jadwal Sebulan Penuh");
    printf("[4]%-35s||"BOLD ITALIC GREEN"Untuk menampilkan jadwal per dokter\n"RESET,"Cari Jadwal Dokter");
    printf("[5]%-35s||"BOLD ITALIC GREEN"Untuk menampilkan jadwal per minggu\n"RESET,"Laporan pelanggaran");
    printf("[6]%-35s||"BOLD ITALIC GREEN"Untuk menyimpan jadwal ke dalam csv\n"RESET,"Simpan Jadwal ke File CSV");
    printf("[0]%-35s||"BOLD ITALIC GREEN"Untuk kembali ke menu utama\n\n"RESET,"Keluar lihat jadwal");
    if (strcasecmp(nama_file,"NONE")==0) printf("Status:\nFile Yang Dimuat< "RED"%s\n"RESET,nama_file);
    else printf("Status:\nFile Yang Dimuat< "GREEN"%s\n"RESET,nama_file);
    if (jadwal_maker_param) printf("Jadwal Tersimpan< "GREEN"SAVE\n\n"RESET"Perintah< ");
    else printf("Jadwal Tersimpan< "RED"UNSAVE\n\n"RESET"Perintah< ");
    *choice = input_integer();
}

//Fungsi Menut Utama Lihat Jadwal : UTAMA BAGIAN LUNA DAN ADI
void menu_lihat_jadwal(char *nama_file,int *jadwal_maker_param){
    int choice=1;
    int hari,minggu;
    if (!jadwalSudahDibuat&&!(*(jadwal_maker_param))){
        printf(RED "\n[!] Silakan buat jadwal terlebih dahulu\n" RESET);
    }
    else {
        cetak_bingkai("MENU LIHAT JADWAL!!!");
        if (!jadwalSudahDibuat){
            muat_jadwal();
        }
        while (choice!=0){
            fitur_display_lihatJadwal(&choice,nama_file,*jadwal_maker_param);
            switch (choice){
            case 1:
                printf("Masukkan hari (1-30): ");
                hari = input_integer_positif();
                tampilkanJadwalHarian(hari);
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            case 2:
                printf("Masukkan minggu (1-5): ");
                minggu = input_integer_positif();
                tampilkanJadwalMingguan(minggu);
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            case 3:
                tampilkanJadwalSebulan();
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            case 4:
                cariJadwalDokter();
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            case 5:
                tampilkanLaporanPelanggaran();
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            case 6:
                simpanJadwalKeCSV();
                *jadwal_maker_param = 1;
                bersihkanMemori();
                load_valid(nama_file,*jadwal_maker_param);    
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            case 0:
                bersihkanMemori();
                break;
            default:
                printf(RED"\nPerintah yang anda Masukkan Salah!!!Tolong Input dengan BenarT_T\n"RESET);
                printBanner("!!!MENU LIHAT JADWAL!!!",'*',136);
                break;
            }
        }
    }
}

