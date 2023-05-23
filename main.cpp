#include <allegro.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
/*** TIMER ***/

long int speed=0;
void increment_speed() /// timer
{
    speed++;
}



/*** TIMER ***/
inline void init()
{
    allegro_init();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);

    install_timer();
    install_int_ex(increment_speed, BPS_TO_TIMER(60));
    install_keyboard();
    install_mouse();
    install_sound( DIGI_AUTODETECT, MIDI_AUTODETECT, "" );
    set_volume( 255, 255 );
    enable_hardware_cursor();
    select_mouse_cursor(MOUSE_CURSOR_ARROW);
}

inline void deinit()
{
    clear_keybuf();
    allegro_exit();
}

using namespace std;
/*** czcionki **/
FONT *czcionka;
FONT *menu_main;
FONT *menu2;
/*** czcionki **/
/**** DZWIEKI ***/
SAMPLE * najechanie = load_sample( "sounds/clicksound.wav" );
SAMPLE * enter = load_sample( "sounds/enter.wav" );
SAMPLE *togame = load_sample( "sounds/asasyni.wav" );
SAMPLE *save = load_sample( "sounds/save.wav" );
//void voice_set_playmode( najechanie, PLAYMODE_PLAY );
/**** DZWIEKI ***/
/** BITMAPY **/
BITMAP * bufor = NULL;
BITMAP * teren = NULL;
BITMAP *podloga;
BITMAP *tlo;
BITMAP *amulet;
BITMAP *kafelki;
BITMAP *ciemnosc;
BITMAP *aard_sign;
BITMAP *igni_sign;
BITMAP *postac;
BITMAP *sihill;
BITMAP *gwyhyr;
BITMAP *zbroja1;
BITMAP *zbroja2;
BITMAP *zbroja3;
BITMAP *duzy_geralt;
BITMAP *jaskolka_eliksir;
BITMAP *grom_eliksir;
BITMAP *atak;
BITMAP *boss;

BITMAP *tek1;
BITMAP *tek2;

BITMAP *sound;
/*** POTWORY **/
BITMAP *leshen;
/** BITMAPY **/

/**** STRUKTURY !!! ****/
struct player //klasa gracza
{
    void wczytaj();
    short lvl;
    int exp;
    int wym_exp;
    short p_x;
    short p_y;
    short hp;
    short max_hp;
    short mp;
    short max_mp;
    short dmg;
    short mag_dmg;
    int ruch = 0;
    short kierunek = 2;
    short attack = 0;
    void od_nowa();
    void atak();
    void zapisz();
    void regeneracja();
    void aard(int);
    void igni(int);
    void menu();
    void odnowienie_spelli();
    void wyswietl_staty();
    void dodaj_exp();
    void ekwipunek_wys();
    void przegrana(bool&);
    void add_lvl();
    short mp_regen;
    short hp_regen;
    short cooldown_regen = 0;
    short cooldown_spell = 0;
    short pancerz;
    bool act_znak = 0; /// ktory znak jest aktywny
    short act_bron;
    short act_pancerz;
    short act_misja;
};
struct dodatki
{
    int hp;
    int mp;
    int dmg;
    int magia;
    int def;
    bool efekt;
    void reset();
} bonusy;

struct item
{
    short id;
    int ilosc;
    void rys_eq(int,int);
    void wyswietl(BITMAP &adres,int,int);
    bool equiq = false;
} ekwipunek[12];

struct pocisk /// POCISH AARD/IGNI
{
    short poz_x;
    short poz_y;
    short kierunek;
    short sila;
    short stoper = 0;
    short dmg;
    short rodzaj;
    void wyswietl();
} shoot; /// deklaracja naszego pocisku

struct portale /// Portale do nastepnych map
{
    short poz_x;
    short poz_y;
    short wystep;
    short nr_dest;
    const char * destination;
};

struct monster /// Klasa potwora - statystyki
{
    bool exist = false;
    short poz_x;
    short poz_y;
    short wystep;
    short hp;
    short pancerz;
    short dmg;
    short agr_x = 5;
    short agr_y = 4;
    short mon_atak = 0; // Czy potwór atakowal
    short range = 1;
    short ruch = 0;
    void monsters_attack(int i);
    void move_monster(int nr_mobka);
    short podpalenie = 0;
    short cooldown_regen;
    bool boss = false;
    void efekty();
};

struct eliksir /// NASZE ELIKSIRY
{
    int ilosc;
    int czas_dzialania;
    void wypij(bool);
} jaskolka,grom;



/** KAFELEK **/
struct kafelek
{
    bool valid;
    int numer;
};
/*** STRUKTURY ***/
/** KAFELEK **/

/************** MISJA **********/

struct misja
{
    int id;
    int wym;
    int act = 0;
    int exp;
    int gold;
    int dest;
    bool aktywnosc = false;
    int mutacja_s = 0;
    int mutacja_magic = 0;
    int mutacja_hp = 0;
    int mutacja_mp = 0;
    int zbroja = 0;
    int bron = 0;
    int swallows;
    int groms;
    void dodaj(int);
} misje[20];

/************** MISJA **********/


/** MAPA NASZA + STRUKTURY INNE :3 **/
 const short int y_table = 27; // Wielkosc mapy
 const short int x_table = 53; // -||-
 //const short int x_table_3 = 59; // -||-
kafelek mapa[y_table][x_table]; /// mapa, po ktorej bedziemy sie poruszac
monster potwory[20]; /// Potwory
player gracz; // NASZ BOHATER
int mapa_x = 0, mapa_y = 0; /// Granice mapy
int act_mapa = 1; /// mapa, ktora jest wyswietlana
bool spacja; /// czy uzyto spacji do ataku
/** PORTALE - TABLICA **/
bool is_sound = true;
bool p_key = false;


portale teleport[20];
/** PORTALE **/
// POZ GRACZA

/** FUNKCJE **/
/**** MISJE ***/

void misje_przypisz() /// lista misji
{
    misje[0].wym = 10;
    misje[0].exp = 45;
    misje[0].bron = 1;
    misje[0].zbroja = 1;
    misje[0].swallows = 6;
    misje[0].groms = 6;

    misje[1].wym = 15;
    misje[1].exp = 80;
    misje[1].mutacja_hp = 150;
    misje[1].mutacja_mp = 80;


    misje[2].wym = 20;
    misje[2].exp = 150;
    misje[2].mutacja_magic = 15;
    misje[2].mutacja_s = 20;

    misje[3].wym = 25;
    misje[3].exp = 200;
    misje[3].swallows = 10;
    misje[3].groms = 10;
    misje[3].bron = 2;
    misje[3].zbroja = 2;

    misje[4].wym = 30;
    misje[4].exp = 250;
    misje[4].mutacja_mp = 250;
    misje[4].mutacja_hp = 150;

    misje[5].wym = 40;
    misje[5].exp = 350;
    misje[5].mutacja_s = 40;
    misje[5].mutacja_magic = 30;

    misje[6].wym = 45;
    misje[6].exp = 500;
    misje[6].swallows = 20;
    misje[6].groms = 20;

    misje[7].wym = 50;
    misje[7].exp = 700;
    misje[7].mutacja_hp = 300;
    misje[7].mutacja_mp = 200;
    misje[7].bron = 3;
    misje[7].zbroja = 3;

    misje[8].wym = 55;
    misje[8].exp = 800;
    misje[8].mutacja_s = 60;
    misje[8].mutacja_magic = 50;

    misje[9].wym = 60;
    misje[9].exp = 1000;
    misje[9].swallows = 30;
    misje[9].groms = 20;

}
void misja::dodaj(int id) /// dodaje mobki, konczy misje
{
    act++;
    if(act >= wym)
    {
        gracz.exp += exp;
        aktywnosc = false;
        /** BRONIE **/
        if(bron == 1) ekwipunek[0].ilosc = 1;
        else if(bron == 2) ekwipunek[1].ilosc = 1;
        else if(bron == 3) ekwipunek[2].ilosc = 1;
        /** ZBROJE **/
        if(zbroja == 1) ekwipunek[3].ilosc = 1;
        else if(zbroja == 2) ekwipunek[4].ilosc = 1;
        else if(zbroja == 3) ekwipunek[5].ilosc = 1;
        /// &*******&
        if(mutacja_hp > 0) gracz.max_hp += mutacja_hp;
        if(mutacja_mp > 0) gracz.max_mp += mutacja_mp;
        if(mutacja_s > 0) gracz.dmg += mutacja_s;
        if(mutacja_magic > 0) gracz.mag_dmg += mutacja_magic;
        if(swallows > 0) jaskolka.ilosc += swallows;
        if(groms > 0) grom.ilosc += groms;
        misje[id+1].aktywnosc = true;
        misje[id+1].act = 0;



        gracz.act_misja++;
    }
}

/**** MISJE ***/
/******* ELIKSIRY **********/

void eliksir::wypij(bool eliksir) /// wypija dany eliksir
{
    if(eliksir == 0)
        czas_dzialania = 5;
    else
        czas_dzialania = 10;
    ilosc--;
}

/******* ELIKSIRY **********/

