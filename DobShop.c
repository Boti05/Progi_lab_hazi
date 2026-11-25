#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

enum menu {BELEPES = 1, REGISZTRACIO, KILEPES, VENDEG, DOLGOZO, ADMIN};

int koszontes()
{
    /**
     * @brief Egy üdvözlő függvény, mely eldönti, hogy melyik menübe lépjünk tovább.
     * @return Egy számot ad vissza, melyet az enumban lévőekkel hasonlítok össze a main-ben
     */
    printf("\nUdvozlunk a BobDrum Shopban!\nKerlek valaszd ki, hogy mit szeretnel:\nBelepni [1]\nRegisztralni [2]\nKilepni [3]\n(csak a megadott ertekek kozul valassz midig): ");
    int szam;
    scanf("%d", &szam);
    return szam;
}

int staff_fajl_beolvas(char *p_felhnev, char *p_jelszo)
{
    printf("\nbeolvassuk a staff fajlt.\na felhasznalonev: %s\na jelszo: %s\n", p_felhnev, p_jelszo);
    return 0;
}

int vendeg_fajl_beolvas(char *p_felhnev, char *p_jelszo)
{
    printf("\nbeolvassuk a vendeg fajlt.\na felhasznalonev: %s\na jelszo: %s\n", p_felhnev, p_jelszo);
    return 0;
}

char *titkositas(char *p_jelszo)
{
    /**
     * @brief egy nagyon egyszerű titkosítás szimuláló függvény (ascii értékek növelése betűnként)
     * @param p_jelszo a titkosítani kívánt jelszó (sztring)
     * @return az eltitkosított jelszót
    */
    int jelszo_len = strlen(p_jelszo);
    for (int i = 0; i < jelszo_len; i++)
    {
        p_jelszo[i] = p_jelszo[i] + 1;
    }
    return p_jelszo;
}

int belepes(char *p_felhnev, char *p_jelszo)
{
    /**
     * @brief A belépéshez szükséges infók megszerzése, megfelelő státusszal visszatérni
     * @param p_felhnev A megadott felhasználónév
     * @param p_jelszo A megadott jelszó
     * @return Egy meghatározott opcióval, amit a user választott
     */
    //Bekérünk felhasználónevet és jelszót, majd a jelszót eltitkosítjuk

    printf("\nBelepes.\nIrd be a felhasznaloneved (legfeljebb 25 karakter): ");
    if (scanf("%s", p_felhnev) == 1 && strlen(p_felhnev) <= 25)
    {
        printf("A jelszo minimum 8, maximum 20 karakter hosszu.\nAdd meg a jelszavad ('~' karakter nem lehet benne): ");
        if (scanf(" %s", p_jelszo) == 1 && strlen(p_jelszo) >= 8 && strlen(p_jelszo) <= 20)
        {
            p_jelszo = titkositas(p_jelszo);

            int felhnev_len = strlen(p_felhnev);
            int statusz;

            //Eldöntjük, hogy dolgozó, admin vagy vásárló lép-e be és az alapján térünk vissza 
            if (p_felhnev[felhnev_len - 2] == '_')
            statusz = p_felhnev[felhnev_len - 1] == 'e' ? DOLGOZO : ADMIN;

            else statusz = VENDEG;

            switch (statusz)
            {
            case DOLGOZO: printf("\nDolgozo.\nSikeres belepes!\n"); return DOLGOZO + BELEPES;
            case ADMIN: printf("\nAdmin.\nSikeres belepes!\n"); return ADMIN + BELEPES;
            case VENDEG: printf("\nVendeg.\nSikeres belepes!\n"); return VENDEG + BELEPES;
            }
            return -1;
        }
        else if (strlen(p_jelszo) < 8)
        {
            printf("Tul rovid jelszo!\n");
            belepes(p_felhnev, p_jelszo);
        }
        else if (strlen(p_jelszo) > 20)
        {
            printf("Tul hosszu jelszo!\n");
            belepes(p_felhnev, p_jelszo);
        }
        else
        {
            printf("Nem adtal meg jelszot!\n");
            belepes(p_felhnev, p_jelszo);
        }
    }
    printf("Tul hosszu felhasznalonev.\n");
    belepes(p_felhnev, p_jelszo);
}

int regiszracio()
{
    printf("regisztracio");
    return 0;
}

int program_vege()
{
    /**
     * @brief Elköszön majd a kilép
     * @return A kilépés opció értékét adja vissza
     */
    printf("\nKoszonjuk, hogy nalunk vasarolt!");
    return KILEPES;
}

int main()
{
    int opcio;
    char felhnev[30];
    char jelszo[25];

    opcio = koszontes();

    //A menüben melyik opciót választotta, az alapján megyünk tovább
    switch (opcio)
    {
    case BELEPES: opcio = belepes(felhnev, jelszo); break;
    case REGISZTRACIO: opcio = regiszracio(); break;
    case KILEPES: program_vege(); return 0;
    }
    
    //Ha már tudjuk mit akar csinálni és ki, akkor annak megfelelő fájlt olvasunk be
    switch (opcio)
    {
    case BELEPES + ADMIN: staff_fajl_beolvas(felhnev, jelszo); break;
    case BELEPES + DOLGOZO: staff_fajl_beolvas(felhnev, jelszo); break;
    case BELEPES + VENDEG: vendeg_fajl_beolvas(felhnev, jelszo); break;
    }
    
    printf("%s", jelszo);

    return 0;
}