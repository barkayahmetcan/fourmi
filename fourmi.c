#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

#define FICHIER_BITMAP "carte.bmp"

#define VIDE 0 // case vide (ni nourriture, ni obstacle)
#define OBSTACLE -1
#define FOOD_MAX 40 // FOOD, c’est plus court que NOURRITURE

#define PHEROMONES 1000	   // pour définir une case de pheromone.
#define MAX_PHEROMONES 300 // combien de valeur de pheromones peut etre mis sur une seule case
#define GOUTTE_PHEROMONES 60

#define MODE_RECHERCHE 0 // Modes des fourmis
#define MODE_RETOUR 1

#define NOMBRE_FOURMI 50 // Nombre de fourmi
#define TEMPS_BOUCLE 50	 // temps pour chaque boucle de la simulation

#define ORIENTATION_NORD 0
#define ORIENTATION_NORD_EST 1
#define ORIENTATION_EST 2
#define ORIENTATION_EST_SUD 3
#define ORIENTATION_SUD 4
#define ORIENTATION_SUD_OUEST 5
#define ORIENTATION_OUEST 6
#define ORIENTATION_NORD_OUEST 7

#define NOMBRE_DE_ORIENTATION 8

#define TAILLE_ECRAN 5

#define ORIENTATIONS_CUMUL 5
// const int DISTRI_CUMUL_RECHERCHE[ORIENTATIONS_CUMUL] = {5,13,19,23,24};
int DISTRI_CUMUL_RECHERCHE[ORIENTATIONS_CUMUL] = {55, 70, 74, 0, 0};
int DISTRI_CUMUL_RETOUR[ORIENTATIONS_CUMUL] = {80, 90, 91, 0, 0};

int width, height, FOURMILIERE_C, FOURMILIERE_L;

typedef struct _fourmi fourmi;
struct _fourmi
{
	int i, j, orientation, rotation, mode;
};

void genererCarte(int monde[width][height], SDL_Surface *image);													   // genere la carte en dépendant du bitmap.
int positionCorrecte(int carte[width][height], int i, int j);														   // verifie si une position est correcte.
int orientationFaceTraget(int trajet_i, int trajet_j, int pos_i, int pos_j);										   // donne une orientation face a un trajet en dépendant de pos_i et pos_j.
void modifierPositionAvecOrientation(int *i, int *j, int orientation);												   // cette fonction retourne un nombre entre a et b.
int nouvelOrientationDistriCumul(int DISTRI_CUMUL[ORIENTATIONS_CUMUL], int rotation, int orientation);				   // Donne une nouvel orientation pour une fourmi en dépendant d'une distribution cumulatif.
void bougerFourmis(int carte[width][height], fourmi fourmis[NOMBRE_FOURMI], int *score);							   // boucle pour bouger les fourmi
int randn(int a, int b);																							   // cette fonction retourne un nombre entre a et b.
void afficherMonde(int carte[width][height], fourmi fourmis[NOMBRE_FOURMI], SDL_Window *fenetre, SDL_Renderer *rendu); // affichiage
float distanceEntrePoints(int x1, int y1, int x2, int y2);															   // calcul mathématique pour la distance entre deux points.
int orientationCorrecte(int orientation);																			   // recalcule une orientation correcte dans le cas d'un calcul d'une orientation.
int msleep(long msec);

