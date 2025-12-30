#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <direct.h>

#define MAX_SERVERS 20
#define MAX_STR 128
#define MAX_PATH_LEN 512
#define ENCRYPT_KEY 0xAF

// --- ROZŠÍŘENÁ BAREVNÁ PALETA ---
#define COL_FRAME 9       // Tmavě modrá pro rámečky
#define COL_CYAN 11       // Hlavní barva (text)
#define COL_GREEN 10      // Úspěch / Přidat
#define COL_GRAY 8        // Méně důležité info
#define COL_RED 12        // Nebezpečí / Exit
#define COL_YELLOW 14     // Výstraha / Klíče
#define COL_WHITE 15      // Jasný text
#define COL_BG_BAR 240    // Inverzní barva pro status bar
#define COL_DEFAULT 7

typedef struct {
    char jmeno[MAX_STR];
    char adresa[MAX_STR];
} Server;

Server servery[MAX_SERVERS];
int pocetServeru = 0;
char configPath[MAX_PATH_LEN];

// --- POMOCNÉ FUNKCE ---

void setCol(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void cipher(char *data) {
    if (!data) return;
    for(int i = 0; i < (int)strlen(data); i++) data[i] ^= ENCRYPT_KEY;
}

// --- LOGIKA SOUBORŮ ---

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
        if (pocetServeru > MAX_SERVERS) pocetServeru = MAX_SERVERS;
        for (int i = 0; i < pocetServeru; i++) {
            fread(&servery[i], sizeof(Server), 1, f);
            cipher(servery[i].jmeno); cipher(servery[i].adresa);
        }
    }
    fclose(f);
}

// --- UI KOMPONENTY ---

void drawFrame(int x, int y, int w, int h, int color, char* title) {
    setCol(color);
    gotoxy(x, y); printf("%c", 201);
    for(int i=0; i<w; i++) printf("%c", 205);
    printf("%c", 187);

    for(int i=1; i<h; i++) {
        gotoxy(x, y+i); printf("%c ", 186);
        gotoxy(x+w+1, y+i); printf(" %c", 186);
    }

    gotoxy(x, y+h); printf("%c", 200);
    for(int i=0; i<w; i++) printf("%c", 205);
    printf("%c", 188);

    if(title) {
        int len = (int)strlen(title);
        gotoxy(x + (w-len)/2, y);
        setCol(color); printf("%c", 185);
        setCol(COL_WHITE); printf(" %s ", title);
        setCol(color); printf("%c", 204);
    }
}

void tiskniNadpis() {
    gotoxy(0, 0);
    setCol(COL_CYAN);
    printf("\n    %c%c %c%c%c %c%c%c %c%c %c%c %c%c%c %c%c%c %c%c%c%c%c %c%c%c%c%c \n", 219,219, 219,219,219, 219,219,219, 219,219, 219,219, 219,219,219, 219,219,219, 219,219,219,219,219, 219,219,219,219,219);
    printf("    %c%c %c     %c   %c %c %c %c %c %c %c   %c     %c %c   \n", 219,219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219);
    printf("    %c%c %c%c%c %c%c%c %c%c %c%c %c%c%c %c%c%c %c%c    %c%c%c  \n", 219,219, 219,219,219, 219,219,219, 219,219, 219,219, 219,219,219, 219,219,219, 219,219, 219,219,219);
    printf("    -----------------------------------------------\n");
}

void statusLine() {
    gotoxy(0, 24);
    setCol(COL_BG_BAR);
    for(int i=0; i<80; i++) printf(" ");
    gotoxy(2, 24);
    printf(" ENTER: Connect | DEL: Delete | ESC: Exit | Current Nodes: %d", pocetServeru);
    setCol(COL_DEFAULT);
}

