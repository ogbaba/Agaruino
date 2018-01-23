#include <Gamebuino.h>
#include <SPI.h>

Gamebuino gb;

struct boule {
    byte est_ia, est_nourr;
    float taille=1, vx, vy, x, y;
};
#define T_CARTE_MAX_X 100
#define T_CARTE_MAX_Y 100
#define VIT_MAX 1.2
#define NB_JOUEURS 16


// En solo, le joueur 1 est l'élément 0 de ce tableau
int num_joueur = 0;
struct boule joueurs [NB_JOUEURS];


// En jeu ou non ?
byte en_jeu = 0;
byte gagne = 0;
void menu();
void afficher();
void bouger_ia(struct boule * joueur);
void manger(struct boule * mangeur, struct boule * proie);
void jouer();
void gerer_boutons();
void accelerer(float x, float y);
void gerer_gagner();

void setup(){
    gb.begin();
    gb.titleScreen(F("Agaruino"));
}

void loop(){
  if(gb.update()){
      if (!en_jeu) menu();
      else {
	  jouer();
	  afficher();
      }
  }
}

void menu() {
    gb.display.println("Agaruino !\nA pour lancer");
    if (gagne) gb.display.println("Ah, quelqu'un a gagne");
    if (gb.buttons.pressed(BTN_A))
    {
	for (int i=0; i<NB_JOUEURS; ++i)
	{
	    joueurs[i].x = random(T_CARTE_MAX_X);
	    joueurs[i].y = random(T_CARTE_MAX_Y);
	    if (i != num_joueur)
	    {
		if (random(2))
		{
		    joueurs[i].est_nourr = 1;
		    joueurs[i].est_ia = 0;
		    joueurs[i].taille = 1;
		} else {
		    joueurs[i].est_ia = 1;
		    joueurs[i].est_nourr = 0;
		    joueurs[i].taille = 2;
		}
	    } else {
		joueurs[num_joueur].est_ia = 0;
		joueurs[num_joueur].est_nourr = 0;
		joueurs[num_joueur].taille = 2;
	    }
	}
	en_jeu = 1;
	gagne = 0;
    }
}

void afficher() {
    gb.display.print("Taille : ");
    gb.display.println(joueurs[num_joueur].taille);
    for (int i=0; i<NB_JOUEURS; i++)
    {
	if ((joueurs[i].x - joueurs[i].taille < joueurs[num_joueur].x - LCDWIDTH/2)
	    ||(joueurs[i].x + joueurs[i].taille > joueurs[num_joueur].x + LCDWIDTH/2)
	    ||(joueurs[i].y - joueurs[i].taille < joueurs[num_joueur].y - LCDHEIGHT/2)
	    ||(joueurs[i].y + joueurs[i].taille < joueurs[num_joueur].y + LCDHEIGHT/2))
	{
	    gb.display.fillCircle(joueurs[i].x - joueurs[num_joueur].x + LCDWIDTH/2,
				  joueurs[i].y - joueurs[num_joueur].y + LCDHEIGHT/2,
				  joueurs[i].taille);
	}
    }
}

void accelerer(struct boule * joueur, float vx, float vy) {
    float vmax = VIT_MAX - joueur->taille / 20;
    joueur->vx = min(joueur->vx + vx, vmax);
    joueur->vx = max(joueur->vx + vx, -vmax);
    joueur->vy = min(joueur->vy + vy, vmax);
    joueur->vy = max(joueurs->vy + vy, -vmax);
}

void gerer_boutons() {
    if (gb.buttons.repeat(BTN_DOWN,10))
	accelerer(&joueurs[num_joueur],0,0.5);
    if (gb.buttons.repeat(BTN_UP,10))
	accelerer(&joueurs[num_joueur],0,-0.5);
    if (gb.buttons.repeat(BTN_RIGHT,10))
	accelerer(&joueurs[num_joueur],0.5,0);
    if (gb.buttons.repeat(BTN_LEFT,10))
	accelerer(&joueurs[num_joueur],-0.5,0);
    if (gb.buttons.pressed(BTN_C))
    {
	en_jeu = 0;
	gb.titleScreen(F("Agaruino"));
    }
}

void manger(struct boule * mangeur, struct boule * proie)
{
    if (mangeur->taille > proie->taille)
    {
	mangeur->taille += proie->taille/4;
	proie->x = random(T_CARTE_MAX_X);
	proie->y = random(T_CARTE_MAX_Y);
	if (proie->est_nourr) proie->taille = 1;
	else proie->taille = 2;
	gb.sound.playOK();
    }
}

void gerer_repas() {
    for (int i=0; i<NB_JOUEURS; ++i)
    {
	for (int j=0; j<NB_JOUEURS; ++j)
	{
	    if (i == j) continue;
	    if (gb.collidePointRect(joueurs[j].x, joueurs[j].y,
				   joueurs[i].x - joueurs[i].taille,
				   joueurs[i].y - joueurs[i].taille,
				   joueurs[i].taille * 2, joueurs[i].taille * 2)
		&& (joueurs[i].taille > joueurs[j].taille))
		manger(&joueurs[i], &joueurs[j]);	
	}
    }
}
void bouger_ia (struct boule * joueur) {
    if (!random(10))
	accelerer(joueur,random(2) - 0.5,
	      random(2) - 0.5);
}

void gerer_deplacements() {
    for (int i=0; i<NB_JOUEURS; ++i)
    {
	if (joueurs[i].est_ia)
	    bouger_ia(&joueurs[i]);
	joueurs[i].x += joueurs[i].vx;
	joueurs[i].y += joueurs[i].vy;
	joueurs[i].x = min(T_CARTE_MAX_X, joueurs[i].x);
	joueurs[i].x = max(0, joueurs[i].x);
	joueurs[i].y = min(T_CARTE_MAX_Y, joueurs[i].y);
	joueurs[i].y = max(0, joueurs[i].y);
    }
}

void gerer_gagner() {
    for (int i=0; i<NB_JOUEURS; ++i)
	if (joueurs[i].taille > 20)
	{
	    en_jeu = 0;
	    gagne = 1;
	}
}

void jouer() {
    gerer_boutons();
    gerer_deplacements();
    gerer_repas();
    gerer_gagner();
}