/*** PLAYER ***/
void player::od_nowa() /// nowa gra
{
    lvl = 1;
    exp = 0;
    wym_exp = 50;
    p_x = 3;
    p_y = 3;
    hp = 150;
    max_hp = 150;
    mp = 100;
    max_mp = 100;
    dmg = 17;
    mag_dmg = 7;
    ruch = 0;
    kierunek = 2;
    attack = 0;
    mp_regen = 5;
    hp_regen = 7;
    cooldown_regen = 0;
    cooldown_spell = 0;
    pancerz = 8;
    act_znak = 0; /// ktory znak jest aktywny

    mapa_x = 0;
    mapa_y = 0;
    act_bron = 0;
    act_pancerz = 0;
    act_misja = 0;

    for(int i = 0; i < 7; i++)
    {
        ekwipunek[i].ilosc = 0;
        ekwipunek[i].equiq = false;
    }
    for(int i = 0; i < 20; i++)
    {
        misje[i].aktywnosc = false;
        misje[i].act = 0;
    }
    misje[0].aktywnosc = true;

    act_mapa = 1;

    zapisz();
}
void player::wczytaj() /// wczytuje dane z pliku
{
    fstream wczytaj;
    int nr_linii = 0;
    string linia;
    wczytaj.open("gracz.txt");
    while(getline(wczytaj,linia))
    {
        switch(nr_linii)
        {
        case 0:
        {
            lvl = atoi(linia.c_str());
            break;
        };
        case 1:
        {
            exp = atoi(linia.c_str());
            break;
        };
        case 2:
        {
            wym_exp = atoi(linia.c_str());
            break;
        };
        case 3:
        {
            hp = atoi(linia.c_str());
            break;
        };
        case 4:
        {
            hp_regen = atoi(linia.c_str());
            break;
        };
        case 5:
        {
            mp = atoi(linia.c_str());
            break;
        };
        case 6:
        {
            mp_regen = atoi(linia.c_str());
            break;
        };
        case 7:
        {
            dmg = atoi(linia.c_str());
            break;
        };
        case 8:
        {
            mag_dmg = atoi(linia.c_str());
            break;
        };
        case 9:
        {
            act_bron = atoi(linia.c_str());
            break;
        };
        case 10:
        {
            act_pancerz = atoi(linia.c_str());
            break;
        };
        case 11:
        {
            pancerz = atoi(linia.c_str());
            break;
        };
        case 12:
        {
            max_hp = atoi(linia.c_str());
            break;
        };
        case 13:
        {
            max_mp = atoi(linia.c_str());
            break;
        };
        case 14:
        {
            jaskolka.ilosc = atoi(linia.c_str());
            break;
        };
        case 15:
        {
            grom.ilosc = atoi(linia.c_str());
            break;
        };
        case 16:
        {
            misje[atoi(linia.c_str())].aktywnosc = true;
            gracz.act_misja = atoi(linia.c_str());
            break;
        };
        case 17:
        {
            misje[act_misja].act = atoi(linia.c_str());
            break;
        };
        case 18:
        {
            act_mapa = atoi(linia.c_str());
            break;
        };
        }
        nr_linii++;
    }
}
void player::zapisz()
{
    fstream zapisz;
    int nr_linii = 0;
    string linia;
    zapisz.open("gracz.txt");
    while(nr_linii <= 18)
    {
        switch(nr_linii)
        {
        case 0:
        {
            //lvl = atoi(linia.c_str());
            zapisz<<lvl<<endl;
            break;
        };
        case 1:
        {
            //exp = atoi(linia.c_str());
             zapisz<<exp<<endl;
            break;
        };
        case 2:
        {
           // wym_exp = atoi(linia.c_str());
            zapisz<<wym_exp<<endl;
            break;
        };
        case 3:
        {
            //hp = atoi(linia.c_str());
             zapisz<<hp<<endl;
            break;
        };
        case 4:
        {
           // hp_regen = atoi(linia.c_str());
            zapisz<<hp_regen<<endl;
            break;
        };
        case 5:
        {
           // mp = atoi(linia.c_str());
            zapisz<<mp<<endl;
            break;
        };
        case 6:
        {
           // mp_regen = atoi(linia.c_str());
            zapisz<<mp_regen<<endl;
            break;
        };
        case 7:
        {
            //dmg = atoi(linia.c_str());
             zapisz<<dmg<<endl;
            break;
        };
        case 8:
        {
           // mag_dmg = atoi(linia.c_str());
            zapisz<<mag_dmg<<endl;
            break;
        };
        case 9:
        {
           // act_bron = atoi(linia.c_str());
            zapisz<<act_bron<<endl;
            break;
        };
        case 10:
        {
           // act_pancerz = atoi(linia.c_str());
            zapisz<<act_pancerz<<endl;
            break;
        };
        case 11:
        {
           // pancerz = atoi(linia.c_str());
            zapisz<<pancerz<<endl;
            break;
        };
        case 12:
        {
            //max_hp = atoi(linia.c_str());
             zapisz<<max_hp<<endl;
            break;
        };
        case 13:
        {
           // max_mp = atoi(linia.c_str());
            zapisz<<max_mp<<endl;
            break;
        };
        case 14:
        {
           // max_mp = atoi(linia.c_str());
            zapisz<<jaskolka.ilosc<<endl;
            break;
        };
        case 15:
        {
           // max_mp = atoi(linia.c_str());
            zapisz<<grom.ilosc<<endl;
            break;
        };
        case 16:
        {
           // max_mp = atoi(linia.c_str());
            zapisz<<gracz.act_misja<<endl;
            break;
        };
        case 17:
        {
           // max_mp = atoi(linia.c_str());
            zapisz<<misje[act_misja].act<<endl;
            break;
        };
        case 18:
        {
           // max_mp = atoi(linia.c_str());
            zapisz<<act_mapa<<endl;
            break;
        };
        }
        nr_linii++;
    }
}

void player::dodaj_exp() /// jesli potwor zginal dodaje exp
{
    for(int i = 0; i < 20; i++)
    {
        if(potwory[i].hp <= 0 && potwory[i].exist == true)
        {
            potwory[i].exist = false; /// teraz nie bedzie brany pod uwage
            exp += 10;
            potwory[i].hp = 0;

            for(int i = 0; i < 20; i++) /// dodawanie mobkow do misji
            {
                //if(act_mapa == misje[i].dest) misje[i].act++;
                if(misje[i].aktywnosc == true)
                    misje[i].dodaj(i);
            }

        }
    }
}
void player::add_lvl()
{
    lvl++;
    exp = 0;
    wym_exp *= 1.5;
    /// poprawa statystyk
    dmg *= 1.2;
    max_hp *= 1.2;
    hp_regen += 1;
    mp_regen += 1;
    mag_dmg *= 1.2;
    max_mp *= 1.2;
    pancerz *= 1.2;

    hp = max_hp;
    mp = max_mp;


}
void player::odnowienie_spelli() /// odnawia cooldown spelli
{
    if(gracz.cooldown_spell > 0)
        gracz.cooldown_spell--;
}
void player::aard(int kierunek_zakl) /// rzuca zaklecie aard
{
    shoot.poz_x = p_x;
    shoot.poz_y = p_y;
    shoot.kierunek = kierunek_zakl;
    shoot.sila = 16;
    shoot.dmg = mag_dmg+bonusy.magia;
    cooldown_spell= 30;
    shoot.rodzaj = 1;
    mp-=15;
}
void player::igni(int kierunek_zakl) /// rzuca zaklecie igni
{
    shoot.poz_x = p_x;
    shoot.poz_y = p_y;
    shoot.kierunek = kierunek_zakl;
    shoot.sila = 8;
    shoot.dmg = (mag_dmg+bonusy.magia)*1.5;
    cooldown_spell = 10;
    shoot.rodzaj = 2;
    mp-=10;
}
void player::regeneracja() /// regeneracja hp i mp
{
    if(cooldown_regen == 0)
    {
        if(hp+bonusy.hp < max_hp+bonusy.hp)
        {
            if(jaskolka.czas_dzialania == 0) hp += hp_regen;
            else hp += hp_regen*4; /// jaskolka ulepsza regeneracje 4-krotnie
        }


        if(mp+bonusy.mp < max_mp+bonusy.mp)
        {
            if(jaskolka.czas_dzialania == 0) mp += mp_regen;
            else  mp += mp_regen*3; /// jaskolka ulepsza regeneracje trzykrotnie
        }

        if(grom.czas_dzialania > 0)
            grom.czas_dzialania--;

        if(hp+bonusy.hp > max_hp+bonusy.hp)
            hp = max_hp; /// nie mozna przekroczyc max hp

        if(mp+bonusy.mp > max_mp+bonusy.mp)
            mp = max_mp; /// nie mozna przekroczyc max mp

        if(jaskolka.czas_dzialania > 0) jaskolka.czas_dzialania--;

        cooldown_regen = 60;
    }
    else
        cooldown_regen--;
}
void player::atak() /// Atak mieczem, wyprowadzany spacja
{
    for(int i = 0; i < 20; i++)
    {
        int odl_x = potwory[i].poz_x-p_x; /// odleglosci miedzy potworem a graczem
        int odl_y = potwory[i].poz_y-p_y; /// GRACZ

        if(potwory[i].exist  == true)
        {
            if(odl_x <= 1 && odl_y <= 1 && odl_x>= -1 && odl_y >= -1)
            {
                if(attack == 0)
                {
                    if(grom.czas_dzialania > 0) /// czy grom jest aktywny
                    {
                        potwory[i].hp-=(dmg+bonusy.dmg)*1.5-potwory[i].pancerz; /// mocniejszy efekt gromu
                    }
                    else
                    {
                        potwory[i].hp-=(dmg+bonusy.dmg)-potwory[i].pancerz;
                    }

                }
            }
        }

    }
}
void player::wyswietl_staty() /// wyswietlanie dolnej czesci ekranu
{
    /** STATY GRACZA NA DOLE **/
    draw_sprite(bufor,amulet, 0, 500);
    textprintf_ex(bufor,czcionka, 100, 500, makecol(220,20,60),-1, "LVL %d ", lvl);
    textprintf_ex(bufor,czcionka, 100, 520, makecol(220,20,60),-1, "EXP %d / %d ", exp,wym_exp);
    textprintf_ex(bufor,czcionka, 80, 540, makecol(255,0,0),-1, "HP: %d / %d", hp+bonusy.hp, max_hp+bonusy.hp);
    textprintf_ex(bufor,czcionka, 80, 560, makecol(0,0,255),-1, "MP  %d / %d", mp+bonusy.mp, max_mp+bonusy.mp);

    /**************** ZNAKI **/

    /// sterowanie znakami :D

    /* if(shoot.kierunek == 1) textout_ex(bufor, font, "W", 245, 560, makecol(255, 255, 0), 0);
     else textout_ex(bufor, font, "W", 245, 560, makecol(255, 255, 255), 0);
     if(shoot.kierunek == 3)textout_ex(bufor, font, "S", 245, 580, makecol(255, 255, 0), 0);
     else textout_ex(bufor, font, "S", 245, 580, makecol(255, 255, 255), 0);
     if(shoot.kierunek == 4)textout_ex(bufor, font, "A", 230, 570, makecol(255, 255, 0), 0);
     else textout_ex(bufor, font, "A", 230, 570, makecol(255, 255, 255), 0);
     if(shoot.kierunek == 2)textout_ex(bufor, font, "D", 260, 570, makecol(255, 255, 0), 0);
     else textout_ex(bufor, font, "D", 260, 570, makecol(255, 255, 255), 0); */

    // if(gracz.act_znak == 0)
    draw_sprite(bufor,aard_sign,225,536);
    //else
    draw_sprite(bufor,igni_sign,309,530);

    if(act_znak == 1) textprintf_ex(bufor,czcionka, 235, 580, makecol(255,255,255),-1, "Aard [Z]");
    else textprintf_ex(bufor,czcionka, 225, 580, makecol(225,255,0),-1, "Aard [Z]");
    if(act_znak == 0)textprintf_ex(bufor,czcionka, 309, 580, makecol(255,255,255),-1, "Igni [X]");
    else textprintf_ex(bufor,czcionka, 309, 580, makecol(255,255,0),-1, "Igni [X]");
    /**** ZNAKI **/

    draw_sprite(bufor, jaskolka_eliksir, 390,536);/// rys_eliksiru Jaskolka
    textprintf_ex(bufor,czcionka, 390, 580, makecol(255,255,255),-1, "[1]: %d ", jaskolka.ilosc); /// ilosc jaskolek
    if(jaskolka.czas_dzialania > 0)
        rectfill(bufor, 390, 520, 390+jaskolka.czas_dzialania*9, 530, makecol(255,0,0)); /// ile czasu pozostalo
    else
        rectfill(bufor, 390, 520, 435, 530, makecol(0,255,0)); /// mozna uzyc



    draw_sprite(bufor, grom_eliksir, 450,536); /// rys_eliksiru Grom
    textprintf_ex(bufor,czcionka, 450, 580, makecol(255,255,255),-1, "[2]: %d ", grom.ilosc); /// ilosc gromow
    if(grom.czas_dzialania > 0)
        rectfill(bufor, 450, 520, 450+(grom.czas_dzialania*9)/2, 530, makecol(255,0,0)); /// ile czasu pozostalo
    else
        rectfill(bufor, 450, 520, 495, 530, makecol(0,255,0)); /// mozna uzyc



    rect(bufor,550,510,790,540, makecol(255,255,255));
    bool brek = true;
    int nr = 0;

    if(misje[8].aktywnosc == false)
        {
    while(brek)
    {
        if(misje[nr].aktywnosc == 1)
        {
            textprintf_ex(bufor,czcionka, 555, 515, makecol(255,255,255),-1, "Zabij %d potworow: %d / %d",
                       misje[nr].wym, misje[nr].act, misje[nr].wym);
            textout_ex(bufor, czcionka, "Nagroda", 555, 540, makecol(220, 20, 60), -1);

            textprintf_ex(bufor,czcionka, 655, 540, makecol(255,255,255),-1, "exp: %d", misje[nr].exp);
        if(misje[nr].bron > 0) textprintf_ex(bufor,czcionka, 655, 555, makecol(255,255,255),-1, "bron nr: %d", misje[nr].bron);
        if(misje[nr].zbroja > 0) textprintf_ex(bufor,czcionka, 655, 575, makecol(255,255,255),-1, "zbroja nr: %d", misje[nr].zbroja);

            if(misje[nr].groms > 0)
                textprintf_ex(bufor,czcionka, 555, 555, makecol(255,255,255),-1, "gromy: %d", misje[nr].groms);
            if(misje[nr].swallows > 0)
                textprintf_ex(bufor,czcionka, 555, 575, makecol(255,255,255),-1, "jaskolki: %d", misje[nr].swallows);

            if(misje[nr].mutacja_hp > 0)
                textprintf_ex(bufor,czcionka, 555, 555, makecol(255,255,255),-1, "hp: %d", misje[nr].mutacja_hp);
            if(misje[nr].mutacja_mp > 0)
                textprintf_ex(bufor,czcionka, 555, 575, makecol(255,255,255),-1, "mp: %d", misje[nr].mutacja_mp);

            if(misje[nr].mutacja_s > 0)
                textprintf_ex(bufor,czcionka, 555, 555, makecol(255,255,255),-1, "dmg: %d", misje[nr].mutacja_s);
            if(misje[nr].mutacja_magic > 0)
                textprintf_ex(bufor,czcionka, 555, 575, makecol(255,255,255),-1, "magia: %d", misje[nr].mutacja_magic);

            brek = false;
        }
        nr++;
    }
    }
    else
    {
      textout_ex(bufor, czcionka, "Zakonczyles swa misje", 550, 510, makecol(220, 20, 60), -1);
    }
    //  textprintf(bufor,font, 560, 530, makecol(255,255,255), "Godzina: %d ", godzina/125); /// godzina



}

