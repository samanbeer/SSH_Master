#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <direct.h>
#include <ctype.h>

#define MAX_SERVERS 20
#define MAX_STR 128
#define MAX_PATH_LEN 512
#define ENCRYPT_KEY 0xAF

// --- COLOR PALETTE ---
#define COL_FRAME 9       
#define COL_CYAN 11       
#define COL_GREEN 10      
#define COL_GRAY 8        
#define COL_RED 12        
#define COL_YELLOW 14     
#define COL_WHITE 15      
#define COL_BG_BAR 240    
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

void showCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void fixEncoding() {
    SetConsoleOutputCP(852);
    SetConsoleCP(852);
}

void cipher(char *data) {
    if (!data) return;
    for(int i = 0; i < (int)strlen(data); i++) data[i] ^= ENCRYPT_KEY;
}

void inputWithPlaceholder(char* buffer, int maxLen, const char* placeholder, int x, int y) {
    int pos = 0;
    int key;
    
    gotoxy(x, y);
    setCol(COL_GRAY);
    printf("%s", placeholder);
    gotoxy(x, y); 
    
    showCursor();
    while (1) {
        key = _getch();
        if (key == 13) { // ENTER
            buffer[pos] = '\0';
            break;
        } else if (key == 8) { // BACKSPACE
            if (pos > 0) {
                pos--;
                printf("\b \b");
                if (pos == 0) {
                    gotoxy(x, y);
                    setCol(COL_GRAY);
                    printf("%s", placeholder);
                    gotoxy(x, y);
                }
            }
        } else if (key >= 32 && key <= 126 && pos < maxLen - 1) {
            if (pos == 0) {
                gotoxy(x, y);
                for (int i = 0; i < (int)strlen(placeholder); i++) printf(" ");
                gotoxy(x, y);
                setCol(COL_CYAN);
            }
            buffer[pos++] = (char)key;
            printf("%c", key);
        }
    }
    hideCursor();
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
        gotoxy(x, y+i); printf("%c", 186); // OPRAVENO: Odstraněna mezera
        gotoxy(x+w+1, y+i); printf("%c", 186); // OPRAVENO: Odstraněna mezera
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
    printf("\n");
    printf("    ____ ____  _   _   __  __    _    ____ _____ _____ ____  \n");
    printf("   / ___/ ___|| | | | |  \\/  |  / \\  / ___|_   _| ____|  _ \\ \n");
    printf("   \\___ \\___ \\| |_| | | |\\/| | / _ \\ \\___ \\ | | |  _| | |_) |\n");
    printf("    ___) |__) |  _  | | |  | |/ ___ \\ ___) || | | |___|  _ < \n");
    printf("   |____/____/|_| |_| |_|  |_/_/   \\_\\____/ |_| |_____|_| \\_\\\n");
    setCol(COL_GRAY);
    printf("    -----------------------------------------------------------\n");
}

void statusLine() {
    gotoxy(0, 24);
    setCol(COL_BG_BAR);
    for(int i=0; i<80; i++) printf(" ");
    gotoxy(2, 24);
    printf(" ENTER: Connect | DEL: Delete | ESC: Exit | Nodes: %d", pocetServeru);
    setCol(COL_DEFAULT);
}

void pridatPripojeni() {
    system("cls");
    fixEncoding();
    tiskniNadpis();
    drawFrame(5, 8, 60, 10, COL_GREEN, " NEW CONNECTION SETUP "); // Mírně zvětšeno pro stabilitu
    
    setCol(COL_WHITE);
    gotoxy(8, 10); printf("Alias Nazev   : ");
    inputWithPlaceholder(servery[pocetServeru].jmeno, MAX_STR, "Napr. Hlavni Server", 24, 10);
    
    setCol(COL_WHITE);
    gotoxy(8, 12); printf("SSH Adresa    : "); setCol(COL_GRAY); printf("ssh ");
    inputWithPlaceholder(servery[pocetServeru].adresa, MAX_STR, "root@192.168.0.101", 28, 12);

    gotoxy(8, 14); setCol(COL_YELLOW); printf("Attempting RSA Key Handshake...");
    char cmd[512];
    // OPRAVENO: Přidáno přesměrování do nul, aby chyby nerozbíjely UI
    _snprintf(cmd, sizeof(cmd), "type %%USERPROFILE%%\\.ssh\\id_rsa.pub | ssh %s \"mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys\" >nul 2>&1", servery[pocetServeru].adresa);
    
    showCursor();
    int result = system(cmd);
    fixEncoding();

    if (result == 0) {
        pocetServeru++;
        saveToFile();
        gotoxy(8, 16); setCol(COL_GREEN); printf("[ SUCCESS ] Node authorized!");
    } else {
        gotoxy(8, 16); setCol(COL_RED); printf("[ FAILED ] Handshake rejected (Check password/IP).");
    }
    hideCursor();
    setCol(COL_DEFAULT);
    _getch();
    system("cls");
}

int main() {
    fixEncoding();
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
        fixEncoding();
        gotoxy(0, 0);
        tiskniNadpis();
        
        int celkem = 2 + pocetServeru;
        drawFrame(4, 8, 65, celkem + 2, COL_FRAME, " INFRASTRUCTURE NODES "); // Zvětšeno na 65

        for (int i = 0; i < celkem; i++) {
            gotoxy(7, 10 + i);
            if (vybrany == i) {
                setCol(COL_CYAN); printf("%c ", 175);
                setCol(COL_WHITE); 
                if (i == 0) printf("[ + ] REGISTER NEW SERVER             ");
                else if (i == celkem - 1) printf("[ X ] TERMINATE APPLICATION           ");
                else printf("Node: %-35s", servery[i-1].jmeno);
            } else {
                printf("  "); 
                if (i == 0) { setCol(COL_GREEN); printf("  Add new server                   "); }
                else if (i == celkem - 1) { setCol(COL_RED); printf("  Exit program                     "); }
                else { 
                    setCol(COL_GRAY); printf("  Node: ");
                    setCol(COL_DEFAULT); printf("%-35s", servery[i-1].jmeno);
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
                    gotoxy(7, 10 + celkem + 1); printf("Destroy node %s? (y/n): ", servery[vybrany-1].jmeno);
                    showCursor();
                    if (tolower(_getch()) == 'y') {
                        for (int i = vybrany - 1; i < pocetServeru - 1; i++) servery[i] = servery[i+1];
                        pocetServeru--;
                        saveToFile();
                        system("cls");
                    } else {
                        gotoxy(7, 10 + celkem + 1); printf("                                     ");
                    }
                    hideCursor();
                }
            }
        } else if (kl == 13) {
            if (vybrany == 0) pridatPripojeni();
            else if (vybrany == celkem - 1) break;
            else {
                system("cls");
                setCol(COL_GREEN); 
                printf("\n    [ INITIATING ] Connecting to: %s\n", servery[vybrany-1].jmeno);
                printf("    -----------------------------------------------\n\n");
                setCol(COL_DEFAULT);
                
                char ssh_cmd[256];
                _snprintf(ssh_cmd, sizeof(ssh_cmd), "ssh %s", servery[vybrany-1].adresa);
                
                showCursor();
                system(ssh_cmd);
                fixEncoding();
                exit(0);
            }
        } else if (kl == 27) break;
    }
    return 0;
}