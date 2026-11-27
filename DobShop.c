#include <stdio.h>
#include <string.h>
#include <cJSON.h>
#include <stdlib.h>

enum menu {JOGOSULATLAN = -2, ERROR, BELEPES = 1, REGISZTRACIO, KILEPES, VENDEG, DOLGOZO, ADMIN};

typedef struct
{
    char username[31];
    char name[64];
    char email[64];
    char phone[32];
    char pass_hash[31];
    char role[16];
    char second_factor_hash[64];
    char rfid_uid[32];
} staff_data;

typedef struct staff_list
{
    staff_data data;
    struct staff_list *next;
    struct staff_list *prev;
} staff_list, *staff_list_ptr;


int koszontes()
{
    /**
     * @brief Egy üdvözlő függvény, mely eldönti, hogy melyik menübe lépjünk tovább.
     * @return Egy számot ad vissza, melyet az enumban lévőekkel hasonlítok össze a main-ben
     */
    printf("\nUdvozlunk a BobDrum Shopban!\nKerlek valaszd ki, hogy mit szeretnel:\nBelepni [1]\nRegisztralni [2]\nKilepni [3]\n(csak a megadott ertekek kozul valassz midig): ");
    int szam = ERROR;
    scanf("%d", &szam);
    return szam;
}