/*** ATTACK ***/

/****** EKWIPUNEK *******/

void player::ekwipunek_wys() /// Wyswietla ekwipunek po kliknieciu [I]
{
    rest(200);
    while(!key[KEY_I])
    {
        rectfill(bufor,0,0,800, 600, makecol(31,66,34));
        show_mouse( screen );
        //unscare_mouse();
        int nr_eq = 0;
        for(int i = 100; i < 450; i+= 100) /// start wyswietlanie ekwipunku
        {
            for(int j = 400; j <= 600; j+=100)
            {
                if(ekwipunek[nr_eq].id != 0)
                {
                    if(ekwipunek[nr_eq].equiq)
                        rectfill(bufor,j,i,j+75,i+75, makecol(11,46,14));

                    ekwipunek[nr_eq].rys_eq(j,i);
                }

                rect(bufor, j, i, j+75, i+75, makecol(31,76,34));
                nr_eq++;
            }
        }
        if(mouse_b == 1)
        {
            int mx = mouse_x;
            int my = mouse_y;
            if(mx >= 400 && mx <= 475 && my >= 100 && my <= 175 && ekwipunek[0].ilosc > 0)
            {
                gracz.act_bron = 1;
                ekwipunek[0].equiq = true;
                ekwipunek[1].equiq = false;
                ekwipunek[2].equiq = false;
                bonusy.reset();
            }
            if(mx >= 500 && mx <= 575 && my >= 100 && my <= 175 && ekwipunek[1].ilosc > 0)
            {
                gracz.act_bron = 2;
                ekwipunek[0].equiq = false;
                ekwipunek[1].equiq = true;
                ekwipunek[2].equiq = false;
                bonusy.reset();
            }
            if(mx >= 600 && mx <= 675 && my >= 100 && my <= 175 && ekwipunek[2].ilosc > 0)
            {
                gracz.act_bron = 3;
                ekwipunek[0].equiq = false;
                ekwipunek[1].equiq = false;
                ekwipunek[2].equiq = true;
                bonusy.reset();
            }

            if(mx >= 400 && mx <= 475 && my >= 200 && my <= 275 && ekwipunek[3].ilosc > 0)
            {
                gracz.act_pancerz = 1;
                ekwipunek[3].equiq = true;
                ekwipunek[4].equiq = false;
                ekwipunek[5].equiq = false;
                bonusy.reset();
            }
            if(mx >= 500 && mx <= 575 && my >= 200 && my <= 275 && ekwipunek[4].ilosc > 0)
            {
                gracz.act_pancerz = 2;
                ekwipunek[3].equiq = false;
                ekwipunek[4].equiq = true;
                ekwipunek[5].equiq = false;
                bonusy.reset();
            }
            if(mx >= 600 && mx <= 675 && my >= 200 && my <= 275 && ekwipunek[5].ilosc > 0)
            {
                gracz.act_pancerz = 3;
                ekwipunek[3].equiq = false;
                ekwipunek[4].equiq = false;
                ekwipunek[5].equiq = true;
                bonusy.reset();
            }
        }
        draw_sprite(bufor, duzy_geralt, 100, 150);
        textprintf_ex(bufor,czcionka, 125, 125, makecol(220,20,60),-1, "LVL %d ", lvl);

        textprintf_ex(bufor,czcionka, 75, 425, makecol(220,20,60),-1, "EXP %d / %d ", exp,wym_exp);
        textprintf_ex(bufor,czcionka, 75, 450, makecol(255,0,0),-1, "HP: %d / %d ", hp+bonusy.hp, max_hp+bonusy.hp);
        textprintf_ex(bufor,czcionka, 75, 475, makecol(0,0,255),-1, "MP  %d / %d ", mp+bonusy.mp, max_mp+bonusy.mp);

        textprintf_ex(bufor,czcionka, 250, 425, makecol(255,128,0),-1, "DMG:    %d ", dmg+bonusy.dmg);
        textprintf_ex(bufor,czcionka, 250, 450, makecol(100,149,237),-1, "MAGIA:  %d ", mag_dmg+bonusy.magia);
        textprintf_ex(bufor,czcionka, 250, 475, makecol(220,20,60),-1, "OBRONA: %d ", pancerz+bonusy.def);

        draw_sprite(screen,bufor,0,0);
        rest(20);
    }
    for(int i = 0; i < 20; i++)
    {
        if(potwory[i].exist == true) potwory[i].ruch = 60;
    }
    rest(200);
}

/****** EKWIPUNEK *******/
/*** MOZLIWOSC RUCHU *****/
/************************************* GRACZ *****************************************/

/// 1 - gora, 2 - prawo, 3 - dol, 4-lewo
bool czypotwor(int kierunek) /// czy idac nie wchodzimy na potwora
{
    if(kierunek == 1) /// gora
    {
        for(int i = 0; i < 20; i++)
        {
            if(potwory[i].poz_y == gracz.p_y-1 && potwory[i].poz_x ==gracz.p_x && potwory[i].wystep == act_mapa && potwory[i].hp > 0)
                return false;
        }
        return true;
    }

    else if(kierunek == 2) /// prawo
    {
        for(int i = 0; i < 20; i++)
        {
            if(potwory[i].poz_y == gracz.p_y && potwory[i].poz_x == gracz.p_x+1 && potwory[i].wystep == act_mapa && potwory[i].hp > 0)
                return false;
        }
        return true;
    }

    else if(kierunek == 3) /// dol
    {
        for(int i = 0; i < 20; i++)
        {
            if(potwory[i].poz_y == gracz.p_y+1 && potwory[i].poz_x == gracz.p_x && potwory[i].wystep == act_mapa && potwory[i].hp > 0)
                return false;
        }
        return true;
    }

    else if(kierunek == 4) /// lewo
    {
        for(int i = 0; i < 20; i++)
        {
            if(potwory[i].poz_y == gracz.p_y && potwory[i].poz_x == gracz.p_x-1 && potwory[i].wystep == act_mapa && potwory[i].hp > 0)
                return false;
        }
        return true;
    }
}

void eq_wczytaj() /// ilosc danego eq z pliku
{
    fstream item;
    item.open("item.txt");
    int nr_linii = 0;
    string linia;
    while(getline(item,linia))
    {
        ekwipunek[nr_linii].ilosc = atoi(linia.c_str());
        nr_linii++;
    }

}

void eq_zapisz()
{
fstream item;
    item.open("item.txt");
    int nr_linii = 0;
    string linia;
    for(int i = 0; i < 6; i++)
    {
        item<<ekwipunek[i].ilosc<<endl;
    }

}

void eq_lista() /// wczytuje ekwipunek
{
    for(int i = 0; i < 6; i++)
    {
        ekwipunek[i].id = i+1;
    }

    eq_wczytaj();
    /**ekwipunek[0].id = 1;
    ekwipunek[0].ilosc = 0;
    ekwipunek[1].id = 2;
    ekwipunek[1].ilosc = 0;
    ekwipunek[2].id = 3;
    ekwipunek[2].ilosc = 0;
    ekwipunek[3].id = 4;
    ekwipunek[3].ilosc = 0;
    ekwipunek[4].id = 5;
    ekwipunek[4].ilosc = 0;
    ekwipunek[5].id = 6;
    ekwipunek[5].ilosc = 0;**/

    if(gracz.act_bron == 1) ekwipunek[0].equiq = true;
    else if(gracz.act_bron == 2) ekwipunek[1].equiq = true;
    else if(gracz.act_bron == 3) ekwipunek[2].equiq = true;

    if(gracz.act_pancerz == 1) ekwipunek[3].equiq = true;
    else if(gracz.act_pancerz == 2) ekwipunek[4].equiq = true;
    else if(gracz.act_pancerz == 3) ekwipunek[5].equiq = true;
}

