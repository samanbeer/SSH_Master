#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <direct.h>

#define MAX_SERVERS 20
#define MAX_STR 128
#define ENCRYPT_KEY 0xAF

// Rozšířená paleta barev
#define COL_CYAN 11
#define COL_GREEN 10
#define COL_GRAY 8
#define COL_RED 12
#define COL_YELLOW 14
#define COL_WHITE 15
#define COL_BLUE 9
#define COL_GOLD 6
#define COL_DEFAULT 7

typedef struct {
    char jmeno[MAX_STR];
    char adresa[MAX_STR];
} Server;

Server servery[MAX_SERVERS];
int pocetServeru = 0;
char configPath[MAX_STR * 2];

// --- SYSTÉMOVÉ A BEZPEČNOSTNÍ FUNKCE ---

void setCol(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void cipher(char *data) {
    for(int i = 0; i < (int)strlen(data); i++) {
        data[i] ^= ENCRYPT_KEY;
    }
}

void zajistitSSHKey() {
    char path[MAX_STR * 2];
    sprintf(path, "%s\\.ssh\\id_rsa.pub", getenv("USERPROFILE"));
    FILE *f = fopen(path, "r");
    if (!f) {
        setCol(COL_YELLOW);
        printf("\n [!] SSH klic nenalezen. Generuji nove sifrovani...\n");
        system("ssh-keygen -t rsa -b 2048 -f %USERPROFILE%/.ssh/id_rsa -N \"\" > nul 2>&1");
        setCol(COL_GREEN);
        printf(" [OK] Klice byly uspesne vytvoreny.\n");
        Sleep(1000);
    } else fclose(f);
}

void initEnvironment() {
    char folderPath[MAX_STR * 2];
    sprintf(folderPath, "%s\\.ssh_master", getenv("USERPROFILE"));
    sprintf(configPath, "%s\\config.dat", folderPath);
    _mkdir(folderPath);
    zajistitSSHKey();
}

void saveToFile() {
    FILE *f = fopen(configPath, "wb");
    if (!f) return;
    fwrite(&pocetServeru, sizeof(int), 1, f);
    for (int i = 0; i < pocetServeru; i++) {
        Server temp = servery[i];
        cipher(temp.jmeno); cipher(temp.adresa);
        fwrite(&temp, sizeof(Server), 1, f);
    }
    fclose(f);
}

void loadFromFile() {
    FILE *f = fopen(configPath, "rb");
    if (!f) return;
    if (fread(&pocetServeru, sizeof(int), 1, f) == 1) {
        for (int i = 0; i < pocetServeru; i++) {
            fread(&servery[i], sizeof(Server), 1, f);
            cipher(servery[i].jmeno); cipher(servery[i].adresa);
        }
    }
    fclose(f);
}

// --- DESIGN A UI ---

void drawUIFrame(int x, int y, int w, int h, int color, char* title) {
    setCol(color);
    gotoxy(x, y); printf("%c", 201);
    for(int i=0; i<w; i++) printf("%c", 205);
    printf("%c", 187);
    for(int i=1; i<h; i++) {
        gotoxy(x, y+i); printf("%c", 186);
        gotoxy(x+w+1, y+i); printf("%c", 186);
    }
    gotoxy(x, y+h); printf("%c", 200);
    for(int i=0; i<w; i++) printf("%c", 205);
    printf("%c", 188);
    if(title) {
        gotoxy(x + (w/2) - (strlen(title)/2), y);
        setCol(COL_WHITE); printf(" %s ", title);
    }
}

void tiskniNadpis() {
    system("cls");
    setCol(COL_BLUE);
    gotoxy(5, 1); printf(" ____ ____  _   _   __  __           _            ");
    gotoxy(5, 2); printf("/ ___/ ___|| | | | |  \\/  | __ _ ___| |_ ___ _ __ ");
    gotoxy(5, 3); printf("\\___ \\___ \\| |_| | | |\\/| |/ _` / __| __/ _ \\ '__|");
    gotoxy(5, 4); printf(" ___) |__) |  _  | | |  | | (_| \\__ \\ |_  __/ |   ");
    gotoxy(5, 5); printf("|____/____/|_| |_| |_|  |_|\\__,_|___/\\__\\___|_|   ");
    
    setCol(COL_GRAY);
    gotoxy(2, 7); printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176);
}