int main(int argc, char **argv)
{
	SDL_version nb;
	SDL_VERSION(&nb);
	const char *image_path = FICHIER_BITMAP;
	SDL_Surface *image = SDL_LoadBMP(image_path);

	if (!image)
	{
		printf("erreur avec l'image?");
		return 0;
	}

	width = image->w;
	height = image->h;

	int monde[width][height];
	genererCarte(monde, image);
	SDL_FreeSurface(image);

	SDL_Event event;
	SDL_Renderer *rendu;
	SDL_Window *fenetre;

	/*
		Initialisation de l'écran.
	*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(TAILLE_ECRAN * width, TAILLE_ECRAN * height, 0, &fenetre, &rendu);
	SDL_RenderSetScale(rendu, TAILLE_ECRAN, TAILLE_ECRAN);
	SDL_SetRenderDrawBlendMode(rendu, SDL_BLENDMODE_BLEND);

	srand(time(NULL));

	char affichage[80];
	int i, j;
	int score = 0;
	fourmi fourmis[NOMBRE_FOURMI];

	for (i = 0; i < NOMBRE_FOURMI; i++)
	{
		/*
			Déclarons et définition les fourmis,
			par ce que il été dit, on sais que
			l'orientation est l'endroit ou la fourmi se déplace.

			On a des orientation de 0-7, nous mettons une orientation au hasard.
			on donne un sens de rotation préféré de la fourmi (+1 ou -1)

			la fourmi apparait a la fourmiliere et est en mode_recherche.
		*/
		fourmis[i].orientation = randn(0, NOMBRE_DE_ORIENTATION - 1); // On a des orientation de 0-7, nous mettons une orientation au hasard.
		fourmis[i].rotation = randn(0, 1);							  // on donne un sens de rotation préféré de la fourmi (+1 ou -1)
		if (fourmis[i].rotation == 0)
			fourmis[i].rotation = -1; //
		fourmis[i].i = FOURMILIERE_C;
		fourmis[i].j = FOURMILIERE_L;
		fourmis[i].mode = MODE_RECHERCHE;
	}

	while (1)
	{
		// la boucle de la simulation
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) // dans le cas ou la fenetre ferme.
			break;

		for (i = 0; i < width; i++)
		{
			for (j = 0; j < height; j++)
			{
				/*
				Dans la simulation, a chaque boucle, pour une case de pheromone, elle baisse de -1 si il y a de la pheromone.
				*/
				if (monde[i][j] > PHEROMONES)
				{
					monde[i][j]--;
					if (monde[i][j] == PHEROMONES) // la case est complétement vide en phéromones, on la transforme en case vide!
						monde[i][j] = VIDE;
				}
			}
		}
		bougerFourmis(monde, fourmis, &score);
		afficherMonde(monde, fourmis, fenetre, rendu);

		/*
			Affichage du score, /r est utilisé pour éffacé.
			fflush montre directement le message pour que /r ne efface pas et que sa s'affiche.
		*/

		sprintf(affichage, "SCORE : %d", score); //
		SDL_SetWindowTitle(fenetre, affichage);

		msleep(TEMPS_BOUCLE); // le delai pour chaque étape de la simulation en milliseconde
	};
	// dans le cas ou la fenetre ferme, on sort de la boucle while
	SDL_DestroyRenderer(rendu);
	SDL_DestroyWindow(fenetre);
	SDL_Quit();
}
void genererCarte(int monde[width][height], SDL_Surface *image)
{
	Uint8 *pixels = (Uint8 *)image->pixels;
	Uint8 red, green, blue;
	int index, i, j;
	printf("%d", image->pitch);
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			monde[i][j] = 0;
			index = j * image->pitch + i * 3;

			blue = pixels[index];
			green = pixels[index + 1];
			red = pixels[index + 2];

			if (red == 0 && blue == 0 && green == 0)
			{ // obstacle
				monde[i][j] = -1;
			}
			else if (red == 255 && green == 0 && blue == 0)
			{ // nourriture.
				monde[i][j] = FOOD_MAX;
			}
			else if (red == 0 && green == 0 && blue == 255)
			{ // fourmiliére!
				FOURMILIERE_C = i;
				FOURMILIERE_L = j;
			}
		}
	}
}
int positionCorrecte(int carte[width][height], int i, int j)
{
	if (j < 0 || i < 0 || j >= height || i >= width || carte[i][j] == OBSTACLE)
	{ // hors de la carte ou occupé par un obstacle!
		return 0;
	}
	else
		return 1;
}