void pridatPripojeni() {
    system("cls"); // Zde cls nevadí, jsme mimo hlavní smyčku
    tiskniNadpis();
    drawFrame(5, 7, 50, 9, COL_GREEN, " NEW CONNECTION SETUP ");
    
    setCol(COL_WHITE);
    gotoxy(8, 9); printf("Alias Name   : "); setCol(COL_CYAN);
    rewind(stdin);
    fgets(servery[pocetServeru].jmeno, MAX_STR, stdin);
    servery[pocetServeru].jmeno[strcspn(servery[pocetServeru].jmeno, "\n")] = 0;
    
    setCol(COL_WHITE);
    gotoxy(8, 11); printf("SSH Protocol : "); setCol(COL_GRAY); printf("ssh "); setCol(COL_CYAN);
    fgets(servery[pocetServeru].adresa, MAX_STR, stdin);
    servery[pocetServeru].adresa[strcspn(servery[pocetServeru].adresa, "\n")] = 0;

    gotoxy(8, 13); setCol(COL_YELLOW); printf("Attempting RSA Key Handshake...");
    char cmd[512];
    _snprintf(cmd, sizeof(cmd), "type %%USERPROFILE%%\\.ssh\\id_rsa.pub | ssh %s \"mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys\"", servery[pocetServeru].adresa);
    
    if (system(cmd) == 0) {
        pocetServeru++;
        saveToFile();
        gotoxy(8, 14); setCol(COL_GREEN); printf("[ SUCCESS ] Node %s is authorized!  ", servery[pocetServeru-1].jmeno);
    } else {
        gotoxy(8, 14); setCol(COL_RED); printf("[ FAILED ] Handshake rejected.         ");
    }
    setCol(COL_DEFAULT);
    _getch();
    system("cls"); // Vyčistit po návratu do menu
}

int main() {
    char folderPath[MAX_PATH_LEN];
    char *userProfile = getenv("USERPROFILE");
    
    if (userProfile) {
        _snprintf(folderPath, sizeof(folderPath), "%s\\.ssh_master", userProfile);
        _snprintf(configPath, sizeof(configPath), "%s\\config.dat", folderPath);
        _mkdir(folderPath);
    } else {
        strcpy(configPath, "config.dat");
    }
    
    loadFromFile();
    hideCursor();
    int vybrany = 0;
    system("cls");

    while (1) {
        // Místo cls skočíme na začátek - klíč k plynulosti
        gotoxy(0, 0);
        tiskniNadpis();
        
        int celkem = 2 + pocetServeru;
        drawFrame(4, 7, 52, celkem + 2, COL_FRAME, " INFRASTRUCTURE NODES ");

        for (int i = 0; i < celkem; i++) {
            gotoxy(7, 9 + i);
            if (vybrany == i) {
                setCol(COL_CYAN); printf("%c ", 175); // Šipka
                setCol(COL_WHITE); 
                if (i == 0) printf("[ + ] REGISTER NEW SERVER      ");
                else if (i == celkem - 1) printf("[ X ] TERMINATE APPLICATION    ");
                else printf("SSH: %-25s", servery[i-1].jmeno);
            } else {
                printf("  "); // Smazat šipku
                if (i == 0) { setCol(COL_GREEN); printf("  Add new server            "); }
                else if (i == celkem - 1) { setCol(COL_RED); printf("  Exit program              "); }
                else { 
                    setCol(COL_GRAY); printf("  Node: ");
                    setCol(COL_DEFAULT); printf("%-25s", servery[i-1].jmeno);
                }
            }
        }

        statusLine();

        int kl = _getch();
        if (kl == 224) {
            kl = _getch();
            if (kl == 72) vybrany = (vybrany - 1 + celkem) % celkem;
            else if (kl == 80) vybrany = (vybrany + 1) % celkem;
            else if (kl == 83) { // DELETE
                if (vybrany > 0 && vybrany < celkem - 1) {
                    setCol(COL_RED);
                    gotoxy(7, 9 + celkem + 1); printf("Destroy node %s? (y/n): ", servery[vybrany-1].jmeno);
                    if (tolower(_getch()) == 'y') {
                        for (int i = vybrany - 1; i < pocetServeru - 1; i++) servery[i] = servery[i+1];
                        pocetServeru--;
                        saveToFile();
                        system("cls"); // Po smazání vyčistíme zbytky rámečku
                    } else {
                        // Smazat potvrzovací text
                        gotoxy(7, 9 + celkem + 1); printf("                          ");
                    }
                }
            }
        } else if (kl == 13) {
            if (vybrany == 0) pridatPripojeni();
            else if (vybrany == celkem - 1) break;
            else {
                system("cls");
                setCol(COL_GREEN); 
                printf("\n    ===============================================\n");
                printf("    [ INITIATING ] Connecting to: %s\n", servery[vybrany-1].jmeno);
                printf("    ===============================================\n\n");
                setCol(COL_DEFAULT);
                
                char ssh_cmd[256];
                _snprintf(ssh_cmd, sizeof(ssh_cmd), "ssh %s", servery[vybrany-1].adresa);
                system(ssh_cmd);
                exit(0);
            }
        } else if (kl == 27) break;
    }
    return 0;
}