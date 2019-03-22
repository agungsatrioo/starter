#include <stdio.h>
#include <windows.h>

#define RAND_MAX 32767
#define CONSOLE_OUTPUT_WINDOW 0xfffffff5
#define DEFAULT 7
#define LIGHTGREEN  10
#define LIGHTRED  12

struct player {
    int session;
    int day;
    int* val;
    int reputation;
    int stocks[6];
} player;

struct save_data {
    char buff[255];
    char* data[80];
    char* data0[80];
    char s[80];
    char** temp;
    char** buffer[16];
} sav;

struct economy {
    int currs;
    int currs_before;
    int rate;
    int rateDisplay;
} economy;

struct market {
    char *name;
    int price;
    int priceInVal;
    int stock;
} market[6];

struct news {
    char *news0;
    char *politik;
} news;

char** fbuffer[80];
int rate_min, rate_max;
int choice;
int condition;
int tax;
char *trade_strings;
char *items[6]={"Usus","Ecer","Jell","Sonai","Tosima","Ledovo"};


int main() {
    economy.currs=5000;
    check_session();
}

void codestr(int type,char str[80]) {
    int i;
    for(i=0;i<=80;i++) {
        str[i] -=15;
    }
}

void check_session() {
    int i;
    char fname[32];
    if(player.session==1) {
        ganti_hari();
        display();
    } else {
        system("cls");
        player.day=0;
        player.reputation=10;
        player.val=10000000;

        for(i=0;i<=5;i++) {
            player.stocks[i]=0;
        }

        printf("Selamat datang di Starter.\n\nPilih tindakan:\n1. Permainan baru\n2. Muat permainan lama\n\nPilih: ");
        scanf("%d", &choice); fflush(stdin);
        switch(choice) {
        case 1:
            player.val = 10000000;
            player.session=1;
            check_session();
            break;
        case 2:
            printf("Masukkan nama permainan (mis. game1.sav): ");
            gets(fname); fflush(stdin);

            switch(readsave(0,fname)) {
            case ERROR_SUCCESS:
                player.session=1;
                player.day = atoi(sav.buffer[1]);
                player.val = atoi(sav.buffer[2]);
                player.reputation = atoi(sav.buffer[3]);
                player.stocks[0] = atoi(sav.buffer[4]);
                player.stocks[1] = atoi(sav.buffer[5]);
                player.stocks[2] = atoi(sav.buffer[6]);
                player.stocks[3] = atoi(sav.buffer[7]);
                player.stocks[4] = atoi(sav.buffer[8]);
                player.stocks[5] = atoi(sav.buffer[9]);
                check_session();

                break;
            case ERROR_FILE_NOT_FOUND:
                printf("Ups! Sepertinya berkas tidak ditemukan...");
                getch();
                check_session();
                break;
            case ERROR_INVALID_DATA:
                printf("Ups! Berkas rusak...");
                getch();
                check_session();
                break;
            default:
                printf("Ups! Sepertinya kesalahan lain...");
                getch();
                check_session();
                break;
            }
            break;
        default:
            break;
        }
    }
}

int ganti_hari() {
    player.day++;
    srand(time(NULL));
    refresh_trading();
    set_stats();
    change_currs();
}

void display() {

    char fname[32];
    system("cls");
    printf("Starter Release v1.4\nDesember'15\n2015 Agung Satrio\n2015 Fajar Nazmi\n\n");
    printf("Hari ke-%d\nUang Anda: %d val\nKurs dolar: %d val/US$1 (",player.day,player.val, economy.currs);

    if(economy.currs==economy.currs_before) {
        printf("tetap");
     } else if(economy.currs>economy.currs_before){
        color(LIGHTRED);
        printf("melemah");
        color(DEFAULT);
     } else if(economy.currs<economy.currs_before){
        color(LIGHTGREEN);
        printf("menguat");
        color(DEFAULT);
     }

    printf(")\n\nPilih tindakan:\n1. Beli\n2. Jual\n3. Baca Koran\n4. Ganti hari\n5. Simpan Permainan\n6. Keluar sesi\n\n0. Keluar dari program\n\nPilihan Anda: ");
    scanf("%d", &choice); fflush(stdin);

    switch(choice) {
    case 0:
        exit(0);
        break;
    case 1:
        trade(0);
        break;
    case 2:
        trade(1);
        break;
    case 3:
        koran();
        break;
    case 4:
        system("cls");
        printf("=======TIDUR SEJENAK========\nTunggu sejenak...");
        Sleep(2000);
        ganti_hari();
        display();
        break;
    case 5:
        system("cls");
        printf("==========SIMPAN UNTUK NANTI===================\nSimpan dengan nama (mis. gameXX.sav): ");
        gets(fname); fflush(stdin);
        readsave(1,fname);
        printf("Write finished! Kembali ke display...");
        getch(); display();
    break;
    case 6:
        player.session=0;
        check_session();
    default:
        display();
    }
}