int orientationFaceTraget(int trajet_i, int trajet_j, int pos_i, int pos_j)
{
	int orientation;
	if (pos_i > trajet_i && pos_j == trajet_j)
	{ // fourmi est a centre et au sud
		orientation = ORIENTATION_NORD;
	}
	else if (pos_i > trajet_i && pos_j < trajet_j)
	{ // fourmi est a ouest et sud
		orientation = ORIENTATION_NORD_EST;
	}
	else if (pos_i == trajet_i && pos_j < trajet_j)
	{ // fourmi est a ouest et centre
		orientation = ORIENTATION_EST;
	}
	else if (pos_i < trajet_i && pos_j < trajet_j)
	{ // fourmi est a ouest et nord
		orientation = ORIENTATION_EST_SUD;
	}
	else if (pos_i < trajet_i && pos_j == trajet_j)
	{ // fourmi est a centre et nord
		orientation = ORIENTATION_SUD;
	}
	else if (pos_i < trajet_i && pos_j > trajet_j)
	{ // fourmi est a est et nord
		orientation = ORIENTATION_SUD_OUEST;
	}
	else if (pos_i == trajet_i && pos_j > trajet_j)
	{ // fourmi est a est et centre
		orientation = ORIENTATION_OUEST;
	}
	else if (pos_i > trajet_i && pos_j > trajet_j)
	{ // fourmi est a est et sud
		orientation = ORIENTATION_NORD_OUEST;
	}
	return orientation;
}

