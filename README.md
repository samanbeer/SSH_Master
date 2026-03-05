🚀 SSH Master v2.0

SSH Master Pro je vysoce efektivní, bezpečný a lehký terminálový správce (TUI) pro SSH připojení, napsaný v čistém jazyce C. Byl navržen pro systémové administrátory a vývojáře, kteří hledají bleskovou alternativu k těžkopádným GUI aplikacím.
✨ Klíčové Funkce (Functionality)
1. Interaktivní TUI (Text User Interface)

    Zero-Flicker Rendering: Aplikace využívá přímé adresování kurzoru přes windows.h, čímž eliminuje blikání typické pro standardní konzolové programy.

    Intuitivní Navigace: Plná podpora šipek na klávesnici pro pohyb v menu a klávesových zkratek (ENTER pro spojení, DEL pro smazání, ESC pro ukončení).

    Dynamické Menu: Seznam serverů se automaticky překresluje a centruje podle aktuálního počtu uložených uzlů.
   <img width="731" height="513" alt="image" src="https://github.com/user-attachments/assets/fc3adafc-1d6d-447d-9916-469d062bd6ab" />
   <img width="534" height="198" alt="image" src="https://github.com/user-attachments/assets/c02cba06-e801-4111-b287-6a569f8b5692" />



3. Inteligentní SSH Automatizace

    Passwordless Experience: Program automaticky detekuje přítomnost SSH klíčů. Pokud chybí, vygeneruje nový 2048-bitový RSA pár.

    One-Click Key Sync: Při přidání nového serveru aplikace jednorázově distribuuje váš veřejný klíč (id_rsa.pub) na vzdálený uzel do souboru authorized_keys.

    Native Integration: Využívá vestavěný OpenSSH klient ve Windows, nevyžaduje instalaci PuTTY ani jiných nástrojů třetích stran.

4. Workflow & Automatizace

    Clean Session Management: Po odhlášení ze vzdáleného serveru aplikace automaticky ukončí svůj proces, čímž šetří systémové prostředky.

    Auto-Environment Setup: Při prvním spuštění program vytvoří nezbytnou adresářovou strukturu v profilu uživatele.

🔒 Bezpečnostní Architektura (Security)

Bezpečnost byla při vývoji prioritou. Program je navržen tak, aby minimalizoval riziko úniku citlivých dat.
🛡️ Správa Hesel

    Princip nulového ukládání: SSH Master Pro nikdy neukládá vaše hesla k serverům. Heslo je vyžadováno pouze jednou při párování klíčů a je zpracováno přímo systémovým procesem SSH, nikoliv naší aplikací.

    SSH Key-Based Auth: Veškerá následná komunikace probíhá přes asymetrické šifrování (RSA), což je průmyslový standard pro bezpečnou správu serverů.

🔐 Šifrování Dat (XOR Obfuscation)

    Konfigurační soubor config.dat je chráněn algoritmem XOR s fixním klíčem (0xAF).

    To zabraňuje náhodnému přečtení IP adres a názvů serverů v případě, že by někdo získal přístup k vašemu souboru. Data jsou v souboru uložena v binární, nečitelné podobě.

📂 Izolace Dat

    Veškerá data jsou uložena v cestě %USERPROFILE%\.ssh_master\.

    Tato složka je oddělena od ostatních aplikací a využívá standardní oprávnění uživatelského profilu Windows.

🛠️ Instalace a Sestavení (Build)

Projekt obsahuje Makefile pro snadnou kompilaci pomocí GCC (MinGW).
Požadavky

    Kompilátor GCC (součást MinGW-w64).

    Nástroj GNU Make.

Postup sestavení

    Klonujte repozitář:
    Bash

git clone https://github.com/TVOJE-JMENO/SSH-Master-Pro.git
cd SSH-Master-Pro

Zkompilujte aplikaci (včetně ikony):

    make

Spusťte program:
Bash

    ./SSH_Master.exe

📁 Struktura Projektu

    ├── main.c           # Jádro aplikace (TUI logika, šifrování, SSH operace)
    ├── resource.rc      # Definice ikon a systémových prostředků
    ├── icon.ico         # Ikona aplikace
    ├── Makefile         # Automatizovaný build skript
    ├── README.md        # Tato dokumentace
    └── .gitignore       # Definice souborů ignorovaných verzovacím systémem

📝 Licence a Příspěvky

Tento projekt je open-source. Příspěvky (Pull Requesty) jsou vítány! Pokud najdete chybu nebo máte nápad na vylepšení, neváhejte otevřít Issue.