void trade(int dagang) {
    int i;
    int pilih_barang = 0;
    int *jml_beli = 0;


    if(dagang==1) {
        trade_strings="dijual";
    } else {
        trade_strings="dibeli";
    }

    system("cls");
    printf("========PERDAGANGAN===========\n");
    if(dagang==1) {
        for(i=0;i<=5;i++) {
            printf("%d. %s \t(harga jual %d val (US$%d))\tAda %d buah\n",i+1,market[i].name, market[i].priceInVal,market[i].price,player.stocks[i]);
        }
    } else {
        for(i=0;i<=5;i++) {
            printf("%d. %s \t(harga %d val (US$%d))\tTersedia %d\n",i+1,market[i].name, market[i].priceInVal,market[i].price,market[i].stock);
        }
    }

    printf("\nPilih mana yang akan %s (atau masukkan 99 untuk kembali): ", trade_strings);
    scanf("%d",&pilih_barang); fflush(stdin);

    if(pilih_barang==99) {
        display(); return;
    }

    if(dagang==1&&player.stocks[pilih_barang]<=0){
    printf("Invalid!\n\nKembali ke display...");
        getch(); display();
    }

    printf("Mau %s berapa? ",trade_strings);
    scanf("%d", &jml_beli); fflush(stdin);

    transaksi(pilih_barang, jml_beli, dagang);

    printf("\nKembali ke display...");
    getch(); display();
}

void transaksi(int barang0, int jumlah, int ini_dagang) {
    int yakin;
    int barang = barang0-1;
    //system("cls");
    int biaya = market[barang].priceInVal*jumlah;
    printf("Biaya %d val. Masukkan 1 untuk melanjutkan, atau 0 untuk keluar... ", biaya);
    scanf("%d", &yakin); fflush(stdin);
    switch(yakin) {
    case 0:
        display();
        break;
    case 1:
        if(ini_dagang==1) {
            player.val+=biaya;
            player.stocks[barang]-=jumlah;
            market[barang].stock+=jumlah;
        } else {
            if(player.val<biaya) {
                printf("Tidak dapat menyelesaikan transaksi!\nTekan apa saja untuk melanjutkan...");
                getch(); display();
                return;
            }
            player.val-=biaya;
            player.stocks[barang]+=jumlah;
            market[barang].stock-=jumlah;
        }
        break;
    default:
        display();
        break;
    }
}

void koran() {
    system("cls");
    fflush(stdout);
    printf("\nBerita hari ini\n==========================\n");
    printf("KEUANGAN: \nKurs val terhadap US$ ",economy.currs, economy.currs_before);

    if(economy.rate<0) {
        economy.rateDisplay = -economy.rate;
    } else {
        economy.rateDisplay = economy.rate;
    }

     if(economy.currs==economy.currs_before) {
        printf("tetap");
     } else if(economy.currs>economy.currs_before){
        color(LIGHTRED);
        printf("melemah");
        color(DEFAULT);
     } else if(economy.currs<economy.currs_before){
        color(LIGHTGREEN);
        printf("menguat");
        color(DEFAULT);
     }

    printf(" %d poin.\n-> Sebelumnya %d val per US$1\n-> Menjadi %d val per US$1.\n\n", economy.rateDisplay, economy.currs_before, economy.currs);
    printf("BERITA: \n- %s\n\n- %s\n\n",news.news0, news.politik);
    printf("Kembali ke display...");
    getch(); display();
}

void color(WORD value) {
    SetConsoleTextAttribute(GetStdHandle(CONSOLE_OUTPUT_WINDOW), value);
}

void refresh_trading() {
    int i;
    for(i=0;i<=5;i++) {
        double price = random(10,64);
        double stocks = random(10,64);
        double harga = (economy.currs*price)+tax;
        market[i].name = items[i];
        market[i].price = price;
        market[i].priceInVal = harga;
        market[i].stock = stocks;
    }
}

void change_currs() {
    economy.rate = 0;
    if(economy.currs<=0) economy.currs=5000;
    rate_min = random(-1,-962);
    rate_max = random(1,979);
    economy.rate = random(rate_min,rate_max);
    economy.currs_before = economy.currs;
    economy.currs+=economy.rate;
}

int random(signed int min, signed int max) {
    float scaled = (float)rand()/RAND_MAX;
    return (max - min +1)*scaled+min;
}