void modifierPositionAvecOrientation(int *i, int *j, int orientation)
{
	/*
		modifie diréctement une position avec une orientation
	*/
	switch (orientation)
	{
	case ORIENTATION_NORD:
		*i -= 1;
		break;
	case ORIENTATION_NORD_EST:
		*i -= 1;
		*j += 1;
		break;
	case ORIENTATION_EST:
		*j += 1;
		break;
	case ORIENTATION_EST_SUD:
		*i += 1;
		*j += 1;
		break;
	case ORIENTATION_SUD:
		*i += 1;
		break;
	case ORIENTATION_SUD_OUEST:
		*j -= 1;
		*i += 1;
		break;
	case ORIENTATION_OUEST:
		*j -= 1;
		break;
	case ORIENTATION_NORD_OUEST:
		*i -= 1;
		*j -= 1;
		break;
	}
}
int nouvelOrientationDistriCumul(int DISTRI_CUMUL[ORIENTATIONS_CUMUL], int rotation, int orientation)
{
	int valeurMax, n;
	for (n = ORIENTATIONS_CUMUL - 1; n >= 0; n--)
	{
		valeurMax = DISTRI_CUMUL[n];
		if (valeurMax > 0)
		{
			int nombre_de_choix = 0;
			n = randn(0, valeurMax);
			while (n > DISTRI_CUMUL[nombre_de_choix])
			{
				nombre_de_choix++;
			}
			return orientationCorrecte(orientation + nombre_de_choix * rotation);
		}
	}
	return 0;
}
void bougerFourmis(int carte[width][height], fourmi fourmis[NOMBRE_FOURMI], int *score)
{
	float dist;
	int i, k, nouvel_i, nouvel_j, nombre_de_choix, maxi;
	int choix_cases[NOMBRE_DE_ORIENTATION][2] = {0};

	for (k = 0; k < NOMBRE_FOURMI; k++)
	{
		/*
		on suis l'article de l'intelligence collective..

		dans le cas du mode de la fourmi en recherche

		-on cherche de la nourriture dans les 8 cases et on détermine la case contenant le plus de nouriture
		-sinon on cherche les 3 cases devant la fourmi contenant le plus de pheromones.
		-sinon nous prenons une nouvel orientation avec une distribution.


		dans le cas du mode de la fourmi en retour
		-on s'oriente a la fourmiliére avec un peu d'hasard

		la fourmi avance devant sa nouvelle orientation.
		*/
		if (fourmis[k].mode == MODE_RECHERCHE)
		{
			maxi = 0;
			nombre_de_choix = -1;
			for (i = 0; i < NOMBRE_DE_ORIENTATION; i++)
			{

				/*
					Recherche des 8 cases autour de la fourmi pour de la nourriture.

					on prend au choix les cases contenant de la nourriture qui contienne la meme quanitié de nourriture des autres choix et on l'ajoute aux choixs disponible,
					dans le cas ou un nouvel choix se présente et est plus grand que celles des autre,
					elle efface tout les choix sauf elle meme.
				*/
				nouvel_i = fourmis[k].i;
				nouvel_j = fourmis[k].j;
				modifierPositionAvecOrientation(&nouvel_i, &nouvel_j, orientationCorrecte(fourmis[k].orientation - i));
				if (positionCorrecte(carte, nouvel_i, nouvel_j) && carte[nouvel_i][nouvel_j] > VIDE && carte[nouvel_i][nouvel_j] < PHEROMONES && carte[nouvel_i][nouvel_j] >= maxi)
				{
					if (carte[nouvel_i][nouvel_j] > maxi)
						nombre_de_choix = -1;
					nombre_de_choix++;
					choix_cases[nombre_de_choix][0] = nouvel_i;
					choix_cases[nombre_de_choix][1] = nouvel_j;
					maxi = carte[nouvel_i][nouvel_j];
				}
			}

			if (nombre_de_choix == -1)
			{
				maxi = 0;
				/*
					Recherche des 3 cases devant la fourmi pour des pheromones.

					on prend au choix les cases contenant des pheromones qui contienne la meme quanitié de pheromones et éloigné de meme distance
					de la fourmiliére des autres choix et on l'ajoute aux choixs disponible,
					dans le cas ou un nouvel choix se présente et est plus loin de la fourmiliére ou est plus grand que celles des autre,
					elle efface tout les choix sauf elle meme.
				*/
				dist = -1;
				for (i = -1; i <= 1; i++)
				{
					nouvel_i = fourmis[k].i;
					nouvel_j = fourmis[k].j;
					modifierPositionAvecOrientation(&nouvel_i, &nouvel_j, orientationCorrecte(fourmis[k].orientation - i));
					if (positionCorrecte(carte, nouvel_i, nouvel_j) && carte[nouvel_i][nouvel_j] > PHEROMONES && distanceEntrePoints(nouvel_i, nouvel_j, FOURMILIERE_C, FOURMILIERE_L) >= dist)
					{
						if (distanceEntrePoints(nouvel_i, nouvel_j, FOURMILIERE_C, FOURMILIERE_L) > dist || carte[nouvel_i][nouvel_j] > maxi)
						{
							nombre_de_choix = -1;
							maxi = carte[nouvel_i][nouvel_j];
						}
						nombre_de_choix++;
						choix_cases[nombre_de_choix][0] = nouvel_i;
						choix_cases[nombre_de_choix][1] = nouvel_j;
						dist = distanceEntrePoints(nouvel_i, nouvel_j, FOURMILIERE_C, FOURMILIERE_L);
					}
				}
			}
			if (nombre_de_choix == -1)
			{
				// aucun pheromones trouvé, nous prenons une orientation au hasard.
				fourmis[k].orientation = nouvelOrientationDistriCumul(DISTRI_CUMUL_RECHERCHE, fourmis[k].rotation, fourmis[k].orientation);
			}
			else
			{
				// dans les choix disponible nous prenons un en hasard.
				nombre_de_choix = randn(0, nombre_de_choix);
				fourmis[k].orientation = orientationFaceTraget(choix_cases[nombre_de_choix][0], choix_cases[nombre_de_choix][1], fourmis[k].i, fourmis[k].j);
			}
		}
		else
		{
			/*
				La fourmi est en mode retour, car elle a de la nourriture.
				pendant son retour elle laisse des goutte de pheromones pour d'autres fourmi!
			*/
			if (carte[fourmis[k].i][fourmis[k].j] == VIDE)
				carte[fourmis[k].i][fourmis[k].j] = PHEROMONES + GOUTTE_PHEROMONES;
			else if (carte[fourmis[k].i][fourmis[k].j] >= PHEROMONES)
			{
				carte[fourmis[k].i][fourmis[k].j] += GOUTTE_PHEROMONES;
				if (MAX_PHEROMONES + PHEROMONES < carte[fourmis[k].i][fourmis[k].j])
					carte[fourmis[k].i][fourmis[k].j] = MAX_PHEROMONES + PHEROMONES;
			}
			fourmis[k].orientation = orientationFaceTraget(FOURMILIERE_C, FOURMILIERE_L, fourmis[k].i, fourmis[k].j);
			fourmis[k].orientation = nouvelOrientationDistriCumul(DISTRI_CUMUL_RETOUR, fourmis[k].rotation, fourmis[k].orientation);
		}

		while (1)
		{
			nouvel_i = fourmis[k].i;
			nouvel_j = fourmis[k].j;
			modifierPositionAvecOrientation(&nouvel_i, &nouvel_j, fourmis[k].orientation);
			if (!positionCorrecte(carte, nouvel_i, nouvel_j))
			{
				fourmis[k].orientation = orientationCorrecte(fourmis[k].orientation + fourmis[k].rotation);
			}
			else
			{
				break;
			}
		}
		fourmis[k].i = nouvel_i;
		fourmis[k].j = nouvel_j;
		if (fourmis[k].mode == MODE_RECHERCHE)
		{
			if (carte[nouvel_i][nouvel_j] > VIDE && carte[nouvel_i][nouvel_j] <= FOOD_MAX)
			{
				fourmis[k].mode = MODE_RETOUR;
				carte[nouvel_i][nouvel_j]--;
			}
		}
		else
		{
			if (nouvel_i == FOURMILIERE_C && nouvel_j == FOURMILIERE_L)
			{
				/*
					On est retourné a la fourmiliére.
					mettre la mode de la fourmi en mode recherche et faire demi tour
					incrémenté le score de 1.

				*/
				fourmis[k].mode = MODE_RECHERCHE;
				fourmis[k].orientation = orientationCorrecte(fourmis[k].orientation + NOMBRE_DE_ORIENTATION / 2);
				*score += 1;
			}
		}
	}
}
int randn(int a, int b)
{
	return rand() % (b + 1 - a) + a;
}
void afficherMonde(int carte[width][height], fourmi fourmis[NOMBRE_FOURMI], SDL_Window *fenetre, SDL_Renderer *rendu)
{

	int chaineIndice = 0;
	int trouverFourmi = 0;
	int i, n, j;

	int v = 0;
	SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255);
	SDL_RenderClear(rendu);
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			if (i == FOURMILIERE_C && j == FOURMILIERE_L)
			{
				SDL_SetRenderDrawColor(rendu, 0, 0, 255, 255);
			}
			else
			{
				switch (carte[i][j])
				{
				case OBSTACLE:
					SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
					break;
				default:
					trouverFourmi = 0;
					for (n = 0; n < NOMBRE_FOURMI; n++)
					{
						if (fourmis[n].i == i && fourmis[n].j == j)
						{
							trouverFourmi = 1;
							if (fourmis[n].mode == MODE_RECHERCHE)
							{
								SDL_SetRenderDrawColor(rendu, 0, 100, 0, 255);
							}
							else
							{
								SDL_SetRenderDrawColor(rendu, 0, 255, 0, 255);
							}
							break; // sort de la boucle
						}
					}
					if (trouverFourmi == 0)
					{
						if (carte[i][j] == VIDE)
							continue; // on dessine rien.
						if (carte[i][j] > VIDE && carte[i][j] <= FOOD_MAX)
						{
							SDL_SetRenderDrawColor(rendu, 255, 0, 0, (carte[i][j] * 155) / FOOD_MAX + 100);
						}
						else if (carte[i][j] >= PHEROMONES)
						{
							SDL_SetRenderDrawColor(rendu, 255, 175, 0, ((carte[i][j] - PHEROMONES) * 255) / MAX_PHEROMONES);
						}
					}
					break;
				}
			}
			SDL_RenderDrawPoint(rendu, i, j);
		}
	}
	SDL_RenderPresent(rendu);
}

float distanceEntrePoints(int x1, int y1, int x2, int y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
int orientationCorrecte(int orientation)
{
	/*
		Résolu une orientation dans le cas d'un calcul.
	*/
	while (orientation < 0)
		orientation += NOMBRE_DE_ORIENTATION;
	orientation %= NOMBRE_DE_ORIENTATION;
	return orientation;
}

#include <errno.h>
int msleep(long msec)
{
	/*
		implémentation de sleep pour des millisecondes..
	*/
	struct timespec ts;
	int res;

	if (msec < 0)
	{
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	do
	{
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);
	return res;
}