void lista_felaszabadit(staff_list_ptr fej)
{
    /**
     * @brief A már nemhasznált listát felszabadítja, ha eleve nem NULL
     * @param fej A felszabadítani kívánt lista feje (kezdő elemének pointere)
     */
    staff_list_ptr aktualis = fej;
    while (aktualis != NULL)
    {
        //Végigmegyünk a lista elemein, amíg NULL nem lesz az elem, és felszabadítjuk az elemeket egyesével
        staff_list_ptr kovetkezo = aktualis->next;
        free(aktualis);
        aktualis = kovetkezo;
    }    
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

int belephet_e(char *p_felhnev, char *p_jelszo, staff_data *p_belepett_dolgozo)
{
    /**
     * @brief Ellenőrizzük, hogy jogosult-e a belépésre a felhasználó
     * @param p_felhnev A beírt felhasználónév
     * @param p_jelszo A beírt jelszó (már titkosítva)
     * @param p_belepett_dolgozo A belépni kívánt dolgozó adatait paraméterlistán adjuk vissza
     * @return 0 ha nem léphet be és 1 ha beléphet
     */
    int belephet = 0;
    staff_data belepett_dolgozo = *p_belepett_dolgozo;
    if (strcmp(p_felhnev, belepett_dolgozo.username) == 0)
    {
        if (strcmp(p_jelszo, belepett_dolgozo.pass_hash) == 0)
        {
            belephet = 1;
        }
    }
    *p_belepett_dolgozo = belepett_dolgozo;
    return belephet;
}

char *szekvenc_fajl_beolv(char *utvoanl)
{
    /**
     * @brief Darabokban beolvassuk a fájlt és egy bufferben visszaadjuk
     * @param utvonal ez a fájlnak a neve, amit be kell olvasni
     * @return visszaadja a buffer kezdő címét
     */

    FILE *fut = fopen(utvoanl, "rb");
    if (!fut)
    {
        perror("Hiba a fajl megnyitasakor");
        return NULL;
    }
    
    //A buffer karaktertömbben tároljuk majd el a fájl tartalmát
    //meret-nyi nagysággal ellenőrizzük, hogy fér-e még adat
    //A hasznalt azt mutatja, hogy eddig mennyit olvastunk be
    size_t meret = 1024;
    size_t hasznalt = 0;
    char *buffer = malloc(meret);
    if (!buffer)
    {
        fclose(fut);
        return NULL;
    }
    //Kisebb méretdarabokban olvassuk be a fájlt és ezt ismételjük a fájl végéig, vagy error-ig
    while (1)
    {
        if (hasznalt + 512 + 1 > meret)
        {
            //a ritkább méretezésért duplázunk
            size_t uj_meret = meret * 2;
            char *novelt = realloc(buffer, uj_meret);

            //sikertelen realloc esetén felszabadítjuk a buffer-t
            if (!novelt)
            {
                free(buffer);
                fclose(fut);
                return NULL;
            }
            buffer = novelt;
            meret = uj_meret;
        }
        //Az eddig beolvasott adatok után folytatjuk a beolvasást
        size_t n = fread(buffer + hasznalt, 1, 512, fut);
        hasznalt += n;

        //Kisebb beolvasás esetén [vagy hiba], kilépünk a ciklusból [vagy megszakítjuk]
        if (n < 512)
        {
            if (ferror(fut))
            {
                free(buffer);
                fclose(fut);
                return NULL;
            }
            break;
        }
    }
    buffer[hasznalt] = '\0';
    fclose(fut);
    return buffer;
}

void mezo_masol(cJSON *elem, char *kulcs, char *cella, size_t meret)
{
    /**
     * @brief Ellenőrizzük, hogy létezik-e az adott adat mezője, és hogy sztring-e. Ha minden jó, belemásoljuk, majd a végére a \0 karaktert, ha nincs, akkor visszatérünk, és ha 0 hosszú, akkor üres sztringet tárolunk el
     * @param elem az adott dolgozó adatainak json struktúrája
     * @param kulcs az adott kulcs, aminek az értékét keressük
     * @param cella ide mentjük a saját struktúrába a kiszedett adatot
     * @param meret az adat mérete
     */
    if (meret == 0) return;
    cella[0] = '\0';
    cJSON *mezo = cJSON_GetObjectItem(elem, kulcs);

    if (mezo != NULL && mezo->valuestring != NULL)
    {
        //Bemásoljuk a megfelelő helyrére a struktúrának az adatot, helyet hagyva a '\0' karakternek
        strncpy(cella, mezo->valuestring, meret - 1);
        cella[meret - 1] = '\0';
    }
}

staff_list_ptr jsonbol_listaba(cJSON *gyoker, char *p_felhnev, char *p_jelszo, staff_data *belepett_dolgozo)
{
    /**
     * @brief Egy cJSON tömböt kap és egy oda-vissza láncolt listát készít a dolgozók adatainak struktúrájával
     * @param gyoker A cJSON tömb (root)
     * @param p_felhnev A USER által megadott felhasználónév az összehasonlításhoz
     * @param p_jelszo A USER által megadott jelszó (titkosítva) az összehasonlításhoz
     * @param belepett_dolgozo A dolgozó adatainak struktúrája paraméterlistán  
     */
    //Megnézzük, hogy van-e érvényes json tömb
    if (gyoker == NULL || gyoker->type != cJSON_Array || cJSON_GetArraySize(gyoker) <= 0) return NULL;

    int dolgozok_szama = cJSON_GetArraySize(gyoker);
    int belep = 0;
    staff_data dolgozo;
    staff_list_ptr staff_fej = NULL;
    staff_list_ptr staff_vege = NULL;

    //Minden i-edik dolgozó adatát kimentjük, belerakjuk a struktúrába, és minden egyes struktúra elemet a lánchoz fűzünk
    for (int i = 0; i < dolgozok_szama; i++)
    {
        cJSON *elem = cJSON_GetArrayItem(gyoker, i);
        if (elem == NULL || elem->type != cJSON_Object) continue;
        
        mezo_masol(elem, "username", dolgozo.username, sizeof(dolgozo.username));
        mezo_masol(elem, "name", dolgozo.name, sizeof(dolgozo.name));
        mezo_masol(elem, "email", dolgozo.email, sizeof(dolgozo.email));
        mezo_masol(elem, "phone", dolgozo.phone, sizeof(dolgozo.phone));
        mezo_masol(elem, "pass_hash", dolgozo.pass_hash, sizeof(dolgozo.pass_hash));
        mezo_masol(elem, "role", dolgozo.role, sizeof(dolgozo.role));
        mezo_masol(elem, "second_factor_hash", dolgozo.second_factor_hash, sizeof(dolgozo.second_factor_hash));
        mezo_masol(elem, "rfid_uid", dolgozo.rfid_uid, sizeof(dolgozo.rfid_uid));

        //Foglalunk egy új láncszemet, ha nem sikerült, kilépünk, mert nincs elég memória
        staff_list_ptr uj = malloc(sizeof(staff_list));
        if (!uj)
        {
            perror("\nNincs eleg memoria!\n");
            lista_felaszabadit(staff_fej);
            return NULL;
        }
        uj->data = dolgozo;
        uj->next = NULL;

        //Ha az első elem, akkor a fej és a vége is ez lesz
        //Ha nem az első, akkor a végére szúrjuk a listának
        if (staff_vege == NULL)
        {
            uj->prev = NULL;
            staff_fej = uj;
            staff_vege = uj;
        }
        else if (staff_vege != NULL)
        {
            uj->prev = staff_vege;
            staff_vege->next = uj;
            staff_vege = uj;
        }
        
        //Itt ellenőrzöm, hogy beléphet-e az adott ember, ha nem, akkor felszabadítás és kilépés
        if (belephet_e(p_felhnev, p_jelszo, &dolgozo))
        {
            belep = 1;
            *belepett_dolgozo = dolgozo;
        }
        
    }
    //A cJSON fa már úgysem fog kelleni. Ha a belépés megtagadva, felszabadítunk
    if (gyoker != NULL) cJSON_Delete(gyoker);
    if (!belep)
    {
        printf("\nJogosulatlan probalkozas! Hozzaferes megtagadva!\n");
        lista_felaszabadit(staff_fej);
        return NULL;
    }
    return staff_fej;
}

int staff_fajl_beolvas(char *p_felhnev, char *p_jelszo, staff_data *p_belpett_dolgozo, staff_list_ptr *fej)
{
    /**
     * @brief Ez a függvény egy kisebb irányítópultként funkcionál. Innen indítom a fájl beolvasó függvényt, a lista készítő függvényt, kezelem a hibákat/jogosulatlan belépést, valamint visszaadom a kész listát a főprogramnak.
     * @param p_felhnev A USER által megadott felhasználónév (továbbküldöm másik függvénynek)
     * @param p_jelszo A USER által megadott jelszó (titkosított) (továbbküldöm másik függvénynek)
     * @param p_belepett_dolgozo A belépni kívánt USER adatainak struktúrája (továbbküldöm másik függvénynek)
     * @param fej A lista elő elemének pointere, melyet paraméterlistán adok vissza (továbbküldöm másik függvénynek)
     * @return 0 ha minden jó, ERROR (-1) ha hiba történt, JOGOSULATLAN (-2) ha nem jó a felhasználónév/jelszó
     */
    //Fájl beolvasó függvény meghívása
    char *tartalom = szekvenc_fajl_beolv("staff.json");
    if (!tartalom)
    {
        return ERROR;
    }
    //A felépített cJSON struktúra gyökerét adja vissza (megadott json fájl legkülső blokkja (maga a tömb))
    cJSON *gyoker = cJSON_Parse(tartalom);
    free(tartalom);
    if (!gyoker) return ERROR;

    //Kimentjük a lista első elemének pointerét (head)
    if ((*fej = jsonbol_listaba(gyoker, p_felhnev, p_jelszo, p_belpett_dolgozo)) == NULL)
    {
        return JOGOSULATLAN;
    }

    return 0;
}

int vendeg_fajl_beolvas(char *p_felhnev, char *p_jelszo, staff_data *p_bejelentkezett_user)
{
    printf("\nbeolvassuk a vendeg fajlt.\na felhasznalonev: %s\na jelszo: %s\n", p_felhnev, p_jelszo);
    return 0;
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

    while (1)
    {
        printf("\nBelepes.\nIrd be a felhasznaloneved (legalabb 3, legfeljebb 25 karakter): ");

        if (scanf("%29s", p_felhnev) == 1 && strlen(p_felhnev) >= 3 && strlen(p_felhnev) <= 25)
            break;
        else
            printf("Nem megfelelo felhasznalonev.\n");
    }
    while (1)
    {
        printf("A jelszo minimum 8, maximum 20 karakter hosszu.\nAdd meg a jelszavad ('~' karakter nem lehet benne): ");

        if (scanf("%24s", p_jelszo) == 1 && strlen(p_jelszo) >= 8 && strlen(p_jelszo) <= 20 && strcspn(p_jelszo, "~") == strlen(p_jelszo))
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
            case DOLGOZO: return DOLGOZO + BELEPES;
            case ADMIN: return ADMIN + BELEPES;
            case VENDEG: return VENDEG + BELEPES;
            }
        }
        //Ha nem volt megfelelő a jelszó, tájékoztatunk
        else if (strlen(p_jelszo) < 8)
        {
            printf("Tul rovid jelszo!\n");
            continue;
        }
        else if (strlen(p_jelszo) > 20)
        {
            printf("Tul hosszu jelszo!\n\n");
            continue;
        }
        else if (strcspn(p_jelszo, "~") != strlen(p_jelszo))
        {
            printf("Tiltott karakter a jelszoban!\n\n");
            continue;
        }
        else
        {
            printf("Nem adtal meg jelszot!\n\n");
            continue;
        }
    }
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
    staff_data bejelentkezett_user;
    staff_list_ptr staff_lista = NULL;

    opcio = koszontes();

    //A menüben melyik opciót választotta, az alapján megyünk tovább
    switch (opcio)
    {
    case BELEPES: opcio = belepes(felhnev, jelszo); break;
    case REGISZTRACIO: opcio = regiszracio(); break;
    case KILEPES: program_vege(); return 0;
    case ERROR: 
        printf("\nKerlek csak a megadott opciok kozul valassz!\n");
        koszontes();
    }
    
    //Ha már tudjuk mit akar csinálni és ki, akkor annak megfelelő fájlt olvasunk be
    switch (opcio)
    {
    case BELEPES + ADMIN:
        if (staff_fajl_beolvas(felhnev, jelszo, &bejelentkezett_user, &staff_lista) == JOGOSULATLAN)
        {
            program_vege();
            break;  
        }
        break;
    case BELEPES + DOLGOZO:
        if (staff_fajl_beolvas(felhnev, jelszo, &bejelentkezett_user, &staff_lista) == JOGOSULATLAN)
        {
            program_vege();
            break;
        }
        break;
    case BELEPES + VENDEG:
        vendeg_fajl_beolvas(felhnev, jelszo, &bejelentkezett_user);
        break;
    }
    
    printf("\n%s", jelszo);
    printf("\n%s %s %s", bejelentkezett_user.username, bejelentkezett_user.pass_hash, bejelentkezett_user.name);

    lista_felaszabadit(staff_lista);
    return 0;
}