int readsave(int isSave, char fname[32]) {

    char* temp[255];
    FILE *fsave;
    int i;
    switch(isSave) {
    case 0:
        fsave = fopen(fname,"rb");
        if(fsave != NULL) {
            fgets(sav.data,80,fsave);
        } else {
            return ERROR_FILE_NOT_FOUND;
        }
        fclose(fsave);

        if(!sav.data[0]=='S'&&!sav.data[1]=='A'&&!sav.data[1]=='V') {
            return ERROR_INVALID_DATA;
        }

        sav.temp = strtok(sav.data,"!");
        sav.buffer[0]=sav.temp; //SAV header
        sav.temp = strtok(NULL,",");
        sav.buffer[1]=sav.temp; //day
        sav.temp = strtok(NULL,",");
        sav.buffer[2]=sav.temp; //val
        sav.temp = strtok(NULL,",");
        sav.buffer[3]=sav.temp; //reputation
        sav.temp = strtok(NULL,",");
        sav.buffer[4]=sav.temp; //item0
        sav.temp = strtok(NULL,",");
        sav.buffer[5]=sav.temp; //item1
        sav.temp = strtok(NULL,",");
        sav.buffer[6]=sav.temp; //item2
        sav.temp = strtok(NULL,",");
        sav.buffer[7]=sav.temp; //item3
        sav.temp = strtok(NULL,",");
        sav.buffer[8]=sav.temp; //item4
        sav.temp = strtok(NULL,",");
        sav.buffer[9]=sav.temp; //item5
        return ERROR_SUCCESS;

        break;
    case 1:
        //crypt(1,sav.buff);*/

        strcpy(fbuffer,"");
        concat(fbuffer,"SAV!");
        itoa(player.day,temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.val,temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.reputation,temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.stocks[0],temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.stocks[1],temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.stocks[2],temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.stocks[3],temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.stocks[4],temp,10);
        concat(fbuffer,temp);
        concat(fbuffer,",");
        itoa(player.stocks[5],temp,10);
        concat(fbuffer,temp);

        printf("Before: %s\n",fbuffer);

        //crypt(fbuffer);

        printf("After: %s\n",fbuffer);

        fsave = fopen(fname,"wb+");
        fprintf(fsave,fbuffer);
        fclose(fsave);
        break;
    }
}

void concat(char s1[], const char s2[]) {
   int i, j;

   i = strlen(s1);

   for (j = 0; s2[j] != '\0'; i++, j++) {
      s1[i] = s2[j];
   }

   s1[i] = '\0';
}

void set_stats() {
    condition = random(-5,5);
    switch(condition) {
    case 5:
        news.news0="Pelonggaran pajak yang dilakukan oleh pemerintah memicu penurunan harga barang dan jasa.";
        news.politik = "Pembahasan revisi UU Barang Impor sedang berlangsung di Dewan.";
        tax -=32;
        break;
    case 4:
        news.news0="Sepinya pembeli membuat sejumlah produsen memilih menurunkan harga untuk memikat pembeli.";
        news.politik = "Pemerintah menerapkan disiplin ketat pada semua PNS.";
        tax -=28;
        break;
    case 3:
        news.news0="Beberapa produser menurunkan harga barang untuk memikat konsumen menengah ke bawah.";
        news.politik = "Partai XYZ memenangkan hasil hitung cepat.";
        tax-=24;
        break;
    case 2:
        news.news0="Penurunan harga komponen memicu penurunan harga barang.";
        news.politik = "Partai YZZ menetapkan AD/ART baru yang akan diterapkan pekan depan.";
        tax-=16;
        break;
    case 1:
        news.news0="''Mungkin, daya beli masyarakat kita sedang rendah, sehingga para produsen memilih menurunkan harga barang mereka,'' kata seorang ekonom saat diwawancarai pekan lalu di kantornya.";
        news.politik = "Partai GHJ mendorong pemerintah menaikkan daya beli masyarakat.";
        tax-=8;
        break;
    case 0:
        news.news0="Harga-harga tetap stabil meski keadaan ekonomi terus berubah.";
        news.politik = "Dewan menetapkan reses pada hari ini.";
        tax+=0;
        break;
    case -1:
        news.news0="Pemerintah menetapkan kenaikan pajak sebesar 12 poin untuk menyeimbangkan neraca perdagangan.";
        news.politik = "Skandal korupsi ketua kamar dagang membuat investor pikir-pikir kembali.";
        tax+=12;
        break;
    case -2:
        news.news0="Kenaikan permintaan membuat para produsen menaikkan harga barang.";
        news.politik = "Partai ABX memecat sembilan anggotanya terkait kerusuhan di Lapangan HHS beberapa waktu lalu.";
        tax+=16;
        break;
    case -3:
        news.news0="Pemerintah menetapkan kenaikan pajak sebesar 36 poin untuk menyeimbangkan pasokan barang impor yang akhir-akhir ini mengalami kenaikan.";
        news.politik = "Pemerintah dinilai kurang serius menindak anggota dewan yang sering menggunakan uang negara untuk kepentingan pribadi.";
        tax+=36;
        break;
    case -4:
        news.news0="Pemerintah dinilai kurang serius mengatasi pelemahan ekonomi.";
        news.politik = "Kedapatan tidur saat rapat paripurna, seorang anggota fraksi GSD dipecat.";
        tax+=48;
        break;
    case -5:
        news.news0="Pemerintah menetapkan kenaikan pajak impor sebesar 6 poin.";
        news.politik = "Pembahasan UU Barang Impor mangkrak di Dewan.";
        tax+=6;
        break;
    }
}

void checkval() {
    if(player.session==1&&player.val<=0) {
        system("cls");
        printf("Maaf!\nAnda bangkrut. Silakan tekan apa\nsaja untuk memulai kembali...");
        getch();
        player.session=0;
        check_session();
    }
}