void item::wyswietl(BITMAP &adres,int j, int i) /// wyswietla item
{
    draw_sprite(bufor, &adres, j, i);
    textprintf(bufor,font, j+5, i+65, makecol(255,255,255), "Ilosc %d ", ilosc); /// ilosc gromow

}

void item::rys_eq(int j, int i) /// nawiazuje do wyswietlenia
{
    switch(id)
    {
    case 1:
    {
        wyswietl(*sihill,j,i);
        break;
    }
    case 2:
    {
        wyswietl(*gwyhyr,j,i);
        break;
    }
    case 3:
    {
        wyswietl(*gwyhyr,j,i);
        break;
    }
    case 4:
    {
        wyswietl(*zbroja1,j,i);
        break;
    }
    case 5:
    {
        wyswietl(*zbroja2,j,i);
        break;
    }
    case 6:
    {
        wyswietl(*zbroja3,j,i);
        break;
    }
    }
}

void dodatki::reset() /// resetuje bonusy z itemow
{
    def = 0;
    dmg = 0;
    efekt = 0;
    hp = 0;
    magia = 0;
    mp = 0;
    /** miecze */
//if(ekwipunek[0].id == 1 && ekwipunek[0].equiq == true)
    if(gracz.act_bron == 1)
    {
        dmg = 10;
        hp = 20;
        mp = 20;
        magia = 5;
        ekwipunek[0].equiq = true;
//gracz.act_bron = 1;
    }
//else if(ekwipunek[1].id == 2 && ekwipunek[1].equiq == true)
    else if(gracz.act_bron == 2)
    {
        dmg = 20;
        hp = 100;
        mp = 60;
        magia = 15;
        ekwipunek[1].equiq = true;
//gracz.act_bron = 2;
    }
//else if(ekwipunek[2].id == 3 && ekwipunek[2].equiq == true)
    else if(gracz.act_bron == 3)
    {
        dmg = 50;
        hp = 200;
        mp = 150;
        magia = 30;
        ekwipunek[2].equiq = true;
//gracz.act_bron = 3;
    }
    /** miecze */

    /******* ZBROJE *****/

//if(ekwipunek[3].id == 4 && ekwipunek[3].equiq == true)
    if(gracz.act_pancerz == 1)
    {
        def = 10;
        hp = 20;
        mp = 20;
        magia = 3;
        ekwipunek[3].equiq = true;
//gracz.act_pancerz = 1;
    }
//else if(ekwipunek[4].id == 5 && ekwipunek[4].equiq == true)
    else if(gracz.act_pancerz == 2)
    {
        def = 20;
        hp = 100;
        mp = 60;
        magia = 5;
        ekwipunek[4].equiq = true;
//gracz.act_pancerz = 2;
    }
//else if(ekwipunek[5].id == 6 && ekwipunek[5].equiq == true)
    else if(gracz.act_pancerz == 3)
    {
        def = 50;
        hp = 200;
        mp = 150;
        magia = 10;
        ekwipunek[5].equiq = true;
//gracz.act_pancerz = 3;
    }

    /******* ZBROJE *****/


}
/*** MOZLIWOSC RUCHU *****/

/****************************** PLIKI ************************************************/
/************************************ RYSOWANIE MAPY *******************************/
/*** TELEPORTY CZY KOLIZJA **/
int przypisz(int i, int j, int nr_mapy, int gatunek) /// Przypisanie wartosci potworowi
{
    for(int it = 0; it < 20; it++)
    {
        if(potwory[it].exist == false)
        {
            potwory[it].exist = true;
            potwory[it].poz_x = j;
            potwory[it].poz_y = i;
            if(gatunek == 1) /// slabszy mobek
            {
                potwory[it].hp= 45*(gracz.lvl*0.5);
                potwory[it].dmg= 15*gracz.lvl;
                potwory[it].pancerz = 5*(gracz.lvl*0.5);
            }
            else if(gatunek == 2) /// boss
            {
                potwory[it].hp= 100*(gracz.lvl*0.5);
                potwory[it].dmg= 20*gracz.lvl;
                potwory[it].pancerz = 10*(gracz.lvl*0.5);
            }
            else if(gatunek == 3) /// boss
            {
                potwory[it].hp= 3000;
                potwory[it].dmg= 300;
                potwory[it].pancerz = 50;
                potwory[it].boss = true;
            }

            potwory[it].wystep = nr_mapy; /// przypisanie mapy mobkowi
            potwory[it].podpalenie = 0; /// nie moze byc podpalony
            return 0;
        }
    }
}

void przyp_act_mapy(const char * nazwa_pliku, int nr_mapy) /// Przepisywanie mapy z pliku
{
    int dlugosc_x;
    int ilosc_mob = 0;
    for(int i = 0; i < 20; i++)
    {
        potwory[i].exist = false;
    }
    fstream file_map;
    file_map.open(nazwa_pliku);
    act_mapa = nr_mapy;

    //if(nazwa_pliku == "maps/3.txt")  dlugosc_x = x_table_3;
   // else dlugosc_x = x_table;

    for(int i = 0; i < y_table; i++)
    {
        for(int j = 0; j < x_table; j++)
        {
            file_map >> mapa[i][j].numer;
            if(mapa[i][j].numer == 3)
            {
                przypisz(i,j,nr_mapy,1); /// MOB
            }
            if(mapa[i][j].numer == 5)
            {
                przypisz(i,j,nr_mapy,2); /// BOSS
            }
            if(mapa[i][j].numer == 13)
            {
                przypisz(i,j,nr_mapy,3); /// BOSS
            }
            mapa[i][j].valid = mapa[i][j].numer%2; /// wejscie na pole
        }
    }
    file_map.close();
}

void rekur_tp(int i)/*** TELEPORTY CZY KOLIZJA **/
{
    if(i >= 0)
    {
        if(gracz.p_y == teleport[i].poz_y && gracz.p_x == teleport[i].poz_x && teleport[i].wystep == act_mapa)
        {
            przyp_act_mapy(teleport[i].destination, teleport[i].nr_dest);
            mapa_x = 0;
            mapa_y = 0;
            gracz.p_y = 3;
            gracz.p_x = 3;
        }
        if(i > 0)
            return rekur_tp(i-1);
    }


}


/**** RYSOWANIE MAPY ****/
bool rys_kafelek_bool(int mapa_x, int mapa_y, int j, int i) /// CZY KAFELEK NIE WYCHODZI ZA MAPE
{
    if(j+mapa_x < 53 && j+mapa_x >= 0 && i+mapa_y < 27 && i+mapa_y >= 0)
        return true;
    else
        return false;
}