void pridatPripojeni() {
    if (pocetServeru >= MAX_SERVERS) return;
    tiskniNadpis();
    drawUIFrame(5, 9, 50, 6, COL_GREEN, "REGISTRACE");
    
    setCol(COL_WHITE);
    gotoxy(8, 11); printf("Nazev spojeni : "); setCol(COL_CYAN);
    rewind(stdin);
    fgets(servery[pocetServeru].jmeno, MAX_STR, stdin);
    servery[pocetServeru].jmeno[strcspn(servery[pocetServeru].jmeno, "\n")] = 0;
    
    setCol(COL_WHITE);
    gotoxy(8, 12); printf("SSH Adresa    : "); setCol(COL_CYAN);
    fgets(servery[pocetServeru].adresa, MAX_STR, stdin);
    servery[pocetServeru].adresa[strcspn(servery[pocetServeru].adresa, "\n")] = 0;

    gotoxy(8, 14); setCol(COL_YELLOW); printf("Probíhá párování klíčů... ");
    char cmd[512];
    sprintf(cmd, "type %%USERPROFILE%%\\.ssh\\id_rsa.pub | ssh %s \"mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys\"", servery[pocetServeru].adresa);
    
    if (system(cmd) == 0) {
        pocetServeru++;
        saveToFile();
        gotoxy(8, 14); setCol(COL_GREEN); printf("[ OK ] Server byl uspesne autorizovan.   ");
    } else {
        gotoxy(8, 14); setCol(COL_RED); printf("[ ERROR ] Autentizace se nezdarila.      ");
    }
    _getch();
}

int main() {
    initEnvironment();
    loadFromFile();
    int vybrany = 0;

    while (1) {
        tiskniNadpis();
        int celkem = 2 + pocetServeru;

        // Vykreslení seznamu
        for (int i = 0; i < celkem; i++) {
            gotoxy(6, 10 + i);
            if (vybrany == i) {
                setCol(COL_CYAN); printf("%c ", 175);
                setCol(96); // Tyrkysové pozadí pro aktivní prvek
                if (i == 0) printf("  [+] PRIDAT NOVE PRIPOJENI     ");
                else if (i == celkem - 1) printf("  [X] UKONCIT PROGRAM           ");
                else printf("  SSH: %-24s ", servery[i-1].jmeno);
                setCol(COL_DEFAULT);
            } else {
                setCol(COL_GRAY); printf("  ");
                if (i == 0) printf("  Pridat nove pripojeni");
                else if (i == celkem - 1) printf("  Ukoncit program");
                else { 
                    setCol(COL_WHITE); printf("  Node: ");
                    setCol(COL_CYAN); printf("%-24s", servery[i-1].jmeno);
                }
            }
        }

        // Footer
        gotoxy(2, 23); setCol(COL_GRAY);
        printf("%c%c%c ", 219, 219, 219);
        setCol(COL_WHITE); printf("ENTER"); setCol(COL_GRAY); printf(": Connect  ");
        setCol(COL_WHITE); printf("DEL"); setCol(COL_GRAY); printf(": Delete  ");
        setCol(COL_WHITE); printf("ESC"); setCol(COL_GRAY); printf(": Exit");

        int kl = _getch();
        if (kl == 224) {
            kl = _getch();
            if (kl == 72) vybrany = (vybrany - 1 + celkem) % celkem;
            else if (kl == 80) vybrany = (vybrany + 1) % celkem;
            else if (kl == 83) { // DELETE
                if (vybrany > 0 && vybrany < celkem - 1) {
                    setCol(COL_RED);
                    gotoxy(6, 21); printf("Opravdu smazat %s? (y/n)", servery[vybrany-1].jmeno);
                    if (tolower(_getch()) == 'y') {
                        for (int i = vybrany - 1; i < pocetServeru - 1; i++) servery[i] = servery[i+1];
                        pocetServeru--;
                        saveToFile();
                    }
                }
            }
        } else if (kl == 13) {
            if (vybrany == 0) pridatPripojeni();
            else if (vybrany == celkem - 1) break;
            else {
                system("cls");
                setCol(COL_GREEN);
                printf("\n [ CONNECTING ] Remote host: %s\n", servery[vybrany-1].jmeno);
                setCol(COL_GRAY); printf(" --------------------------------------------------\n\n");
                setCol(COL_DEFAULT);
                
                char ssh_cmd[256];
                sprintf(ssh_cmd, "ssh %s", servery[vybrany-1].adresa);
                system(ssh_cmd);
                
                setCol(COL_CYAN);
                printf("\n\n [ DISCONNECTED ] Relace ukoncena. Nashledanou.");
                Sleep(900);
                exit(0);
            }
        } else if (kl == 27) break;
    }
    return 0;
}