void  wys_mape(int p_x, int p_y) /// RYSUJE KAFELKI I POTWORY C:
{
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            if(rys_kafelek_bool( mapa_x, mapa_y, j, i))
            {
                switch(mapa[i+mapa_y][j+mapa_x].numer)
                {
                case 1:
                {
                    blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 2:
                {
                    blit( kafelki, bufor, 100, 50, j*50,i*50, 50, 50 );
                    break;
                }
                case 3:
                {
                    blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 4:
                {
                    blit( kafelki, bufor, 150, 50, j*50,i*50, 50, 50 );
                    break;
                }
                case 5:
                {
                    blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 6:
                {
                    blit( kafelki, bufor, 50, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 7:
                {
                    blit( kafelki, bufor, 0, 50, j*50,i*50, 50, 50 );
                    break;
                }
                case 8:
                {
                    blit( kafelki, bufor, 100, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 9:
                {
                    blit( kafelki, bufor, 50, 50, j*50,i*50, 50, 50 );
                    break;
                }
                case 10:
                {
                    blit( kafelki, bufor, 150, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 12:
                {
                    blit( kafelki, bufor, 200, 0, j*50,i*50, 50, 50 );
                    break;
                }
                case 13:
                {
                    blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
                    break;
                }
                default:
                {
                    blit( kafelki, bufor, 200, 50, j*50,i*50, 50, 50 );
                    break;
                }
                }
                // if(mapa[i+mapa_y][j+mapa_x].numer == 1)
                // blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
                // else
                // {
                // blit( kafelki, bufor, 200, 50, j*50,i*50, 50, 50 );
                //}
                /** PORTALE **/
                for(int it = 0; it < 20; it++)
                {
                    if(teleport[it].poz_x == j+mapa_x && teleport[it].poz_y == i+mapa_y && teleport[it].wystep == act_mapa)
                    {
                        rectfill(bufor, j*50, i*50, j*50+50, i*50+50, makecol(0,0,255)); /// rysujemy teleport
                       draw_sprite(bufor,aard_sign,j*50,i*50);
                    }
                    /** PORTALE **/
                    /** POTWORY **/
                    if(potwory[it].exist == true)
                    {
                        if(potwory[it].poz_x == j+mapa_x && potwory[it].poz_y == i+mapa_y && potwory[it].wystep == act_mapa)
                        {
                            // rectfill(bufor, j*50, i*50, j*50+50, i*50+50, makecol(0,255,255));
                            if(!potwory[it].boss)draw_sprite(bufor,leshen,j*50,i*50);
                            else draw_sprite(bufor,boss,j*50,i*50);

                            if(potwory[it].podpalenie > 0)
                                textprintf(bufor,font, j*50, i*50, makecol(255,0,0), " %d ", potwory[it].hp);
                            else
                                textprintf(bufor,font, j*50, i*50, makecol(255,255,255), " %d ", potwory[it].hp);


                            if(potwory[it].mon_atak == 0)
                                rectfill(bufor, j*50, i*50+35, j*50 + 50, i*50+45, makecol(0,255,0)); /// potwor moze atakowac!
                            else
                                rectfill(bufor, j*50, i*50+35, j*50 + potwory[it].mon_atak, i*50+45, makecol(255,0,0)); /// info czy potwor moze atakowac
                        }
                    }
                }
            }
            else
            {
                // rectfill(bufor, j*50, i*50, j*50+50, i*50+50, makecol(28,31,40));
                // rect(bufor, j*50, i*50, j*50+49, i*50+49, makecol(0,0,0));
                blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
            }

            /** POTWORY **/

        }
    }

};

/************************************ RYSOWANIE MAPY *******************************/



/************************** POTWORY *******************************/
/*** MONSTER KNOCK AARD ***/
void is_monster_knock(int kierunek, int nr_mobka, int ruch, int odl) /// przesuniecie mobka
{
    if(kierunek == 1) /// gora
    {
/// --------------------------
        bool czyruch = true;
        for(int it = 0; it < odl; it++)
        {

            for(int i = 0; i < 20; i++)
            {
                if(potwory[nr_mobka].poz_x == potwory[i].poz_x && potwory[nr_mobka].poz_y+1 == potwory[i].poz_y && nr_mobka != i ||
                        mapa[potwory[nr_mobka].poz_y+1][potwory[nr_mobka].poz_x].valid == 0 ||
                        (potwory[nr_mobka].poz_x == gracz.p_x && potwory[nr_mobka].poz_y+1 == gracz.p_y))
                {
                    czyruch = false;
                }
            }
            if(czyruch == true)
            {
                potwory[nr_mobka].poz_y++;
                potwory[nr_mobka].ruch = ruch;
            }
        }


    }
/// ------------------------------------------
    else if(kierunek == 2) /// prawo
    {
        bool czyruch = true;
        for(int it = 0; it < odl; it++)
        {
            for(int i = 0; i < 20; i++)
            {
                if(potwory[nr_mobka].poz_x+1 == potwory[i].poz_x && potwory[nr_mobka].poz_y == potwory[i].poz_y && nr_mobka != i ||
                        mapa[potwory[nr_mobka].poz_y][potwory[nr_mobka].poz_x+1].valid == 0 ||
                        (potwory[nr_mobka].poz_x+1 == gracz.p_x && potwory[nr_mobka].poz_y == gracz.p_y))
                {
                    czyruch = false;
                }
            }
            if(czyruch == true)
            {
                potwory[nr_mobka].poz_x++;
                potwory[nr_mobka].ruch = ruch;
            }
        }
    }
/// -------------------------------------------
    else if(kierunek == 3) /// dol
    {

        bool czyruch = true;
        for(int it = 0; it < odl; it++)
        {
            for(int i = 0; i < 20; i++)
            {
                if(potwory[nr_mobka].poz_x== potwory[i].poz_x && potwory[nr_mobka].poz_y-1 == potwory[i].poz_y && nr_mobka != i ||
                        mapa[potwory[nr_mobka].poz_y-1][potwory[nr_mobka].poz_x].valid == 0 ||
                        (potwory[nr_mobka].poz_y-1 == gracz.p_y && potwory[nr_mobka].poz_x == gracz.p_x))
                {
                    czyruch = false;
                }
            }
            if(czyruch == true)
            {
                potwory[nr_mobka].poz_y--;
                potwory[nr_mobka].ruch = ruch;
            }
        }

    }
/// -------------------------------------------
    else if(kierunek == 4) /// lewo
    {
        bool czyruch = true;
        for(int it = 0; it < odl; it++)
        {
            for(int i = 0; i < 20; i++)
            {
                if(potwory[nr_mobka].poz_x-1 == potwory[i].poz_x && potwory[nr_mobka].poz_y == potwory[i].poz_y && nr_mobka != i ||
                        mapa[potwory[nr_mobka].poz_y][potwory[nr_mobka].poz_x-1].valid == 0 ||
                        (potwory[nr_mobka].poz_x-1 == gracz.p_x && potwory[nr_mobka].poz_y == gracz.p_y))
                {
                    czyruch = false;
                }
            }
            if(czyruch == true)
            {
                potwory[nr_mobka].poz_x--;
                potwory[nr_mobka].ruch = ruch;
            }
        }
    }
/// -------------------------------------------
}
/// *** ---------------- *** ///

void monster::move_monster(int nr_mobka) /// poruszanie potwora
{
    if(gracz.p_y > poz_y && mapa[poz_y+1][poz_x].valid == 1 )
    {
        is_monster_knock(1,nr_mobka,40,1); /// ruch w gore

    }
    else if(gracz.p_y < poz_y && mapa[poz_y-1][poz_x].valid == 1)
    {
        is_monster_knock(3,nr_mobka,40,1); /// ruch w dol

    }
    else if(gracz.p_x > poz_x && mapa[poz_y][poz_x+1].valid == 1)
    {
        is_monster_knock(2,nr_mobka,40,1); /// ruch w prawo

    }
    else if(gracz.p_x < poz_x && mapa[poz_y][poz_x-1].valid == 1)
    {
        is_monster_knock(4,nr_mobka,40,1); /// ruch w lewo

    }

}

/*** -------------------------------- ***/
/*** MONSTERS ***/
void monster::monsters_attack(int i) /// atak potwora
{
    if(exist == true)
    {
        if(wystep == act_mapa && hp > 0) /// jesli zyje
        {

            int odl_x = poz_x-gracz.p_x;
            int odl_y = poz_y-gracz.p_y; /// ODL GRACZ

            int odl_x_sh = poz_x-shoot.poz_x;
            int odl_y_sh = poz_y-shoot.poz_y; /// POCISK
            if(odl_x <= agr_x && odl_y <= agr_y && ruch == 0)
            {
                move_monster(i); /// porusza sie w strone gracza
            }
            if(odl_x <= range && odl_x >= -range && odl_y <= range && odl_y >= -range ) /// atak
            {

                if(mon_atak == 0)
                {
                    if(grom.czas_dzialania > 0) /// jesli aktywny grom
                    {
                        if((gracz.pancerz+bonusy.def)/1.5 < dmg) /// oslabiony pancerz!

                            if(grom.czas_dzialania > 0)
                                gracz.hp -= dmg-(gracz.pancerz+bonusy.def)/1.5;
                            else
                                gracz.hp -= dmg-(gracz.pancerz+bonusy.def);
                    }
                    else
                    {
                        if(gracz.pancerz+bonusy.def< dmg) /// zwykly pancerz

                            if(grom.czas_dzialania > 0)
                                gracz.hp -= dmg-(gracz.pancerz+bonusy.def);
                            else
                                gracz.hp -= dmg-(gracz.pancerz+bonusy.def);
                    }

                    mon_atak =90;
                }

                if(gracz.pancerz >= dmg) /// jesli pancerz mocniejszy od moba hp -= 1
                    gracz.hp--;


            }

            if(odl_x_sh == 0 && odl_y_sh == 0 && shoot.sila > 0) /// czy kolizja z pociskiem
            {
                if(shoot.rodzaj == 1) /// aard
                {
                    if(shoot.kierunek == 1)
                    {
                        mon_atak = 60;
                        is_monster_knock(3,i,15,2); /// odbicie top
                    }
                    else if(shoot.kierunek == 2)
                    {
                        mon_atak = 60;
                        is_monster_knock(2,i,15,2); /// odbicie right
                    }
                    else if(shoot.kierunek == 3)
                    {
                        mon_atak = 60;
                        is_monster_knock(1,i,15,2); /// odbicie bottom
                    }
                    else if(shoot.kierunek == 4)
                    {
                        mon_atak = 60;
                        is_monster_knock(4,i,15,2); /// odbicie left
                    }
                }
                if(shoot.rodzaj == 2) /// igni
                {
                    podpalenie = 5; /// potwor zostaje podpalony
                }
                hp-=shoot.dmg; /// dmg pocisku
                shoot.sila = 0; /// rozwalenie pocisku
            } /// POCISK
        }

        if(mon_atak > 0)
            mon_atak--;
        if(ruch > 0)
            ruch--;
    }
}

void monster::efekty()
{
    if(exist == true)
    {
        if(podpalenie > 0) /// jesli podpalony
        {
            hp -= gracz.mag_dmg/5; /// zadaje dmg
            podpalenie--;

        }

        cooldown_regen = 60;
    }

}


/*** MONSTERS ***/

/*** POCISKI ***/
void pocisk::wyswietl() /// wyswietlamy pocisk
{
    bool rys_pocisk = false;
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            if(poz_x == j+mapa_x && poz_y == i+mapa_y && sila != 0 && rys_pocisk == false)
            {

                if(rodzaj == 1) draw_sprite(bufor, aard_sign, j*50,i*50);
                else if(rodzaj == 2) draw_sprite(bufor, igni_sign, j*50,i*50);

               // if(sila%4 == 0)
               if(stoper == 0)
                {
                    if(kierunek == 1) /// kierunek top
                        poz_y--;
                    else if(kierunek == 2) /// kierunek right
                        poz_x++;
                    else if(kierunek == 3) /// kierunek down
                        poz_y++;
                    else if(kierunek == 4) /// kierunek left
                        poz_x--;

                       stoper = 5;


                }
                rys_pocisk = true;
            }
        }
    }
}



/*** POCISKI ***/

/**************** INNE ***/
/**************** INNE ***/

/// TWORZENIE MAPY !!!!!
void rys_new_mapa()
{
mapa_x = 0;
mapa_y = 0;
int tab[20][26] = {{0}}; /// wszystkie elementy mapy maja [0]
int act_liczba = 0;
int act_frag = 1;
while(!key[KEY_ESC])
{
//clear_to_color(bufor,makecol(255,0,255));
int liczba = 0;



for(int i = 0; i < 2; i++) /// wyswietla dostepne kafelki
{
    for(int j = 0; j < 5; j++)
    {
    blit( kafelki, bufor, j*50, i*50, j*50,500+(i*50), 50, 50 );

    if(liczba == act_liczba) textprintf(bufor,font, j*50+10, 510+(i*50), makecol(220,20,60), " %d ", liczba);
    else textprintf(bufor,font, j*50+10, 510+(i*50), makecol(255,255,255), " %d ", liczba);

    liczba++;
    }

}
draw_sprite(bufor, leshen, 250,500);
if(act_liczba == 10 ) textout_ex(bufor, font, "Q", 260, 510, makecol(220, 20, 60), 0);
    else textout_ex(bufor, font, "Q", 260, 510, makecol(255,255,255), 0);



for(int i = 0; i < 10; i++) /// wyswietla twoja mape FRAGMENT
{
    for(int j = 0; j < 16; j++)
    {
       //if(tab[i][j] == 0) rect(bufor, j*50, i*50, j*50+49, i*50+49, makecol(255,255,255));
     //  if(rys_kafelek_bool_new_map(mapa_x,mapa_y,j,i))
       //{
       if(tab[i][j] == 0) blit( kafelki, bufor, 0, 0, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 1) blit( kafelki, bufor, 50, 0, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 2) blit( kafelki, bufor, 100, 0, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 3) blit( kafelki, bufor, 150, 0, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 4) blit( kafelki, bufor, 200, 0, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 5) blit( kafelki, bufor, 0, 50, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 6) blit( kafelki, bufor, 50, 50, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 7) blit( kafelki, bufor, 100, 50, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 8) blit( kafelki, bufor, 150, 50, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 9) blit( kafelki, bufor, 200, 50, j*50,i*50, 50, 50 );
       else if(tab[i][j] == 10) draw_sprite(bufor,leshen, j*50, i*50);
      // }

    }
}
if(key[KEY_0]) act_liczba = 0;
else if(key[KEY_1]) act_liczba = 1;
else if(key[KEY_2]) act_liczba = 2;
else if(key[KEY_3]) act_liczba = 3;
else if(key[KEY_4]) act_liczba = 4;
else if(key[KEY_5]) act_liczba = 5;
else if(key[KEY_6]) act_liczba = 6;
else if(key[KEY_7]) act_liczba = 7;
else if(key[KEY_8]) act_liczba = 8;
else if(key[KEY_9]) act_liczba = 9;
else if(key[KEY_Q]) act_liczba = 10;


if(mouse_b == 1)
{
    int mx = mouse_x/50*50;
    int my = mouse_y/50*50;

    //blit( kafelki, bufor, j*50, i*50, jp*50,500+(ip*50), 50, 50 );

    tab[my/50][mx/50] = act_liczba;
}

show_mouse(screen);
draw_sprite(screen,bufor,0,0);
}

fstream nowy_plik;
nowy_plik.open("maps/my_map.txt", ios::out);
for(int i = 0; i < 10; i++)
{
    for(int j = 0; j < 16; j++)
    {
        if(tab[i][j] == 0) nowy_plik<<" 1";
        else if(tab[i][j] == 1) nowy_plik<<" 6";
        else if(tab[i][j] == 2) nowy_plik<<" 8";
        else if(tab[i][j] == 3) nowy_plik<<" 10";
        else if(tab[i][j] == 4) nowy_plik<<" 12";
        else if(tab[i][j] == 5) nowy_plik<<" 7";
        else if(tab[i][j] == 6) nowy_plik<<" 9";
        else if(tab[i][j] == 7) nowy_plik<<" 2";
        else if(tab[i][j] == 8) nowy_plik<<" 4";
        else if(tab[i][j] == 9) nowy_plik<<" 0";
        else if(tab[i][j] == 10) nowy_plik<<" 3";
        //nowy_plik<<tab[i][j]<<" ";
    }
    nowy_plik<<" 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"<<endl;
}
nowy_plik.close();
}

/// MENU
void  pisz()
{
/*rectfill(bufor,25,100,400,175,makecol(0,0,0));*/ textout_ex(bufor, menu_main, "Rozpocznij", 50, 100, makecol(255, 255, 255), -1);
/*rectfill(bufor,25,190,400,265,makecol(0,0,0));*/ textout_ex(bufor, menu_main, "Wczytaj", 50, 190, makecol(255, 255, 255), -1);
/*rectfill(bufor,25,280,400,355,makecol(0,0,0));*/ textout_ex(bufor, menu_main, "Pomoc", 50, 280, makecol(255, 255, 255), -1);
/*rectfill(bufor,25,370,400,445,makecol(0,0,0));*/ textout_ex(bufor, menu_main, "Wyjdz", 50, 370, makecol(255, 255, 255), -1);
}

int startowe(int act_pole)
{
textout_ex(bufor, menu2, "Wiedzmin", 50, 0, makecol(255, 255, 255), -1);

pisz();
switch(act_pole)
{
    case 1: {textout_ex(bufor, menu_main, "Rozpocznij", 50, 100, makecol(255, 0, 0), -1); break;}
    case 2: {textout_ex(bufor, menu_main, "Wczytaj", 50, 190, makecol(255, 0, 0), -1); break;}
    case 3: {textout_ex(bufor, menu_main, "Pomoc", 50, 280, makecol(255, 0, 0), -1); break;}
    case 4: {textout_ex(bufor, menu_main, "Wyjdz", 50, 370, makecol(255, 0, 0), -1); break;}
}

int mx = mouse_x;
int my = mouse_y;

if(mx >= 25 && mx < 300)
{
    if(my >= 100 && my <= 175) {return(1);}
    else if(my >= 190 && my <= 265) {return(2);}
    else if(my >= 280 && my <= 355) {return(3);}
    else if(my >= 370 && my <= 445) {return(4);}
}
return(0);
}



/** FUNKCJE **/
/********* MENU GRY *******/
void rys_escape(int act_wybor)
{
//rectfill(bufor,0,0,800,600,makecol(30,33,42));
draw_sprite(bufor,tlo,0,0);

   textout_ex(bufor, menu2, "Zatrzymales gre", 50, 10, makecol(255, 255, 255), -1);

   textout_ex(bufor, menu_main, "Kontynuuj", 50, 100, makecol(255, 255, 255), -1);

   textout_ex(bufor, menu_main, "Zapisz", 50, 190, makecol(255, 255, 255), -1);

   textout_ex(bufor, menu_main, "Ekwipunek", 50, 280, makecol(255, 255, 255), -1);

   textout_ex(bufor, menu_main, "Wyjdz", 50, 370, makecol(255, 255, 255), -1);

if(act_wybor == 1)
    textout_ex(bufor, menu_main, "Kontynuuj", 50, 100, makecol(255, 0, 0), -1);
else if(act_wybor == 2)
    textout_ex(bufor, menu_main, "Zapisz", 50, 190, makecol(255, 0, 0), -1);
else if (act_wybor == 3)
    textout_ex(bufor, menu_main, "Ekwipunek", 50, 280, makecol(255, 0, 0), -1);
else if (act_wybor == 4)
    textout_ex(bufor, menu_main, "Wyjdz", 50, 370, makecol(255, 0, 0), -1);


}
void escape(bool &exit)
{
    rest(200);
    show_mouse(screen);
    int act_wybor = 1;
    int old_wybor = 1;
    int rest_przes = 0;
    bool wyjsc = true;
    while(wyjsc == true)
    {
        while(speed > 0)
        {
        if(key[KEY_UP] && rest_przes == 0) {act_wybor--; rest_przes = 10;}
     else if(key[KEY_DOWN] && rest_przes == 0) {act_wybor++; rest_przes = 10;}

     if(rest_przes > 0) rest_przes--;


     if(act_wybor > 4) act_wybor = 1;
     else if(act_wybor < 1) act_wybor = 4;

     if(key[KEY_ENTER] || mouse_b == 1)
     {
         if(act_wybor == 1) wyjsc = false;
         else if(act_wybor == 2) {gracz.zapisz(); eq_zapisz(); play_sample( save, 255, 127, 1000, 0 );}
         else if(act_wybor == 3) {gracz.ekwipunek_wys(); speed = 0;}
         else if(act_wybor == 4) {exit = false; gracz.zapisz(); eq_zapisz(); stop_sample( togame ); wyjsc = false; rest(100); }
     }

     if(mouse_x >= 25 && mouse_x < 300)
{
    if(mouse_y >= 100 && mouse_y <= 175) act_wybor = 1;
    else if(mouse_y >= 190 && mouse_y <= 265) act_wybor = 2;
    else if(mouse_y >= 280 && mouse_y <= 355) act_wybor = 3;
    else if(mouse_y >= 370 && mouse_y <= 445) act_wybor = 4;
}
if(act_wybor != old_wybor) {play_sample( najechanie, 255, 127, 1000, 0 ); old_wybor = act_wybor;}
     speed--;
        }


     rys_escape(act_wybor);
     draw_sprite(screen,bufor,0,0);

    }
    rest(200);
}
/********* MENU GRY *******/
/****** ROZP0CZYNIANIE GRY ***/
int gra(int nr_mapy)
{
    bool was_rys = false;
    bool powt6 = false;

play_sample( togame, 125, 127, 1000, 1 );

rest(1000);

bool exit = true;
bool start = false;
int czekaj = 0;

if(act_mapa == 1) przyp_act_mapy("maps/1.txt",1);
else if(act_mapa == 2) przyp_act_mapy("maps/2.txt",2);
else if(act_mapa == 3) przyp_act_mapy("maps/3.txt",3);
else if(act_mapa == 4) przyp_act_mapy("maps/4.txt",4);
else if(act_mapa == 5) przyp_act_mapy("maps/5.txt",5);
else  przyp_act_mapy("maps/boss.txt",6);
show_mouse(screen);

while(exit)
    {

    while(mouse_b == 1 || key[KEY_Q])
    {
        rectfill(bufor,0,0,800,600, makecol(0,0,0));
        textout_ex(bufor, menu2, "Pomoc", 60, 0, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[Z] - ustawia aard jako aktywny znak", 60, 120, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[X] - ustawia igni jako aktywny znak", 60, 170, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[1] - uzywa eliksiru Jaskolka", 60, 220, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[2] - uzywa eliksiru Grom", 60, 270, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[W][S][A][D] -uzywa znaku w danym kierunku", 60, 370, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[I] - wlacza ekwipunek", 60, 320, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "LVL - pokazuje twoj poziom", 400, 120, makecol(220, 20, 60), -1);
        textout_ex(bufor, czcionka, "EXP/EXP - pokazuje twoj exp / wymagany exp", 400, 170, makecol(220, 20, 60), -1);
        textout_ex(bufor, czcionka, "HP - oznacza ilosc hp", 400, 220, makecol(255, 0, 0), -1);
        textout_ex(bufor, czcionka, "MP - oznacza ilosc mp", 400, 270, makecol(0, 0, 255), -1);

textout_ex(bufor, czcionka, "Poziom boss jest dla b. mocnych postaci, sproboj jesli potrafisz!", 60, 420, makecol(255, 255, 255), -1);

        draw_sprite(bufor,aard_sign,400,320);
        textout_ex(bufor, czcionka, "Tu jest portal!!!", 450, 340, makecol(0, 0, 255), -1);
        draw_sprite(screen,bufor,0,0);
        speed = 0;
    }
        clear_to_color(bufor, makecol(30,33,42));

        bonusy.reset();



        if(act_mapa != 0)
        {


            wys_mape(mapa_x,mapa_y);
            int przes_y = 0;
           // if(grom.czas_dzialania%2 == 0 && grom.czas_dzialania > 0) masked_blit(postac, bufor,50,0, 150, 150, 50, 50);
            //else if(grom.czas_dzialania%2 == 1 && grom.czas_dzialania > 0) masked_blit(postac, bufor,100,0, 150, 150, 50, 50);
           // else masked_blit(postac, bufor,0,0, 150, 150, 50, 50);
           if(grom.czas_dzialania > 0) przes_y = 50;
           if(gracz.ruch >= 0 && gracz.ruch <= 5) masked_blit(postac, bufor,0,przes_y, 150, 150, 50, 50);
           else if(gracz.ruch > 5 && gracz.ruch <= 10)
           {
              if(gracz.kierunek == 1) masked_blit(postac, bufor,50,przes_y, 150, 150, 50, 50);
              else if(gracz.kierunek == 2) masked_blit(postac, bufor,100,przes_y, 150, 150, 50, 50);
              else if(gracz.kierunek == 3) masked_blit(postac, bufor,200,przes_y, 150, 150, 50, 50);
              else if(gracz.kierunek == 4) masked_blit(postac, bufor,150,przes_y, 150, 150, 50, 50);

           }
           /// miecz !!!
           if(gracz.attack != 0)
           {
               if(gracz.attack <= 45 && gracz.attack >= 35)masked_blit(atak, bufor,0,0, 150, 150, 50, 50);
               else if(gracz.attack < 35 && gracz.attack >= 25)masked_blit(atak, bufor,50,0, 150, 150, 50, 50);
               else if(gracz.attack < 25 && gracz.attack >= 15)masked_blit(atak, bufor,100,0, 150, 150, 50, 50);
               else if(gracz.attack < 15 && gracz.attack > 0)masked_blit(atak, bufor,150,0, 150, 150, 50, 50);
           }

           /**** RYSOWANIE ODL. DO NASTEPNEGO ATAKU **/
           int procenty_atak = 0;
           if(grom.czas_dzialania > 0) procenty_atak = grom.czas_dzialania*3.3;
           else procenty_atak = gracz.attack*2;

           if(gracz.attack != 0)rectfill(bufor,150,195,150+procenty_atak/2,200,makecol(255,0,0));
           else rectfill(bufor,150,195,200,200,makecol(0,255,0));
           /*** / **/

            rekur_tp(9);
            shoot.wyswietl();
            gracz.wyswietl_staty();

            /*** rysowanie mapy ***/
            while(speed > 0)
            {
            if(key[KEY_ESC])
                {
                   escape(exit);
                   speed = 0;
               }
                /*************************************/
                gracz.dodaj_exp();
                for(int i = 0; i < 20; i++)
                    potwory[i].monsters_attack(i); /// ATAKI POTWOROW

                gracz.regeneracja();

                /// RUCH
                if(gracz.ruch == 0)
                {
                    if(key[KEY_RIGHT])
                    {
                        if(mapa[gracz.p_y][gracz.p_x+1].valid == 1 && czypotwor(2) == 1)
                        {
                            gracz.p_x++;
                            mapa_x++;
                            gracz.ruch = 10;
                            gracz.kierunek = 2;
                        }

                    }
                    else if(key[KEY_LEFT])
                    {
                        if(mapa[gracz.p_y][gracz.p_x-1].valid == 1 && czypotwor(4) == 1)
                        {
                            gracz.p_x--;
                            mapa_x--;
                            gracz.ruch = 10;
                            gracz.kierunek = 4;
                        }

                    }
                    else if(key[KEY_DOWN])
                    {
                        if(mapa[gracz.p_y+1][gracz.p_x].valid == 1 && czypotwor(3) == 1)
                        {
                            gracz.p_y++;
                            mapa_y++;
                            gracz.ruch = 10;
                            gracz.kierunek = 3;
                        }
                    }
                    else if(key[KEY_UP])
                    {
                        if(mapa[gracz.p_y-1][gracz.p_x].valid == 1 && czypotwor(1) == 1)
                        {
                            gracz.p_y--;
                            mapa_y--;
                            gracz.ruch = 10;
                            gracz.kierunek = 1;
                        }
                    }
                }
                else if(gracz.ruch > 0)
                    gracz.ruch--;
                /// RUCH

                if(key[KEY_SPACE] && gracz.attack == 0)
                {
                    gracz.atak();/// ATAK!
                    if(grom.czas_dzialania > 0) gracz.attack = 30;
                    else gracz.attack = 45;
                }

                if(key[KEY_Z] && gracz.cooldown_spell == 0) /// zmiana znaku na AARD
                {
                    gracz.act_znak = 0;
                }
                if(key[KEY_X] && gracz.cooldown_spell == 0) /// zmiana znaku na IGNI
                {
                    gracz.act_znak = 1;
                }

                /****************************************/
                if(key[KEY_W] && gracz.cooldown_spell == 0) /// strzal w gora
                {
                    if(gracz.act_znak == 0)
                        gracz.aard(1);
                    else
                        gracz.igni(1);
                }
                else if(key[KEY_A] && gracz.cooldown_spell == 0) /// strzal w lewo
                {
                    if(gracz.act_znak == 0)
                        gracz.aard(4);
                    else
                        gracz.igni(4);
                }
                else if(key[KEY_S] && gracz.cooldown_spell == 0) /// strzal w dol
                {
                    if(gracz.act_znak == 0)
                        gracz.aard(3);
                    else
                        gracz.igni(3);
                }
                else if(key[KEY_D] && gracz.cooldown_spell == 0) /// strzal w prawo
                {
                    if(gracz.act_znak == 0)
                        gracz.aard(2);
                    else
                        gracz.igni(2);
                }
                /************************************************/

                /********************************************************************/

                if(key[KEY_1] && jaskolka.czas_dzialania == 0 && jaskolka.ilosc > 0)
                    jaskolka.wypij(0);
                else if(key[KEY_2] && grom.czas_dzialania == 0 && grom.ilosc > 0)
                    grom.wypij(1);
                if(key[KEY_I])
                {
                    //gracz.ekwipunek();
                    gracz.ekwipunek_wys();

                    speed = 0;
                }

                /// WLACZANIE I WYLACZANIE DZWIEKU DZWIEK
                 if(key[KEY_P] && p_key == false)
                 {
                     if(is_sound) {is_sound = false; stop_sample(togame);}
                     else {is_sound = true; play_sample( togame, 255, 127, 1000, 1 );}

                     p_key = true;
                 }
                 if(!key[KEY_P])
                 {
                     p_key = false;
                 }

                 if(act_mapa == 6)
                 {
                     for(int i = 0; i < 20; i++)
                     {
                         if(potwory[i].exist == true)
                         {
                             if(potwory[i].boss == true)
                             {
                                 if(potwory[i].hp <= 0)
                                 {
                                    while(!key[KEY_ENTER])
                                    {
                                     draw_sprite(bufor,tek2,0,0);
                                textout_ex(bufor,czcionka, "Kliknij [ENTER] by przejsc dalej", 60, 550, makecol(255, 255, 255), -1);
                                     draw_sprite(screen,bufor,0,0);
                                    }
                                    exit = false; break;
                                }
                                 }
                             }
                         }
                     }
                     if(act_mapa == 6 && was_rys == false)
                        {
                            while(!key[KEY_ENTER])
                            {
                    draw_sprite(bufor,tek1,0,0);
                     textout_ex(bufor,czcionka, "Kliknij [ENTER] by przejsc dalej", 60, 550, makecol(255, 255, 255), -1);
                     draw_sprite(screen,bufor,0,0);
                     was_rys = true;
                            }
                        speed = 0;
                        }

                /** RUCH **/
                /// EFEKTY POTWOROW
                for(int i = 0; i < 20; i++)
                {
                    if(potwory[i].cooldown_regen == 0)
                        potwory[i].efekty();
                    else
                        potwory[i].cooldown_regen--;
                }


                /// EFEKTY POTWOROW


                if(shoot.sila > 0)
                    shoot.sila--;
                if(shoot.stoper > 0)
                    shoot.stoper--;

                if(gracz.attack > 0)gracz.attack--;
                gracz.odnowienie_spelli();

                if(gracz.exp >= gracz.wym_exp)
                {
                    gracz.add_lvl();
                }


                if(gracz.hp+bonusy.hp <= 0) /// Przegrana
            gracz.przegrana(exit);



            if(czekaj < 120) czekaj++;
                speed--;
            }

            draw_sprite(bufor,sound,650,0);
            if(is_sound == false) {rectfill(bufor,650,23,700,27,makecol(0,0,0));
            textout_ex(bufor, czcionka, "[P] by wlaczyc", 650, 50, makecol(255, 255, 255), -1);}
            else textout_ex(bufor, czcionka, "[P] by wylaczyc", 650, 50, makecol(255, 255, 255), -1);


            draw_sprite(screen,bufor,0,0);



            /*** rysowanie mapy ***/

        }


        if(act_mapa == 0)
        {
            rys_new_mapa();
        }
    }
    stop_sample(togame);
//}
}
void pomoc()
{
    while(!key[KEY_ESC])
    {
        rectfill(bufor,0,0,800,600, makecol(0,0,0));
        textout_ex(bufor, menu2, "Pomoc", 60, 0, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[Z] - ustawia aard jako aktywny znak", 60, 120, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[X] - ustawia igni jako aktywny znak", 60, 170, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[1] - uzywa eliksiru Jaskolka", 60, 220, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[2] - uzywa eliksiru Grom", 60, 270, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[W][S][A][D] -uzywa znaku w danym kierunku", 60, 370, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "[I] - wlacza ekwipunek", 60, 320, makecol(255, 255, 255), -1);
        textout_ex(bufor, czcionka, "LVL - pokazuje twoj poziom", 400, 120, makecol(220, 20, 60), -1);
        textout_ex(bufor, czcionka, "EXP/EXP - pokazuje twoj exp / wymagany exp", 400, 170, makecol(220, 20, 60), -1);
        textout_ex(bufor, czcionka, "HP - oznacza ilosc hp", 400, 220, makecol(255, 0, 0), -1);
        textout_ex(bufor, czcionka, "MP - oznacza ilosc mp", 400, 270, makecol(0, 0, 255), -1);

        draw_sprite(bufor,aard_sign,400,320);
        textout_ex(bufor, czcionka, "Tu jest portal!!!", 450, 340, makecol(0, 0, 255), -1);

textout_ex(bufor, czcionka, "Poziom boss jest dla b. mocnych postaci, sproboj jesli potrafisz!", 60, 420, makecol(255, 0, 0), -1);

        textout_ex(bufor, czcionka, "Aby wyjsc kliknij [ESC]", 0, 500, makecol(255, 255, 255), -1);

        draw_sprite(screen,bufor,0,0);
    }
}

int zacznij_gre()
{
    //przyp_act_mapy("maps/1.txt",1);
    //gracz.p_x = 3;
   // gracz.p_y = 3;
    /// ELIKSIRY
    //jaskolka.ilosc = 5;
   // jaskolka.czas_dzialania = 0;
   // grom.ilosc = 3;
   // grom.czas_dzialania = 0;
    /// ELIKSIRY
    gracz.od_nowa();
    gracz.wczytaj(); /// wczytuje dane z pliku
    eq_lista(); /// Przypomnienie ekwipunku
    for(int i = 0; i < 6; i++)
    {
     ekwipunek[i].ilosc = 0;
    }
    eq_zapisz();
    misje_przypisz(); /// wczytuje misje do tabeli
    gra(1);
}


int continue_game(int robic)
{
while(mouse_b == 1)
{
}
gracz.p_x = 3;
gracz.p_y = 3;
mapa_x = 0;
mapa_y = 0;
eq_lista(); /// Przypomnienie ekwipunku
misje_przypisz(); /// wczytuje misje do tabeli
gracz.wczytaj(); /// wczytuje dane z pliku

int act_poz = 1;
int s_act_poz = 1;
int rest_poz = 0;
show_mouse(screen);
rectfill(bufor,50,50,750,550,makecol(0,0,0));
rectfill(bufor,50,20,470,70,makecol(255,0,0));
textout_ex(bufor, czcionka, "Aby zatwierdzic kliknij [X] albo l. przycisk myszy", 60, 30, makecol(255, 255, 255), -1);
rectfill(bufor,50,490,470,550,makecol(255,0,0));
textout_ex(bufor, czcionka, "Aby wyjsc kliknij [ESC]", 60, 500, makecol(255, 255, 255), -1);
if(robic != 0)
{
while(!key[KEY_X] && mouse_b != 1 && !key[KEY_ESC] )
{

textout_ex(bufor, menu2, "Wybierz mape", 150, 90, makecol(255, 255, 255), -1);
textout_ex(bufor, menu_main, "1.txt", 150, 170, makecol(255, 255, 255), -1);
textout_ex(bufor, menu_main, "2.txt", 150, 220, makecol(255, 255, 255), -1);
textout_ex(bufor, menu_main, "3.txt", 150, 270, makecol(255, 255, 255), -1);
textout_ex(bufor, menu_main, "4.txt", 150, 320, makecol(255, 255, 255), -1);
textout_ex(bufor, menu_main, "5.txt", 150, 370, makecol(255, 255, 255), -1);
textout_ex(bufor, menu_main, "boss.txt", 150, 420, makecol(255, 255, 255), -1);

switch(act_poz)
{
    case 1: {textout_ex(bufor, menu_main, "1.txt", 150, 170, makecol(255, 0, 0), -1); break;}
    case 2: {textout_ex(bufor, menu_main, "2.txt", 150, 220, makecol(255, 0, 0), -1); break;}
    case 3: {textout_ex(bufor, menu_main, "3.txt", 150, 270, makecol(255, 0, 0), -1); break;}
    case 4: {textout_ex(bufor, menu_main, "4.txt", 150, 320, makecol(255, 0, 0), -1); break;}
    case 5: {textout_ex(bufor, menu_main, "5.txt", 150, 370, makecol(255, 0, 0), -1); break;}
    case 6: {textout_ex(bufor, menu_main, "boss.txt", 150, 420, makecol(255, 0, 0), -1); break;}
}
while(speed > 0)
{
if(key[KEY_UP] && rest_poz == 0) {act_poz--; rest_poz = 10;}
if(key[KEY_DOWN] && rest_poz == 0) {act_poz++; rest_poz = 10;}
if(rest_poz > 0) rest_poz--;


if(mouse_x < 450 && mouse_x >= 50)
{
    if(mouse_y < 220 && mouse_y > 170) {act_poz = 1;}
    else if(mouse_y < 270 && mouse_y > 220) {act_poz = 2;}
    else if(mouse_y < 320 && mouse_y > 270) {act_poz = 3;}
    else if(mouse_y < 370 && mouse_y > 320) {act_poz = 4;}
    else if(mouse_y < 420 && mouse_y > 370) {act_poz = 5;}
    else if(mouse_y < 450 && mouse_y > 420) {act_poz = 6;}
}

if(act_poz < 1) act_poz = 6;
if(act_poz > 6) act_poz = 1;

if(s_act_poz != act_poz)
{
    s_act_poz = act_poz;
    play_sample( najechanie, 255, 127, 1000, 0 );
}
speed--;
}

draw_sprite(screen,bufor,0,0);
}
if(!key[KEY_ESC])
{
if(act_poz == 1) {act_mapa = 1;}
else if(act_poz == 2) {act_mapa = 2;}
else if(act_poz == 3)  {act_mapa = 3;}
else if(act_poz == 4) {act_mapa = 4;}
else if(act_poz == 5) {act_mapa = 5;}
else if(act_poz == 6) {act_mapa = 6;}

}
}
play_sample( enter, 255, 127, 1000, 0);
gra(act_mapa);

}




void menu_dzialaj()
{
    while(mouse_b == 1){}
    while(key[KEY_ENTER]){}

    int act_pole = 1;
    int old_pole = 1;
    int dont_move = 0;
    int wynik = 1;
    bool exit = true;
    while(exit)
    {
        while(speed > 0)
        {
        if(key[KEY_UP] && dont_move == 0) {act_pole--; dont_move = 10;}
        else if(key[KEY_DOWN]  && dont_move == 0) {act_pole++; dont_move = 10;}

        if(dont_move > 0) dont_move--;

        if(act_pole < 1) act_pole = 4;
        else if(act_pole > 4) act_pole = 1;

        if(wynik != 0) act_pole = wynik;

        speed--;
        }
        //clear_to_color(bufor, makecol(30,33,42));
        draw_sprite(bufor,tlo,0,0);
        wynik = startowe(act_pole);

        if(key[KEY_ENTER] || mouse_b == 1)
        {
            switch(act_pole)
            {
                case 1: {show_mouse(NULL); play_sample( enter, 255, 127, 1000, 0); zacznij_gre(); break;}
                case 2: {show_mouse(NULL); continue_game(1); break;}
                case 3: {show_mouse(NULL); pomoc(); break;}
                case 4: {exit = false; break;}
            }
        }
        if(act_pole != old_pole) {play_sample( najechanie, 255, 127, 1000, 0 ); old_pole = act_pole;}
        show_mouse(screen);
        draw_sprite(screen,bufor,0,0);
    }
}

void player::przegrana(bool &exit) /// PRZEGRYWASZ!!!
{
    stop_sample(togame);
    grom.czas_dzialania = 0;
    jaskolka.czas_dzialania = 0;
    int act_przycisk = 1;
    int old = 1;
    int let_rest = 0;
    show_mouse(screen);
    int mx = mouse_x;
    int my = mouse_y;
    bool wyjsc = true;
    while(wyjsc)
    {
    mx = mouse_x;
    my = mouse_y;
    rectfill(bufor,100,100,700,500,makecol(0,0,0));
    textout_ex(bufor, menu2, "Umarles...", 150, 100, makecol(255, 255, 255), -1);
    textout_ex(bufor, menu_main, "Od nowa", 150, 200, makecol(255, 255, 255), -1);
    textout_ex(bufor, menu_main, "Wroc do zapisu", 150, 270, makecol(255, 255, 255), -1);
    textout_ex(bufor, menu_main, "Wroc do menu", 150, 340, makecol(255, 255, 255), -1);

    if(mx >= 150 && mx <= 400 && my >= 200 && my <= 250) act_przycisk = 1;
    else if(mx >= 150 && mx <= 550 && my >= 270 && my <= 320) act_przycisk = 2;
    else if(mx >= 150 && mx <= 570 && my >= 340 && my <= 390) act_przycisk = 3;

    switch(act_przycisk)
    {
        case 1: {textout_ex(bufor, menu_main, "Od nowa", 150, 200, makecol(255, 0, 0), -1); break;}
        case 2: {textout_ex(bufor, menu_main, "Wroc do zapisu", 150, 270, makecol(255, 0, 0), -1); break;}
        case 3: {textout_ex(bufor, menu_main, "Wroc do menu", 150, 340, makecol(255, 0, 0), -1); break;}
    }

    if(key[KEY_ENTER] || mouse_b)
    {
        switch(act_przycisk)
        {
            case 1: {zacznij_gre(); wyjsc = false;}
            case 2: {wyjsc = false; exit = false; continue_game(0); }
            case 3: {exit = false; wyjsc = false; rest(100);}
        }
    }

    while(speed > 0)
    {
     if(key[KEY_UP] && let_rest == 0) {act_przycisk--; let_rest = 10;}
    if(key[KEY_DOWN] && let_rest == 0) {act_przycisk++; let_rest = 10;}

    if(act_przycisk < 1) act_przycisk = 3;
    else if(act_przycisk > 3) act_przycisk = 1;


    if(let_rest > 0) let_rest--;

    if(act_przycisk != old) {play_sample( najechanie, 255, 127, 1000, 0 ); old = act_przycisk; }

    speed--;
    }

    draw_sprite(screen,bufor,0,0);
    }
    speed = 0;
    exit = false;

}

int main()
{
//allocate_voice( najechanie );
for(int i = 0; i < 8; i++)
{
    misje[i].act = 0;
}

/// TELEPORTY
    for(int i = 0; i < 20; i++)
    {
        teleport[i].wystep = 0;
    }


    teleport[0].poz_x = 31;
    teleport[0].poz_y = 3;
    teleport[0].wystep = 1;
    teleport[0].nr_dest = 2;
    teleport[0].destination = "maps/2.txt";

    teleport[1].poz_x = 40;
    teleport[1].poz_y = 15;
    teleport[1].wystep = 2;
    teleport[1].nr_dest = 3;
    teleport[1].destination = "maps/3.txt";

    teleport[2].poz_x = 45;
    teleport[2].poz_y = 20;
    teleport[2].wystep = 3;
    teleport[2].nr_dest = 4;
    teleport[2].destination = "maps/4.txt";

    teleport[3].poz_x = 35;
    teleport[3].poz_y = 15;
    teleport[3].wystep = 4;
    teleport[3].nr_dest = 5;
    teleport[3].destination = "maps/5.txt";

    teleport[4].poz_x = 38;
    teleport[4].poz_y = 17;
    teleport[4].wystep = 5;
    teleport[4].nr_dest = 1;
    teleport[4].destination = "maps/1.txt";


    init();
    /*** DZWIEKI ***/
    //najechanie = load_sample( "clicksound.wav" );
    /*** czcionki **/
    czcionka = load_font("fonts/allegro.pcx",0,0);
    menu_main = load_font("fonts/menu.pcx",0,0);
    menu2 = load_font("fonts/menu2.pcx",0,0);
    /*** czcionki **/
    bufor = create_bitmap(800,600);
    podloga = load_bmp("img/podloga.bmp",0);
    tlo = load_bmp("img/tlo.bmp",0);
    kafelki = load_bmp("img/tiles.bmp",0);
    postac = load_bmp("img/geralt.bmp",0);
    duzy_geralt = load_bmp("img/duzy_geralt.bmp",0);
    aard_sign = load_bmp("img/aard.bmp",0);
    igni_sign = load_bmp("img/igni.bmp",0);
    jaskolka_eliksir = load_bmp("img/swallow.bmp",0);
    grom_eliksir = load_bmp("img/grom.bmp",0);
    sihill = load_bmp("img/sihill.bmp",0);
    gwyhyr = load_bmp("img/gwyhyr.bmp",0);
    zbroja1 = load_bmp("img/zbroja1.bmp",0);
    zbroja2 = load_bmp("img/zbroja2.bmp",0);
    zbroja3 = load_bmp("img/zbroja3.bmp",0);
    amulet = load_bmp("img/amulet.bmp",0);
    atak = load_bmp("img/SWORD.bmp",0);
    sound = load_bmp("img/sound.bmp",0);
    tek1 = load_bmp("img/tekst1.bmp",0);
    tek2 = load_bmp("img/tekst2.bmp",0);
    /*** POTWORY ***/
    boss = load_bmp("img/triss.bmp",0);
    leshen = load_bmp("img/bies.bmp",0);



    //textout_ex(screen, font, "Hello World!", 0, 0, makecol(255, 255, 255), 0);

    menu_dzialaj(); /// ustawia menu


    destroy_bitmap( bufor );
    destroy_bitmap( podloga );
    destroy_bitmap( tlo );
    destroy_bitmap( kafelki );
    destroy_bitmap( postac );
    destroy_bitmap( duzy_geralt );
    destroy_bitmap( aard_sign );
    destroy_bitmap( igni_sign );
    destroy_bitmap( jaskolka_eliksir );
    destroy_bitmap( grom_eliksir );
    destroy_bitmap( sihill );
    destroy_bitmap( gwyhyr );
    destroy_bitmap( zbroja1 );
    destroy_bitmap( zbroja2 );
    destroy_bitmap( zbroja3 );
    destroy_bitmap( amulet );
    destroy_bitmap( atak );
    destroy_bitmap( sound );
    destroy_bitmap( boss );
    destroy_bitmap( leshen );


    deinit();
    return 0;
}
END_OF_MAIN()

